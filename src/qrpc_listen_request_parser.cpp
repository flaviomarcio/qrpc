#include "./qrpc_listen_request_parser.h"
#include "./private/p_qrpc_listen_request_code.h"
#include "./private/p_qrpc_util.h"
#include "./qrpc_controller.h"
#include "./qrpc_listen_request.h"
#include <QByteArray>
#include <QDebug>
#include <QMetaMethod>
#include <QMetaType>
#include <QMutex>
#include <QThread>
#include <QUuid>
#include <QVariantHash>

namespace QRpc {

typedef QHash<QString, QString> StringStringMap;
typedef QMultiHash<QString, QMetaMethod> StringMetaMethod;

Q_GLOBAL_STATIC(StringStringMap, staticMetaObjectRoute)
Q_GLOBAL_STATIC(StringMetaMethod, staticMetaObjectMetaMethod)
Q_GLOBAL_STATIC(QMutex, staticMetaObjectLock)

class ListenRequestParserPvt:public QObject
{
public:
    Controller *controller = nullptr;
    QStringList basePathList;
    explicit ListenRequestParserPvt(QObject *parent = nullptr):QObject{parent} {}
};

ListenRequestParser::ListenRequestParser(QObject *parent) : QObject{parent}, QRpcPrivate::AnotationsExtended{this}
{
    this->p = new ListenRequestParserPvt{parent};
}

QStringList &ListenRequestParser::basePath() const
{
    if(!p->basePathList.isEmpty())
        return p->basePathList;

    auto &annotations=this->annotation();
    const auto &annotation = annotations.find(apiBasePath());
    QVariantList vList;
    if(annotation.isValid()){
        auto v = annotation.value();
        switch (v.typeId()) {
        case QMetaType::QStringList:
        case QMetaType::QVariantList:{
            vList=v.toList();
            break;
        }
        default:
            vList.append(v);
        }

        for (auto &row : vList) {
            auto line = row.toString().trimmed().toLower();
            if (line.isEmpty())
                continue;
            p->basePathList.append(line);
        }
    }
    if(p->basePathList.isEmpty())
        p->basePathList.append(QStringList{QStringLiteral("/")});
    return p->basePathList;
}

Controller &ListenRequestParser::controller()
{
    return *p->controller;
}

ListenRequest &ListenRequestParser::request()
{
    if (p->controller == nullptr) {
        static ListenRequest req;
        req.clear();
        return req;
    }
    return p->controller->request();
}

ListenRequest &ListenRequestParser::rq()
{
    return this->request();
}

bool ListenRequestParser::canRoute(const QMetaObject &metaObject, const QString &route)
{
    auto className = QByteArray(metaObject.className());
    QString staticRoute = staticMetaObjectRoute->value(className);
    auto route1 = QRpc::Util::routeParser(staticRoute).toLower();
    auto route2 = QRpc::Util::routeExtract(route).toLower();
    if (route == route2 || route == route2 || route1 == route2)
        return true;
    return false;
}

bool ListenRequestParser::routeToMethod(const QMetaObject &metaObject,
                                        const QString &route,
                                        QMetaMethod &outMethod)
{
    auto vRoute = QRpc::Util::routeParser(route).split(QByteArrayLiteral("/")).join(QByteArrayLiteral("/"));
    if (ListenRequestParser::canRoute(metaObject, vRoute)) {
        auto list = staticMetaObjectMetaMethod->values(metaObject.className());
        auto v0 = QRpc::Util::routeExtractMethod(route);
        for (auto &v : list) {
            auto v1 = QByteArray(v.name()).toLower();
            if (v0 != v1)
                continue;
            outMethod = v;
            return outMethod.isValid();
        }
    }
    outMethod = QMetaMethod();
    return false;
}

void ListenRequestParser::initializeInstalleds(const QMetaObject &metaObject)
{
    QScopedPointer<QObject> scopePointer(metaObject.newInstance(Q_ARG(QObject*, nullptr)));
    auto object = scopePointer.data();
    if (object == nullptr)
        return;

    auto parser = dynamic_cast<ListenRequestParser *>(object);
    if (parser == nullptr)
        return;

    static const auto ignoreNames = QStringList{QStringLiteral("route"), QStringLiteral("initializeInstalleds")};
    auto className = QByteArray(metaObject.className());
    if (staticMetaObjectRoute->contains(className))
        return;

    auto routeList = parser->basePath();

    QMutexLocker<QMutex> locker(staticMetaObjectLock);
    for(auto &route:routeList){
        staticMetaObjectRoute->insert(className, route);
        for (int methodIndex = 0; methodIndex < metaObject.methodCount(); ++methodIndex) {
            auto method = metaObject.method(methodIndex);
            if (method.returnType() != QMetaType::Bool)
                continue;

            if (method.parameterCount() > 0)
                continue;

            if (ignoreNames.contains(method.name()))
                continue;

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
            staticMetaObjectMetaMethod->insert(className, method);
#else
            staticMetaObjectMetaMethod->insertMulti(className, method);
#endif
        }
    }
}

bool ListenRequestParser::parse(const QMetaMethod &metaMethod)
{
    bool returnVariant = false;
    auto argReturn = Q_RETURN_ARG(bool, returnVariant);
    if (!metaMethod.invoke(this, Qt::DirectConnection, argReturn)) {
#if Q_RPC_LOG
        rWarning() << "Parser not valid " << className;
#endif
        return this->rq().co().setInternalServerError().isOK();
    }

    if (!returnVariant) { //Unauthorized
#if Q_RPC_LOG
        rWarning() << "Parser validation error " << className;
#endif
        return this->rq().co().setInternalServerError().isOK();
    }

    return true;
}

void ListenRequestParser::setController(Controller *value)
{
    p->controller = value;
}

} // namespace QRpc
