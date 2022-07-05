#include "./qrpc_request.h"
#include "./private/p_qrpc_request.h"
#include "./private/p_qrpc_listen_request_code.h"

namespace QRpc {

Request::Request(QObject *parent):QObject{parent}
{
    this->p = new RequestPvt{this};
}

bool Request::startsWith(const QString &requestPath, const QVariant &requestPathBase)
{
    QStringList paths;

    switch (qTypeId(requestPathBase)){
    case QMetaType_QStringList:
    case QMetaType_QVariantList:
    {
        for(auto &v:requestPathBase.toList())
            paths.append(v.toString().trimmed());
        break;
    }
    default:
        paths.append(requestPathBase.toString().trimmed());
    }

    for(auto &pathItem:paths){
        auto startWith=pathItem.contains(qsl("*"));
        if(startWith){
            pathItem=pathItem.split(qsl("*")).first();
        }

        auto route=qsl("/%1/").arg(pathItem.trimmed().toLower());
        auto path=qsl("/%1/").arg(requestPath.trimmed().toLower());

        while(route.contains(qsl("//")))
            route=route.replace(qsl("//"),qsl("/"));

        while(path.contains(qsl("//")))
            path=path.replace(qsl("//"),qsl("/"));

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

Request &Request::setSettings(const ServiceSetting &setting)
{

    p->setSettings(setting);
    return*this;
}

Request &Request::setSettings(const QVariantHash &setting)
{
    ServiceSetting _setting;
    _setting.fromHash(setting);
    auto &request=*this;
    request.header().addRawHeader(_setting.headers());//precisa ser add
    request.setProtocol(_setting.protocol());
    request.setPort(_setting.port());
    request.setHostName(_setting.hostName());
    request.setMethod(_setting.method());
    request.setRoute(_setting.route());

    auto method=request.exchange().call().method();

    switch(method){
    case QRpc::Post:
    case QRpc::Put:
        request.setBody(_setting.body());
        break;
    default:
        if(!_setting.parameters().isEmpty())
            request.setBody(_setting.parameters());
        else
            request.setBody(_setting.body());
    };


    return*this;
}

QString Request::url() const
{
    return this->url(qsl_null);
}

QString Request::url(const QString &path) const
{
    auto &rq=*this;

    auto spath=path.trimmed().isEmpty()?this->route().trimmed():path.trimmed();
    spath=qsl("/%1").arg(spath);
    while(spath.contains(qsl("//")))
        spath=spath.replace(qsl("//"),"/");

    if(path.isEmpty())
        return qsl("%1://%2:%3").arg(rq.protocolName(),rq.hostName(),rq.port().toString());

    return qsl("%1://%2:%3%4").arg(rq.protocolName(),rq.hostName(),rq.port().toString(),spath);
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
    return*this;
}

RequestMethod Request::method() const
{

    return RequestMethod(p->exchange.call().method());
}

Request &Request::setMethod(const QString &value)
{

    p->exchange.setMethod(value);
    return*this;
}

Request &Request::setMethod(const QByteArray &value)
{

    p->exchange.setMethod(value);
    return*this;
}

Request &Request::setMethod(const int &value)
{

    p->exchange.setMethod(RequestMethod(value));
    return*this;
}

QString Request::driver() const
{

    return p->exchange.call().driver();
}

Request &Request::setDriver(const QString &value)
{

    p->exchange.call().setDriver(value);
    return*this;
}

QString Request::hostName() const
{

    return p->exchange.call().hostName();
}

Request &Request::setHostName(const QString &value)
{

    p->exchange.setHostName(value);
    return*this;
}

QString Request::userName() const
{

    return p->exchange.call().userName();
}

Request &Request::setUserName(const QString &value)
{

    p->exchange.call().setUserName(value);
    return*this;
}

QString Request::password() const
{

    return p->exchange.call().passWord();
}

Request &Request::setPassword(const QString &value)
{

    p->exchange.call().setPassWord(value);
    return*this;
}

QString &Request::route() const
{

    return p->exchange.call().route();
}

Request &Request::setRoute(const QVariant &value)
{

    p->exchange.call().setRoute(value);
    return*this;
}

QVariant Request::body() const
{

    return p->qrpcBody.body();
}

Request &Request::setBody(const QVariant &value)
{

    p->qrpcBody.setBody(value);
    return*this;
}

QVariant Request::port() const
{

    return p->exchange.call().port();
}

Request &Request::setPort(const QVariant &value)
{
    QVariant v;
    auto type=qTypeId(value);
    switch (type) {
    case QMetaType_QVariantList:
    case QMetaType_QStringList:
    {
        auto l=value.toList();
        v=l.isEmpty()?0:l.last().toInt();
        break;
    }
    case QMetaType_QVariantHash:
    case QMetaType_QVariantMap:
    {
        auto l=value.toHash().values();
        v=l.isEmpty()?0:l.last().toInt();
        break;
    }
    default:
        v=value.toInt();
    }
    p->exchange.setPort(v.toInt());
    return*this;
}

qlonglong Request::activityLimit() const
{

    return p->exchange.call().activityLimit();
}

Request &Request::setActivityLimit(const QVariant &value)
{

    p->exchange.call().setActivityLimit(value);
    return*this;
}

RequestExchange &Request::exchange()
{

    return p->exchange;
}

QRpc::HttpHeaders &Request::header()
{

    return p->qrpcHeader;
}

Request::Body &Request::body()
{

    return p->qrpcBody;
}

HttpResponse &Request::response()
{

    return p->qrpcResponse;
}

QHash<int, int> Request::requestRecovery()const
{

    return p->requestRecovery;
}

Request &Request::setRequestRecovery(int statusCode)
{

    p->requestRecovery[statusCode]=1;
    return*this;
}

Request &Request::setRequestRecovery(int statusCode, int repeatCount)
{

    p->requestRecovery[statusCode]=repeatCount;
    return*this;
}

Request &Request::setRequestRecoveryOnBadGateway(int repeatCount)
{

    p->requestRecovery[ListenRequestCode::ssBadGateway]=repeatCount;
    return*this;
}

LastError &Request::lastError()
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

Request &Request::operator=(const QRpc::ServiceSetting &value)
{
    this->setSettings(value);
    return*this;
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
        if(!i.key().toLower().startsWith(qsl("set-cookie")))
            continue;
        auto cookieList=i.value().toString().split(qsl(";"));
        for(auto &cookie:cookieList){
            if(!cookies.contains(cookie))
                cookies.append(cookie);
        }
    }
    this->header().setCookies(cookies);
    return*this;
}

QString Request::toString() const
{
    auto &response=p->qrpcResponse;
    auto qt_text=ListenRequestCode::qt_network_error_phrase(p->response_qt_status_code);
    auto msg=qsl("%1:QtStatus: Status:%2, %3, %4").arg(p->exchange.call().url(), QString::number(response.qtStatusCode()),response.reasonPhrase(), qt_text);
    return msg;
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
    return*this;
}

Request &Request::print()
{
    for(auto &v:this->printOut())
        sInfo()<<v;
    return*this;
}

QStringList Request::printOut()
{
    QStringList out;
    for(auto &v:this->exchange().printOut(qsl("exchange")))
        out.append(v);
    for(auto &v:this->header().printOut(qsl("request")))
        out.append(v);
    for(auto &v:this->response().printOut(qsl("response")))
        out.append(v);
    return out;
}

Request::Body::Body(QObject *parent):QObject{parent}
{
}

QVariant &Request::Body::body() const
{
    return p->request_body;
}

void Request::Body::setBody(const QVariant &value)
{

    p->request_body=value;
}

QString Request::Body::toString()const
{

    auto type=qTypeId(p->request_body);
    switch (type) {
    case QMetaType_QVariantList:
    case QMetaType_QVariantHash:
    case QMetaType_QVariantMap:
    case QMetaType_QStringList:
        return QJsonDocument::fromVariant(p->request_body).toJson();
    default:
        return p->request_body.toString();
    }
}

QVariantMap Request::Body::toMap() const
{
    return QVariant{this->toHash()}.toMap();
}

QVariantHash Request::Body::toHash()const
{

    auto type=qTypeId(p->request_body);
    switch (type) {
    case QMetaType_QVariantHash:
    case QMetaType_QVariantList:
    case QMetaType_QVariantMap:
    case QMetaType_QStringList:
        return QJsonDocument::fromVariant(p->request_body).object().toVariantHash();
    default:
        return QJsonDocument::fromJson(p->request_body.toByteArray()).object().toVariantHash();
    }
}

QVariantList Request::Body::toList() const
{

    auto type=qTypeId(p->request_body);
    switch (type) {
    case QMetaType_QVariantHash:
    case QMetaType_QVariantList:
    case QMetaType_QVariantMap:
    case QMetaType_QStringList:
        return QJsonDocument::fromVariant(p->request_body).array().toVariantList();
    default:
        return QJsonDocument::fromJson(p->request_body.toByteArray()).array().toVariantList();
    }
}

Request &Request::Body::rq()
{

    return*p->parent;
}

}
