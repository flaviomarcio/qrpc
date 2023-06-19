#include "./qrpc_request.h"
#include "./private/p_qrpc_request.h"
#include "./private/p_qrpc_listen_request_code.h"
#include "./qrpc_macro.h"

namespace QRpc {

Request::Request(QObject *parent):QObject{parent}
{
    this->p = new RequestPvt{this};
}

bool Request::startsWith(const QString &requestPath, const QVariant &requestPathBase)
{
    QStringList paths;

    switch (requestPathBase.typeId()){
    case QMetaType::QStringList:
    case QMetaType::QVariantList:
    {
        for(auto &v:requestPathBase.toList())
            paths.append(v.toString().trimmed());
        break;
    }
    default:
        paths.append(requestPathBase.toString().trimmed());
    }

    for(auto &pathItem:paths){
        auto startWith=pathItem.contains(QStringLiteral("*"));
        if(startWith){
            pathItem=pathItem.split(QStringLiteral("*")).first();
        }

        auto route=QStringLiteral("/%1/").arg(pathItem.trimmed().toLower());
        auto path=QStringLiteral("/%1/").arg(requestPath.trimmed().toLower());

        while(route.contains(QStringLiteral("//")))
            route=route.replace(QStringLiteral("//"),QStringLiteral("/"));

        while(path.contains(QStringLiteral("//")))
            path=path.replace(QStringLiteral("//"),QStringLiteral("/"));

        if(startWith && path.startsWith(route))
            return true;

        if(path==route)
            return true;

    }

    return false;
}

bool Request::startsWith(const QString &requestPath)
{
    return this->startsWith(this->route(), requestPath);
}

bool Request::isEqual(const QString &requestPath)
{
    return this->isEqual(requestPath, this->route());
}

bool Request::isEqual(const QString &requestPath, const QVariant &requestPathBase)
{
    if(!Request::startsWith(requestPath, requestPathBase))
        return false;

    return true;
}

bool Request::canRequest() const
{
    if(this->port().toInt()<0)
        return false;

    if(this->hostName().isEmpty())
        return false;

    return true;
}

Request &Request::setSettings(const QStm::SettingBase &setting)
{
    p->setSettings(setting);
    return *this;
}

Request &Request::setSettings(const QVariantHash &setting)
{
    QStm::SettingBase settings;
    settings.fromHash(setting);
    auto &request=*this;
    request.header().addRawHeader(settings.headers());//precisa ser add
    request.setProtocol(settings.protocol());
    request.setPort(settings.port());
    request.setHostName(settings.hostName());
    request.setMethod(settings.method());
    request.setRoute(settings.route());

    auto method=request.exchange().call().method();

    switch(method){
    case QRpc::Post:
    case QRpc::Put:
        request.setBody(settings.body());
        break;
    default:
        if(!settings.parameters().isEmpty())
            request.setBody(settings.parameters());
        else
            request.setBody(settings.body());
    };

    return *this;
}

QString Request::url() const
{
    return this->url({});
}

QString Request::url(const QString &path) const
{
    auto &rq=*this;

    auto spath=path.trimmed().isEmpty()?this->route().trimmed():path.trimmed();
    spath=QStringLiteral("/%1").arg(spath);
    while(spath.contains(QStringLiteral("//")))
        spath=spath.replace(QStringLiteral("//"),"/");

    if(path.isEmpty())
        return QStringLiteral("%1://%2:%3").arg(rq.protocolName(),rq.hostName(),rq.port().toString());

    return QStringLiteral("%1://%2:%3%4").arg(rq.protocolName(),rq.hostName(),rq.port().toString(),spath);
}

Protocol Request::protocol() const
{
    return p->exchange.call().protocol();
}

QString Request::protocolName() const
{
    return p->exchange.call().protocolName();
}

Request &Request::setProtocol(const QVariant &value)
{
    p->exchange.setProtocol(value);
    return *this;
}

RequestMethod Request::method() const
{
    return RequestMethod(p->exchange.call().method());
}

Request &Request::setMethod(const QString &value)
{
    p->exchange.setMethod(value);
    return *this;
}

Request &Request::setMethod(const QByteArray &value)
{
    p->exchange.setMethod(value);
    return *this;
}

Request &Request::setMethod(const int &value)
{
    p->exchange.setMethod(RequestMethod(value));
    return *this;
}

QString Request::driver() const
{
    return p->exchange.call().driver();
}

Request &Request::setDriver(const QString &value)
{
    p->exchange.call().setDriver(value);
    return *this;
}

QString Request::hostName() const
{
    return p->exchange.call().hostName();
}

Request &Request::setHostName(const QString &value)
{
    p->exchange.setHostName(value);
    return *this;
}

QString Request::userName() const
{
    return p->exchange.call().userName();
}

Request &Request::setUserName(const QString &value)
{
    p->exchange.call().setUserName(value);
    return *this;
}

QString Request::password() const
{
    return p->exchange.call().passWord();
}

Request &Request::setPassword(const QString &value)
{
    p->exchange.call().setPassWord(value);
    return *this;
}

QString &Request::route() const
{
    return p->exchange.call().route();
}

Request &Request::setRoute(const QVariant &value)
{
    p->exchange.call().setRoute(value);
    return *this;
}

Request &Request::setBody(const QVariant &value)
{
    p->qrpcBody.setBody(value);
    return *this;
}

QVariant Request::port() const
{
    return p->exchange.call().port();
}

Request &Request::setPort(const QVariant &value)
{
    QVariant v;
    auto type=value.typeId();
    switch (type) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    {
        auto l=value.toList();
        v=l.isEmpty()?0:l.last().toInt();
        break;
    }
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    {
        auto l=value.toHash().values();
        v=l.isEmpty()?0:l.last().toInt();
        break;
    }
    default:
        v=value.toInt();
    }
    p->exchange.setPort(v.toInt());
    return *this;
}

