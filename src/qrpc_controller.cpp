#include "./qrpc_controller.h"
#include "./private/p_qrpc_util.h"
#include "./qrpc_listen_request_parser.h"
#include "./qrpc_request.h"
#include "./qrpc_server.h"
#include "./qrpc_startup.h"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QList>
#include <QMetaMethod>
#include <QMutex>
#include <QVector>

namespace QRpc {

typedef QMultiHash<QByteArray, QRpc::MethodsMultHash> MultStringMethod;

Q_GLOBAL_STATIC(MetaObjectVector, staticInstalled);
Q_GLOBAL_STATIC(MetaObjectVector, staticParserInstalled);

static void initBasePathParser()
{
    for (auto &metaObject : *staticParserInstalled)
        ListenRequestParser::initializeInstalleds(*metaObject);
}

static void init()
{
    initBasePathParser();
}

Q_RPC_STARTUP_FUNCTION(init);

class ControllerPvt
{
public:
    QStringList basePathList;
    Server *server = nullptr;
    ListenRequest *request = nullptr;
    ControllerSetting setting;
    bool enabled = true;

    ListenRequest ____static_request;

    explicit ControllerPvt() {}

    virtual ~ControllerPvt() {}
};

Controller::Controller(QObject *parent) : QObject{parent}, QRpcPrivate::NotationsExtended{this}
{
    this->p = new ControllerPvt();
}

Controller::~Controller()
{

}

Controller::MethodInfoCollection Controller::invokableMethod() const
{
    static QHash<QByteArray, MethodInfoCollection> controllerMethods;

    auto metaObject=this->metaObject();
    auto className = QByteArray(metaObject->className()).toLower().trimmed();

    MethodInfoCollection __return = controllerMethods.value(className);
    if (!__return.isEmpty())
        return __return;

    auto controller=this;
    auto&n=this->notation();

    if (n.contains(controller->apiRedirect))
        return {};

    static auto nottionExcludeMethod=QVariantList{controller->rqRedirect, controller->rqExcludePath};
    const auto &vBasePathList = controller->basePath();
    if (vBasePathList.isEmpty())
        return {};




    static QStm::Network network;
    static ByteArrayVector methodBlackList=QRPC_METHOD_BACK_LIST;
    for(auto basePath:vBasePathList){
        basePath=qsl("/")+basePath+qsl("/");
        while(basePath.contains(qsl("//")))
            basePath=basePath.replace(qsl("//"),qsl("/"));
        for (auto i = 0; i < metaObject->methodCount(); ++i) {
            auto method = metaObject->method(i);
            MethodInfo info;

            if (method.methodType() != method.Method)
                continue;

            if (method.parameterCount() > 0)
                continue;

            info.method = method;
            info.name = method.name().toLower();

            if (methodBlackList.contains(info.name))
                continue;

            if (info.name.startsWith(qbl("_"))) //ignore methods with [_] in start name
                continue;

            info.notations=QNotation::Collection{controller->notation(info.method)};
            info.excluded=info.notations.contains(nottionExcludeMethod);
            if(info.excluded)
                continue;


            if(info.notations.contains(opCrud)){
                info.methods.append(network.METHOD_GET);
                info.methods.append(network.METHOD_POST);
                info.methods.append(network.METHOD_PUT);
                info.methods.append(network.METHOD_DELETE);
                info.methods.append(network.METHOD_HEAD);
                info.methods.append(network.METHOD_OPTIONS);
            }
            else{
                if(info.notations.contains(opGet))
                    info.methods.append(network.METHOD_GET);

                if(info.notations.contains(opPost))
                    info.methods.append(network.METHOD_POST);

                if(info.notations.contains(opPut))
                    info.methods.append(network.METHOD_PUT);

                if(info.notations.contains(opDelete))
                    info.methods.append(network.METHOD_DELETE);

                if(info.notations.contains(opHead))
                    info.methods.append(network.METHOD_HEAD);

                if(info.notations.contains(opOptions))
                    info.methods.append(network.METHOD_OPTIONS);
            }

            if(info.notations.contains(opTrace))
                info.methods.append(network.METHOD_TRACE);

            if(info.notations.contains(opPatch))
                info.methods.append(network.METHOD_PATCH);

            info.rules=info.notations.find(opRules()).toVariant().toStringList();
            info.basePath=basePath.toUtf8();
            info.path=info.notations.find(opPath()).toValueByteArray();
            if(info.path.isEmpty())
                info.path=info.name;

            info.name=info.notations.find(opName()).toValueByteArray();
            if(info.path.isEmpty())
                info.path=info.path;

            info.description=info.notations.find(opDescription()).toValueByteArray();
            if(info.description.isEmpty())
                info.description=info.name;

            info.group=info.notations.find(opGroup()).toValueByteArray();
            if(info.group.isEmpty())
                continue;

            auto fullPath=qsl("%1/%2").arg(info.basePath, info.path);
            while(fullPath.contains(qsl("//")))
                fullPath=fullPath.replace(qsl("//"),qsl("/"));
            info.fullPath=fullPath.toUtf8();
            __return.append(info);
        }
    }
    return __return;
}

QStringList &Controller::basePath() const
{

    if(!p->basePathList.isEmpty())
        return p->basePathList;

    auto &notations=this->notation();
    const auto &notation = notations.find(apiBasePath());
    QVariantList vList;
    if(notation.isValid()){
        auto v = notation.value();
        switch (qTypeId(v)) {
        case QMetaType_QStringList:
        case QMetaType_QVariantList:{
            vList=v.toList();
            break;
        }
        default:
            vList<<v;
        }

        for (auto &row : vList) {
            auto line = row.toString().trimmed().toLower();
            if (line.isEmpty())
                continue;
            p->basePathList<<line;
        }
    }
    if(p->basePathList.isEmpty())
        p->basePathList<<QStringList{qsl("/")};
    return p->basePathList;
}

QString Controller::module() const
{
    auto &notations=this->notation();
    const auto &notation = notations.find(apiModule());
    if(notation.isValid())
        return notation.value().toString();
    return {};
}

QUuid Controller::moduleUuid() const
{
    return {};
}

QString Controller::description() const
{
    const auto &notations = this->notation();
    const auto &notation = notations.find(apiDescription());

    if(!notation.isValid())
        return notation.value().toString();

    return {};
}

ControllerSetting &Controller::setting()
{

    return p->setting;
}

bool Controller::enabled() const
{

    return p->enabled;
}

void Controller::setEnabled(bool enabled)
{

    p->enabled = enabled;
}

ListenRequest &Controller::request()
{

    return *p->request;
}

ListenRequest &Controller::rq()
{

    return (p->request == nullptr) ? (p->____static_request) : (*p->request);
}

bool Controller::requestSettings()
{
    auto &rq = this->rq();
    auto vHearder = rq.requestHeader();
    {
        QVariantHash vHash;
        QHashIterator<QString, QVariant> i(vHearder);
        while (i.hasNext()) {
            i.next();
            vHash.insert(QRpc::Util::headerFormatName(i.key()), i.value());
        }
        vHash.remove(qsl("host"));
        vHearder = vHash;
    }

    QVariantHash vHearderResponse;
    auto insertHeaderResponse = [&vHearderResponse](const QString &v0, const QString &v1) {
        const auto key = QRpc::Util::headerFormatName(v0);
        const auto value = v1.trimmed();
        if (!key.isEmpty() && !value.isEmpty())
            vHearderResponse.insert(key, value);
    };

    auto header_application_json = vHearder.value(ContentTypeName).toString().trimmed();
    auto origin = vHearder.value(qsl("Origin")).toString();
    auto referer = vHearder.value(qsl("Referer")).toString();
    auto connectionType = vHearder.value(qsl("Connection-Type")).toString();
    auto connection = vHearder.value(qsl("Connection")).toString();
    auto accessControlRequestHeaders = vHearder.value(qsl("Access-Control-Request-Headers"))
                                           .toString();
    auto accessControlAllowMethods = vHearder.value(qsl("Access-Control-Request-Method")).toString();
    auto accessControlAllowOrigin = origin.isEmpty() ? referer : origin;
    auto keepAlive = vHearder.value(qsl("Keep-Alive")).toString();

    if (rq.isMethodOptions()) {
        //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        //https://developer.mozilla.org/pt-BR/docs/Web/HTTP/Methods/OPTIONS
        header_application_json = (!header_application_json.isEmpty()) ? header_application_json
                                                                       : qsl("text/javascripton");
        insertHeaderResponse(qsl("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
        insertHeaderResponse(qsl("Server"), this->server()->serverName());
        insertHeaderResponse(qsl("Access-Control-Allow-Origin"), accessControlAllowOrigin);
        insertHeaderResponse(qsl("Access-Control-Allow-Methods"), accessControlAllowMethods);
        insertHeaderResponse(qsl("Access-Control-Allow-Headers"), accessControlRequestHeaders);
        insertHeaderResponse(qsl("Access-Control-Max-Age"), qsl("86400"));
        insertHeaderResponse(qsl("Vary"), qsl("Accept-Encoding, Origin"));
        insertHeaderResponse(qsl("Keep-Alive"), keepAlive);
        insertHeaderResponse(qsl("Connection"), connection);

        rq.setResponseHeader(vHearderResponse);
        rq.co().setOK();
        return rq.co().isOK();
    }

    //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
    header_application_json = (!header_application_json.isEmpty()) ? header_application_json
                                                                   : qsl("text/javascripton");
    insertHeaderResponse(qsl("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
    insertHeaderResponse(qsl("Server"), this->server()->serverName());
    insertHeaderResponse(qsl("Access-Control-Allow-Origin"), accessControlAllowOrigin);
    insertHeaderResponse(qsl("Vary"), qsl("Accept-Encoding, Origin"));
    insertHeaderResponse(qsl("Keep-Alive"), keepAlive);
    insertHeaderResponse(qsl("Connection"), connection);
    insertHeaderResponse(qsl("Connection-Type"), connectionType);

    rq.setResponseHeader(vHearderResponse);
    rq.co().setOK();
    return rq.co().isOK();
}

bool Controller::canOperation(const QMetaMethod &method)
{

    if (p->request == nullptr)
        return {};

    if (this->rq().isMethodOptions())
        return true;

    auto &rq = *p->request;
    const auto &notations = this->notation(method);

    if(notations.isEmpty())
        return true;

    if (notations.contains(rqRedirect))
        return true;

    if (notations.contains(rqExcludePath))
        return false;

    if (!notations.containsClassification(ApiOperation))
        return true;

    if (notations.contains(opCrud))
        return true;

    auto operation = qsl("op%1").arg(QString::fromUtf8(rq.requestMethod())).toLower();
    if (notations.contains(operation))
        return true;

    return {};
}

bool Controller::canAuthorization()
{
    if (this->rq().isMethodOptions())
        return true;

    const auto &notations = this->notation();

    if (!notations.containsClassification(Security))
        return true;

    if (notations.contains(this->rqSecurityIgnore))
        return true;

    return false;
}

bool Controller::canAuthorization(const QMetaMethod &method)
{
    if (this->rq().isMethodOptions())
        return true;

    const auto &notations = this->notation(method);

    if (!notations.containsClassification(Security))
        return true;

    if (notations.contains(this->rqSecurityIgnore))
        return true;

    return false;
}

bool Controller::beforeAuthorization()
{
    return true;
}

bool Controller::authorization()
{
    return true;
}

bool Controller::authorization(const QMetaMethod &method)
{
    const auto &notations = this->notation(method);
    if (!notations.containsClassification(Security))
        return true;

    if (notations.contains(this->rqSecurityIgnore))
        return true;

    return false;
}

bool Controller::afterAuthorization()
{
    return true;
}

bool Controller::requestBeforeInvoke()
{
    return true;
}

bool Controller::requestAfterInvoke()
{
    return true;
}

Server *Controller::server()
{

    return p->server;
}

const QMetaObject &Controller::install(const QMetaObject &metaObject)
{
    if (!staticInstalled->contains(&metaObject)) {
#if Q_RPC_LOG_VERBOSE
        if (staticInstalled->isEmpty())
            sInfo() << qsl("interface registered: ") << metaObject.className();
        qInfo() << qbl("interface: ") + metaObject.className();
#endif
        (*staticInstalled) << &metaObject;
    }
    return metaObject;
}

const QMetaObject &Controller::installParser(const QMetaObject &metaObject)
{
    if (!staticParserInstalled->contains(&metaObject)) {
#if Q_RPC_LOG_VERBOSE
        if (staticParserInstalled->isEmpty())
            sInfo() << qsl("parser interface registered: ") << metaObject.className();
        qInfo() << qbl("parser interface") + metaObject.className();
#endif
        (*staticParserInstalled) << &metaObject;
    }
    return metaObject;
}

QVector<const QMetaObject *> &Controller::staticApiList()
{
    return *staticInstalled;
}

QVector<const QMetaObject *> &Controller::staticApiParserList()
{
    return *staticParserInstalled;
}

Controller &Controller::setServer(Server *server)
{

    p->server = server;
    return *this;
}

Controller &Controller::setRequest(ListenRequest &request)
{

    p->request = &request;
    return *this;
}

} // namespace QRpc
