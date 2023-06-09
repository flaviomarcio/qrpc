#include "./qrpc_controller.h"
#include "./private/p_qrpc_util.h"
#include "./qrpc_listen_request_parser.h"
#include "./qrpc_const.h"
#if Q_RPC_LOG
#include "./qrpc_macro.h"
#endif
#include "./qrpc_server.h"
#include "./qrpc_startup.h"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QList>
#include <QMetaMethod>
#include <QMutex>
#include <QVector>
#include <QHash>

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

class ControllerPvt:public QObject
{
public:
    QStringList basePathList;
    Server *server = nullptr;
    ListenRequest *request = nullptr;
    ControllerSetting setting;
    bool enabled = true;

    ListenRequest ____static_request;

    explicit ControllerPvt(QObject *parent):QObject{parent} {}
};

Controller::Controller(QObject *parent) : QObject{parent}, QRpcPrivate::AnotationsExtended{this}
{
    this->p = new ControllerPvt{this};
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
    auto &ann=this->annotation();

    if (ann.contains(controller->apiRedirect))
        return {};

    static auto annotionExcludeMethod=QVariantList{controller->rqRedirect, controller->rqExcludePath};
    const auto &vBasePathList = controller->basePath();
    if (vBasePathList.isEmpty())
        return {};

    auto makeSortedMethods=[&controller, &metaObject]()
    {
        QHash<QString, QMetaMethod> hashItems;
        QVector<QMetaMethod> __return;
        static ByteArrayVector methodBlackList=QRPC_METHOD_BACK_LIST;
        for (auto i = 0; i < metaObject->methodCount(); ++i) {
            auto method = metaObject->method(i);

            if (method.methodType() != method.Method)
                continue;

            if (method.parameterCount() > 0)
                continue;

            if (method.name().startsWith(QByteArrayLiteral("_"))) //ignore methods with [_] in start name
                continue;

            if (methodBlackList.contains(method.name()))
                continue;

            const auto &ann = QAnnotation::Collection{controller->annotation(method)};
            const auto displaName=ann.find(opName()).toValueString();
            if(displaName.isEmpty())
                continue;
            hashItems.insert(displaName, method);
        }
        auto keys=hashItems.keys();
        keys.sort();
        for(auto &key:keys)
            __return.append(hashItems.value(key));
        return __return;
    };


    auto sortedMethods=makeSortedMethods();

    static QStm::Network network;

    for(auto basePath:vBasePathList){
        basePath=QStringLiteral("/")+basePath+QStringLiteral("/");
        while(basePath.contains(QStringLiteral("//")))
            basePath=basePath.replace(QStringLiteral("//"),QStringLiteral("/"));
        for(auto &method:sortedMethods){
            MethodInfo info;

            info.method = method;
            info.name = method.name().toLower();

            info.annotations=QAnnotation::Collection{controller->annotation(info.method)};
            info.excluded=info.annotations.contains(annotionExcludeMethod);
            if(info.excluded)
                continue;


            if(info.annotations.contains(opCrud)){
                info.methods.append(network.METHOD_GET);
                info.methods.append(network.METHOD_POST);
                info.methods.append(network.METHOD_PUT);
                info.methods.append(network.METHOD_DELETE);
                info.methods.append(network.METHOD_HEAD);
                info.methods.append(network.METHOD_OPTIONS);
            }
            else{
                if(info.annotations.contains(opGet))
                    info.methods.append(network.METHOD_GET);

                if(info.annotations.contains(opPost))
                    info.methods.append(network.METHOD_POST);

                if(info.annotations.contains(opPut))
                    info.methods.append(network.METHOD_PUT);

                if(info.annotations.contains(opDelete))
                    info.methods.append(network.METHOD_DELETE);

                if(info.annotations.contains(opHead))
                    info.methods.append(network.METHOD_HEAD);

                if(info.annotations.contains(opOptions))
                    info.methods.append(network.METHOD_OPTIONS);
            }

            if(info.annotations.contains(opTrace))
                info.methods.append(network.METHOD_TRACE);

            if(info.annotations.contains(opPatch))
                info.methods.append(network.METHOD_PATCH);

            info.order=info.annotations.find(opOrder()).toValueInt(-1);
            info.rules=info.annotations.find(opRules()).toVariant().toStringList();
            info.basePath=basePath.toUtf8();
            info.path=info.annotations.find(opPath()).toValueByteArray();
            if(info.path.isEmpty())
                info.path=info.name;

            info.name=info.annotations.find(opName()).toValueByteArray();
            if(info.path.isEmpty())
                info.path=info.path;

            info.description=info.annotations.find(opDescription()).toValueByteArray();
            if(info.description.isEmpty())
                info.description=info.name;

            info.group=info.annotations.find(opGroup()).toValueByteArray();
            if(info.group.isEmpty())
                continue;

            auto fullPath=QStringLiteral("%1/%2").arg(info.basePath, info.path);
            while(fullPath.contains(QStringLiteral("//")))
                fullPath=fullPath.replace(QStringLiteral("//"),QStringLiteral("/"));
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

    auto annotations=this->annotation();
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

QString Controller::module() const
{
    auto annotations=this->annotation();
    const auto &ann = annotations.find(apiModule());
    if(ann.isValid())
        return ann.value().toString();
    return {};
}

QUuid Controller::moduleUuid() const
{
    return {};
}

QString Controller::description() const
{
    const auto &annotations = this->annotation();
    const auto &ann = annotations.find(apiDescription());

    if(!ann.isValid())
        return ann.value().toString();

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
        vHash.remove(QStringLiteral("host"));
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
    auto origin = vHearder.value(QStringLiteral("Origin")).toString();
    auto referer = vHearder.value(QStringLiteral("Referer")).toString();
    auto connectionType = vHearder.value(QStringLiteral("Connection-Type")).toString();
    auto connection = vHearder.value(QStringLiteral("Connection")).toString();
    auto accessControlRequestHeaders = vHearder.value(QStringLiteral("Access-Control-Request-Headers")).toString();
    auto accessControlAllowMethods = vHearder.value(QStringLiteral("Access-Control-Request-Method")).toString();
    auto accessControlAllowOrigin = origin.isEmpty() ? referer : origin;
    auto keepAlive = vHearder.value(QStringLiteral("Keep-Alive")).toString();


    if (rq.isMethodOptions()) {
        //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        //https://developer.mozilla.org/pt-BR/docs/Web/HTTP/Methods/OPTIONS
        header_application_json = (!header_application_json.isEmpty()) ? header_application_json: QStringLiteral("text/javascripton");
        insertHeaderResponse(QStringLiteral("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
        insertHeaderResponse(QStringLiteral("Server"), this->server()->serverName());
        //TODO REVER CORS
        //  https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        //  Note: When responding to a credentialed requests request, the server must specify an origin in the value of the Access-Control-Allow-Origin header, instead of specifying the "*" wildcard.
        insertHeaderResponse(QStringLiteral("Access-Control-Allow-Origin"), accessControlAllowOrigin);

        //TODO DEVE RETORNAR AS POSSIBILIDADES DE METHODS DO METHODO REQUISITADO
        //  https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        insertHeaderResponse(QStringLiteral("Access-Control-Allow-Methods"), accessControlAllowMethods);

        //TODO REVER CORS
        //  https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        //O cabeçalho Access-Control-Allow-Headers é usado em resposta a uma solicitação de comprovação para indicar quais cabeçalhos HTTP podem ser usados ao fazer a solicitação real. Este cabeçalho é a resposta do lado do servidor ao cabeçalho Access-Control-Request-Headers do navegador.
        insertHeaderResponse(QStringLiteral("Access-Control-Allow-Headers"), accessControlRequestHeaders);

        //TODO REVER CORS
        //  https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        //  O cabeçalho Access-Control-Max-Age indica por quanto tempo os resultados de uma solicitação de comprovação podem ser armazenados em cache. Para obter um exemplo de solicitação de comprovação, consulte os exemplos acima.
        insertHeaderResponse(QStringLiteral("Access-Control-Max-Age"), QStringLiteral("86400"));

        //TODO REVER CORDS
        //  https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        //Se o servidor especificar uma única origem (que pode mudar dinamicamente com base na origem solicitante como parte de uma lista de permissões) em vez do curinga "*", o servidor também deverá incluir Origem no cabeçalho de resposta Vary para indicar aos clientes que as respostas do servidor será diferente com base no valor do cabeçalho da solicitação Origem.
        insertHeaderResponse(QStringLiteral("Vary"), QStringLiteral("Accept-Encoding, Origin"));
        insertHeaderResponse(QStringLiteral("Keep-Alive"), keepAlive);
        insertHeaderResponse(QStringLiteral("Connection"), connection);

        rq.setResponseHeader(vHearderResponse);
        rq.co().setOK();
        return rq.co().isOK();
    }

    //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
    header_application_json = (!header_application_json.isEmpty()) ? header_application_json: QStringLiteral("text/javascripton");
    insertHeaderResponse(QStringLiteral("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
    insertHeaderResponse(QStringLiteral("Server"), this->server()->serverName());
    //TODO REVER CORS
    //  https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
    //  Note: When responding to a credentialed requests request, the server must specify an origin in the value of the Access-Control-Allow-Origin header, instead of specifying the "*" wildcard.
    insertHeaderResponse(QStringLiteral("Access-Control-Allow-Origin"), accessControlAllowOrigin);

    //TODO REVER CORDS
    //  https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
    //Se o servidor especificar uma única origem (que pode mudar dinamicamente com base na origem solicitante como parte de uma lista de permissões) em vez do curinga "*", o servidor também deverá incluir Origem no cabeçalho de resposta Vary para indicar aos clientes que as respostas do servidor será diferente com base no valor do cabeçalho da solicitação Origem.
    insertHeaderResponse(QStringLiteral("Vary"), QStringLiteral("Accept-Encoding, Origin"));
    insertHeaderResponse(QStringLiteral("Keep-Alive"), keepAlive);
    insertHeaderResponse(QStringLiteral("Connection"), connection);
    insertHeaderResponse(QStringLiteral("Connection-Type"), connectionType);

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
    const auto &annotations = this->annotation(method);

    if(annotations.isEmpty())
        return true;

    if (annotations.contains(rqRedirect))
        return true;

    if (annotations.contains(rqExcludePath))
        return false;

    if (!annotations.containsClassification(ApiOperation))
        return true;

    if (annotations.contains(opCrud))
        return true;

    auto operation = QStringLiteral("op%1").arg(QString::fromUtf8(rq.requestMethod())).toLower();
    if (annotations.contains(operation))
        return true;

    return {};
}

bool Controller::canAuthorization()
{
    if (this->rq().isMethodOptions())
        return true;

    const auto &annotations = this->annotation();

    if (!annotations.containsClassification(Security))
        return true;

    if (annotations.contains(this->rqSecurityIgnore))
        return true;

    return false;
}

bool Controller::canAuthorization(const QMetaMethod &method)
{
    if (this->rq().isMethodOptions())
        return true;

    const auto &annotations = this->annotation(method);

    if (!annotations.containsClassification(Security))
        return true;

    if (annotations.contains(this->rqSecurityIgnore))
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
    const auto &annotations = this->annotation(method);
    if (!annotations.containsClassification(Security))
        return true;

    if (annotations.contains(this->rqSecurityIgnore))
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
            rInfo() << QStringLiteral("interface registered: ") << metaObject.className();
        qInfo() << QByteArrayLiteral("interface: ") + metaObject.className();
#endif
        (*staticInstalled).append(&metaObject);
    }
    return metaObject;
}

const QMetaObject &Controller::installParser(const QMetaObject &metaObject)
{
    if (!staticParserInstalled->contains(&metaObject)) {
#if Q_RPC_LOG_VERBOSE
        if (staticParserInstalled->isEmpty())
            rInfo() << QStringLiteral("parser interface registered: ") << metaObject.className();
        rInfo() << QByteArrayLiteral("parser interface") + metaObject.className();
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