qlonglong Request::activityLimit() const
{
    return p->exchange.call().activityLimit();
}

Request &Request::setActivityLimit(const QVariant &value)
{
    p->exchange.call().setActivityLimit(value);
    return *this;
}

RequestExchange &Request::exchange() const
{
    return p->exchange;
}

QRpc::HttpHeaders &Request::header() const
{
    return p->qrpcHeader;
}

Request::Body &Request::body()const
{
    return p->qrpcBody;
}

HttpResponse &Request::response() const
{
    return p->qrpcResponse;
}

const QHash<int, int> &Request::requestRecovery()const
{
    return p->requestRecovery;
}

Request &Request::setRequestRecovery(int statusCode)
{
    p->requestRecovery.insert(statusCode,1);
    return *this;
}

Request &Request::setRequestRecovery(int statusCode, int repeatCount)
{
    p->requestRecovery.insert(statusCode, repeatCount);
    return *this;
}

Request &Request::setRequestRecoveryOnBadGateway(int repeatCount)
{
    p->requestRecovery[ListenRequestCode::ssBadGateway]=repeatCount;
    return *this;
}

const LastError &Request::lastError() const
{
    return p->qrpcLastError;
}

HttpResponse &Request::call()
{
    auto &e=p->exchange.call();
    return p->call(e.method(), e.route(), {});
}

HttpResponse &Request::call(const QVariant &route)
{
    auto &e=p->exchange.call();
    e.setRoute(route);
    return p->call(e.method(), e.route(), this->body().body());
}

HttpResponse &Request::call(const QVariant &route, const QVariant &body)
{
    auto &e=p->exchange.call();
    e.setRoute(route);
    return p->call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const QVariant &route, const QVariant &body, const QString &method, const QVariantHash parameter)
{
    auto &e=p->exchange.call();
    e.setMethod(method);
    e.setRoute(route);
    e.setParameter(parameter);
    return p->call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const RequestMethod &method, const QString &route, const QVariant &body)
{
    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p->call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const RequestMethod &method)
{
    auto &e=p->exchange.call();
    e.setMethod(method);
    auto &body=this->body().body();
    return p->call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const QVariant &route, const QObject &objectBody)
{
    QVariantHash mapObject;
    for(int i = 0; i < objectBody.metaObject()->propertyCount(); ++i) {
        auto property=objectBody.metaObject()->property(i);
        mapObject.insert(property.name(), property.read( &objectBody));
    }

    auto &e=p->exchange.call();
    e.setMethod(QRpc::Post);
    e.setRoute(route);
    return p->call(e.method(), route, QJsonDocument::fromVariant(mapObject).toJson());
}

HttpResponse &Request::call(const RequestMethod &method, const QString &route, const QObject &objectBody)
{
    QVariantHash mapObject;
    for(int i = 0; i < objectBody.metaObject()->propertyCount(); ++i) {
        auto property=objectBody.metaObject()->property(i);
        mapObject.insert(property.name(), property.read( &objectBody));
    }

    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p->call(e.method(), e.route(), mapObject);
}

HttpResponse &Request::call(const QVariant &route, QIODevice &ioDeviceBody)
{

    auto body=ioDeviceBody.readAll();
    auto &e=p->exchange.call();
    e.setMethod(QRpc::Post);
    e.setRoute(route);
    return p->call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const RequestMethod &method, const QString &route)
{

    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p->call(e.method(), e.route(), this->body().body());
}

HttpResponse &Request::call(const RequestMethod &method, const QString &route, QIODevice &ioDeviceBody)
{

    auto body=ioDeviceBody.readAll();
    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p->call(e.method(), e.route(), body);
}

Request &Request::operator=(const QStm::SettingBase &value)
{
    this->setSettings(value);
    return *this;
}

HttpResponse &Request::upload(QFile &file)
{

    auto &e=p->exchange.call();
    e.setMethod(QRpc::Post);
    p->upload(e.route(), file.fileName());
    file.close();
    return this->response();
}

HttpResponse &Request::upload(const QVariant &route, const QByteArray &buffer)
{

    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Post);
    QTemporaryFile file;

    if(!file.open())
        return this->response();
    file.write(buffer);
    file.flush();
    p->upload(e.route(), file.fileName());
    file.close();
    return this->response();
}


HttpResponse &Request::upload(const QVariant &route, QFile &file)
{

    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Post);
    p->upload(e.route(), file.fileName());
    file.close();
    return this->response();
}

HttpResponse &Request::upload(const QVariant &route, QString &fileName, QFile &file)
{

    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Post);
    p->upload(e.route(), fileName);
    file.close();
    return this->response();
}


HttpResponse &Request::download(QString &fileName)
{

    auto _fileName=p->parseFileName(fileName);
    auto &e=p->exchange.call();
    e.setMethod(QRpc::Get);
    auto &response=p->download(e.route(), _fileName);
    if(response)
        fileName=_fileName;
    return this->response();
}

HttpResponse &Request::download(const QVariant &route, QString &fileName)
{

    auto _fileName=p->parseFileName(fileName);
    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Get);
    auto &response=p->download(e.route(), _fileName);
    if(response)
        fileName=_fileName;
    return response;
}

HttpResponse &Request::download(const QVariant &route, QString &fileName, const QVariant &parameter)
{

    auto _fileName=p->parseFileName(fileName);
    auto &e=p->exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Get);
    this->setBody(parameter);
    auto &response=p->download(e.route(), _fileName);
    if(response)
        fileName=_fileName;
    return response;
}

Request &Request::autoSetCookie()
{
    auto vRawHeader=this->response().header().rawHeader();
    QHashIterator<QString, QVariant> i(vRawHeader);
    auto cookies=this->header().cookies().toStringList();
    while (i.hasNext()){
        i.next();
        if(!i.key().toLower().startsWith(QStringLiteral("set-cookie")))
            continue;
        auto cookieList=i.value().toString().split(QStringLiteral(";"));
        for(auto &cookie:cookieList){
            if(!cookies.contains(cookie))
                cookies.append(cookie);
        }
    }
    this->header().setCookies(cookies);
    return *this;
}

QString Request::toString() const
{
    auto &response=p->qrpcResponse;
    auto qt_text=ListenRequestCode::qt_network_error_phrase(p->response_qt_status_code);
    static auto format=QStringLiteral("%1:QtStatus: Status:%2, %3, %4");
    return format.arg(p->exchange.call().url(), QString::number(response.qtStatusCode()),response.reasonPhrase(), qt_text);
}

QVariantHash Request::toResponse()const
{
    return p->qrpcResponse.toResponse();
}

QSslConfiguration &Request::sslConfiguration()
{
    return p->sslConfiguration;
}

Request &Request::setSslConfiguration(const QSslConfiguration &value)
{
    p->sslConfiguration = value;
    return *this;
}

Request &Request::print()
{
    for(auto &v:this->printOut())
        rInfo()<<v;
    return *this;
}

QStringList Request::printOut()
{
    QStringList out;
    for(auto &v:this->exchange().printOut(QStringLiteral("exchange")))
        out.append(v);
    for(auto &v:this->header().printOut(QStringLiteral("request")))
        out.append(v);
    for(auto &v:this->response().printOut(QStringLiteral("response")))
        out.append(v);
    return out;
}

Request::Body::Body(QObject *parent):QObject{parent}
{
}

const QVariant &Request::Body::body() const
{
    return p->request_body;
}

void Request::Body::setBody(const QVariant &value)
{
    p->request_body=value;
}

QString Request::Body::toString()const
{
    auto type=p->request_body.typeId();
    switch (type) {
    case QMetaType::QVariantList:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    case QMetaType::QStringList:
        return QJsonDocument::fromVariant(p->request_body).toJson();
    default:
        return p->request_body.toString();
    }
}

QVariantHash Request::Body::toHash()const
{
    auto type=p->request_body.typeId();
    switch (type) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantList:
    case QMetaType::QVariantMap:
    case QMetaType::QStringList:
        return QJsonDocument::fromVariant(p->request_body).object().toVariantHash();
    default:
        return QJsonDocument::fromJson(p->request_body.toByteArray()).object().toVariantHash();
    }
}

QVariantList Request::Body::toList() const
{
    auto type=p->request_body.typeId();
    switch (type) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantList:
    case QMetaType::QVariantMap:
    case QMetaType::QStringList:
        return QJsonDocument::fromVariant(p->request_body).array().toVariantList();
    default:
        return QJsonDocument::fromJson(p->request_body.toByteArray()).array().toVariantList();
    }
}

Request &Request::Body::rq()
{
    return *p->parent;
}

}
