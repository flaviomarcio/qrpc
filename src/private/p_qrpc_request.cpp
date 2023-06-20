#include "./p_qrpc_request.h"
#include "../qrpc_const.h"
#include "../qrpc_log.h"
#include "./p_qrpc_request_job_response.h"
#if Q_RPC_LOG
#include "../qrpc_macro.h"
#endif
#include "./p_qrpc_util.h"
#include "./p_qrpc_request_job.h"
#include "../../../qstm/src/qstm_util_variant.h"

namespace QRpc {

static const auto __json="json";
static const auto __request="request";

RequestPvt::RequestPvt(Request *parent):
    QObject{parent},
    parent{parent},
    exchange{parent},
    qrpcHeader{parent},
    qrpcBody{parent},
    qrpcResponse{parent},
    qrpcLastError{parent},
    sslConfiguration(QSslConfiguration::defaultConfiguration()),
    fileLog{logFile(__json, __request)}
{
    this->qrpcBody.p=this;
}

QString RequestPvt::urlMaker(const QString &path)
{
    auto &rq=*this->parent;
    auto spath=path.trimmed().isEmpty()?rq.route().trimmed():path.trimmed();
    spath=QStringLiteral("/%1").arg(spath);
    while(spath.contains(QStringLiteral("//")))
        spath=spath.replace(QStringLiteral("//"),"/");

    auto shostname=rq.hostName().trimmed();
    auto sprotocol=rq.protocolName().trimmed();
    auto sport=rq.port().toString();
    static const auto __format1=QStringLiteral("%1://%2:%3");
    static const auto __format2=QStringLiteral("%1://%2:%3%4");
    return path.isEmpty()
               ?__format1.arg(sprotocol, shostname, sport)
               :__format2.arg(sprotocol, shostname, sport, spath);
}

void RequestPvt::setSettings(const QStm::SettingBase &setting)
{
    auto __header=setting.headers();
    parent->header().setRawHeader(__header);
    parent->setProtocol(setting.protocol());
    parent->setPort(setting.port());
    parent->setDriver(setting.driverName());
    parent->setHostName(setting.hostName());
    parent->setMethod(setting.method());
    parent->setRoute(setting.route());
}

QString RequestPvt::parseFileName(const QString &fileName)
{
    auto _fileName=fileName.trimmed();
    if(_fileName.isEmpty()){
        QTemporaryFile file;
        file.open();
        _fileName=file.fileName();
        file.close();
    }
    return _fileName;
}

void RequestPvt::writeLog(RequestJobResponse *response, const QVariant &request)
{
    if(!logRegister())
        return;

    QFile file(this->fileLog);
    if (!file.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream outText(&file);
    auto &e=response->request_exchange.call();
    outText << RequestMethodName.value(e.method())<<QStringLiteral(": ")<<response->request_url.toString()<<'\n';
    outText << QJsonDocument::fromVariant(request).toJson(QJsonDocument::Indented);
    outText << QStringLiteral("\n");
    outText << QStringLiteral("\n");
    file.flush();
    file.close();
}

HttpResponse &RequestPvt::upload(const QString &route, const QString &fileName)
{
    this->qrpcLastError.clear();

    {//configuracao do contentType pela extencao
        auto content=this->qrpcHeader.contentType();
        if(!content.isValid() || content.isNull()){
            auto ext=QStringLiteral(".")+fileName.split(QStringLiteral(".")).last().toLower();
            if(!ContentTypeExtensionToHeader.contains(ext)){
                this->qrpcHeader.setContentType(QRpc::AppOctetStream);
            }
            else{
                auto contentType=ContentTypeExtensionToHeader.value(ext);
                this->qrpcHeader.setContentType(contentType);
            }
        }
    }
    {//configuracao do upload
        auto content=this->qrpcHeader.contentDisposition();
        if(!content.isValid() || content.isNull()){
            auto file=fileName.split(QStringLiteral("/")).last();
            auto value=QStringLiteral("form-data; name=\"%1\" filename=\"%1\"").arg(file);
            this->qrpcHeader.addRawHeader(ContentDispositionName, value);
        }
    }
    auto baseRoute = QString::fromUtf8(this->parent->baseRoute()).trimmed().replace(QStringLiteral("\""), "");
    auto routeCall=route.trimmed();
    if(!routeCall.startsWith(baseRoute))
        routeCall=QStringLiteral("/%1/%2").arg(baseRoute, route);

    auto &e=this->exchange.call();
    e.setMethod(QRpc::Post);
    e.setRoute(routeCall);

    switch (e.protocol()) {
    case QRpc::Http:
    {
        auto e_port=e.port().toInt()==80?"":QStringLiteral(":%1").arg(e.port().toInt());
        auto request_url = QStringLiteral("%1%2/%3").arg(e.hostName(), e_port, e.route()).replace(QStringLiteral("\""), "").replace(QStringLiteral("//"), QStringLiteral("/"));
        while(request_url.contains(QStringLiteral("//")))
            request_url=request_url.replace(QStringLiteral("//"), QStringLiteral("/"));
        request_url = QStringLiteral("%1://%2").arg(e.protocolUrlName(), request_url.simplified());
        this->request_url=QUrl{request_url}.toString();
        break;
    }
    default:
        auto request_url = QStringLiteral("%1:%2").arg(e.hostName(),e.port().toInt()).replace(QStringLiteral("\""), "").replace(QStringLiteral("//"), QStringLiteral("/"));
        request_url = QStringLiteral("%1://%2").arg(e.protocolUrlName(), request_url.simplified());
        while(request_url.contains(QStringLiteral("//")))
            request_url=request_url.replace(QStringLiteral("//"), QStringLiteral("/"));
        this->request_url=QUrl{request_url};
    }

    auto job = RequestJob::newJob(Request::acUpload);
    QObject::connect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    job->start();
    emit runJob(&this->sslConfiguration, this->qrpcHeader.rawHeader(), this->request_url, fileName, this->parent);
    job->wait();
    QObject::disconnect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    this->qrpcResponse.setResponse(&job->response());
    this->writeLog(&job->response(), job->response().toVariant());
    job->release();
    return this->qrpcResponse;
}

HttpResponse &RequestPvt::download(const QString &route, const QString &fileName)
{

    if(!this->qrpcHeader.contentType().isValid())
        this->qrpcHeader.setContentType(QRpc::AppOctetStream);
    this->qrpcHeader.addRawHeader(ContentDispositionName, QStringLiteral("form-data; name=%1; filename=%1").arg(fileName));

    this->qrpcLastError.clear();

    auto baseRoute = QString::fromUtf8(this->parent->baseRoute()).trimmed().replace(QStringLiteral("\""), "");

    auto routeCall=route.trimmed();
    if(!routeCall.startsWith(baseRoute))
        routeCall=QStringLiteral("/%1/%2").arg(baseRoute, route);

    auto &e=this->exchange.call();
    e.setMethod(QRpc::Get);
    e.setRoute(routeCall);
    auto &vBody=this->request_body;
    auto method=e.method();
    QMultiHash<QString,QVariant> paramsGet;
    switch (method) {
    case QRpc::Head:
    case QRpc::Get:
    case QRpc::Delete:
    case QRpc::Options:
    {
        Q_DECLARE_VU;
        paramsGet=vu.toMultiHash(vBody);
        vBody.clear();
        auto paramsGetOriginais = e.parameter();
        if (!paramsGetOriginais.isEmpty())
            paramsGet.unite(paramsGetOriginais);
        break;
    }
    default:
        break;
    }

    switch (e.protocol()) {
    case QRpc::Http:
    {
        auto e_port=e.port().toInt()==80?"":QStringLiteral(":%1").arg(e.port().toInt());
        auto request_url_str = QStringLiteral("%1%2/%3").arg(e.hostName(), e_port, e.route()).replace(QStringLiteral("\""), "").replace(QStringLiteral("//"), QStringLiteral("/"));
        auto request_url_part = request_url_str.split(QStringLiteral("/"));
        request_url_str.clear();
        for(auto &line:request_url_part){
            if(line.trimmed().isEmpty())
                continue;
            if(!request_url_str.isEmpty())
                request_url_str+=QStringLiteral("/");
            request_url_str+=line;
        }
        request_url_str=QStringLiteral("%1://%2").arg(e.protocolUrlName(), request_url_str);
        this->request_url=QUrl(request_url_str);
        if(!paramsGet.isEmpty()){
            QUrlQuery url_query;
            QMultiHashIterator<QString, QVariant> i(paramsGet);
            while (i.hasNext()){
                i.next();
                const auto &k=i.key();
                const auto v=Util::parseQueryItem(i.value());
                url_query.addQueryItem(k, v);
            }
            QUrl url{request_url};
            url.setQuery(url_query);
            this->request_url=url;
        }
        break;
    }
    default:
        auto request_url = QStringLiteral("%1:%2").arg(e.hostName()).arg(e.port().toInt()).replace(QStringLiteral("\""), "").replace(QStringLiteral("//"), QStringLiteral("/"));
        request_url = QStringLiteral("%1://%2").arg(e.protocolUrlName(), request_url);
        this->request_url=QUrl{request_url};
    }

    auto job = RequestJob::newJob(Request::acDownload, fileName);
    QObject::connect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    job->start();
    emit runJob(&this->sslConfiguration, this->qrpcHeader.rawHeader(), this->request_url, fileName, this->parent);
    job->wait();
    QObject::disconnect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    this->qrpcResponse.setResponse(&job->response());
    this->writeLog(&job->response(), job->response().toVariant());
    job->release();
    return this->qrpcResponse;
}

HttpResponse &RequestPvt::call(const RequestMethod &method, const QVariant &vRoute, const QVariant &body)
{
    this->qrpcLastError.clear();

    auto vBody=body;

    auto baseRoute = QString::fromUtf8(this->parent->baseRoute()).trimmed().replace(QStringLiteral("\""), "");
    auto route=vRoute.toString().trimmed();
    auto routeCall=route.trimmed();
    if(!routeCall.startsWith(baseRoute))
        routeCall=QStringLiteral("/%1/%2").arg(baseRoute,route);

    auto &e=this->exchange.call();
    e.setMethod(method);
    e.setRoute(routeCall);

    QMultiHash<QString,QVariant> paramsGet;
    switch (method) {
    case QRpc::Head:
    case QRpc::Get:
    case QRpc::Delete:
    case QRpc::Options:
    {
        Q_DECLARE_VU;
        paramsGet=vu.toMultiHash(vBody);
        vBody.clear();
        auto paramsGetOriginais = e.parameter();
        if (!paramsGetOriginais.isEmpty())
            paramsGet.unite(paramsGetOriginais);
        break;
    }
    default:
        break;
    }

    switch (e.protocol()) {
    case QRpc::Http:
    {
        auto e_port=e.port().toInt()==80?"":QStringLiteral(":%1").arg(e.port().toInt());
        auto request_url_str = QStringLiteral("%1%2/%3").arg(e.hostName(), e_port, e.route()).replace(QStringLiteral("\""), "").replace(QStringLiteral("//"), QStringLiteral("/"));
        auto request_url_part = request_url_str.split(QStringLiteral("/"));
        request_url_str.clear();
        for(auto &line:request_url_part){
            if(line.trimmed().isEmpty())
                continue;
            if(!request_url_str.isEmpty())
                request_url_str+=QStringLiteral("/");
            request_url_str+=line;
        }
        request_url_str=QStringLiteral("%1://%2").arg(e.protocolUrlName(), request_url_str);
        this->request_url=QUrl{request_url_str};
        if(!paramsGet.isEmpty()){
            QUrlQuery url_query;
            QMultiHashIterator<QString, QVariant> i(paramsGet);
            while (i.hasNext()) {
                i.next();
                url_query.addQueryItem(i.key(), i.value().toString());
            }
            QUrl url{request_url};
            url.setQuery(url_query);
            this->request_url=url;
        }
        break;
    }
    case QRpc::DataBase:
    {
        auto topic=e.topic().trimmed();
        if(topic.isEmpty()){
            auto driver=e.driver();
            auto port=e.port().toInt();
            topic=QStringLiteral("broker-%1-%2").arg(driver).arg(port).toLower();
        }
        this->request_url=topic;
        break;
    }
    default:
        auto request_url = QStringLiteral("%1:%2").arg(e.hostName()).arg(e.port().toInt()).replace(QStringLiteral("\""), "").replace(QStringLiteral("//"),QStringLiteral("/"));
        request_url = QStringLiteral("%1://%2").arg(e.protocolUrlName(), request_url);
        this->request_url=QUrl{request_url};
    }

    switch (e.protocol()) {
    case QRpc::Http:
    {
        switch (vBody.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        case QMetaType::QVariantMap:
            this->request_body = QJsonDocument::fromVariant(vBody).toJson(QJsonDocument::Compact);
            break;
        default:
            this->request_body = vBody.toByteArray();
        }
        break;
    }
    case QRpc::DataBase:
    case QRpc::Kafka:
    case QRpc::Amqp:
    case QRpc::WebSocket:
    case QRpc::TcpSocket:
    case QRpc::UdpSocket:
    {
        switch (vBody.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        case QMetaType::QVariantMap:
            this->request_body = QJsonDocument::fromVariant(vBody).toJson(QJsonDocument::Compact);
            break;
        default:
            this->request_body = vBody.toByteArray();
        }

        auto base=QUuid::createUuid().toString() + QDateTime::currentDateTime().toString();
        ListenRequest request;
        request.setRequestProtocol(e.protocol());
        request.setRequestUuid( QUuid::createUuidV3(QUuid::createUuid(), base.toUtf8()) );
        request.setRequestMethod(e.methodName().toUtf8().toLower());
        request.setRequestHeader(this->qrpcHeader.rawHeader());
        request.setRequestBody(vBody);
        request.setRequestPath(routeCall.toUtf8());
        this->request_body = request.toHash();
        break;
    }
    default:
        this->request_body = vBody;
    }
    const auto &requestRecovery=this->requestRecovery;
    auto job = RequestJob::newJob(Request::acRequest);
    QObject::connect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    int executeCount=0;
    forever{
        job=RequestJob::runJob(job);
        ++executeCount;
        emit runJob(&this->sslConfiguration, this->qrpcHeader.rawHeader(), this->request_url, {}, this->parent);
        job->wait();

        if(job->response().response_qt_status_code==QNetworkReply::NoError)//if succes then break
            break;

        auto response_status_code=job->response().response_status_code;
        if(!requestRecovery.contains(response_status_code)){//if not check status code then check reason_phrase
            auto response_status_reason_phrase=job->response().response_status_reason_phrase.toLower().simplified();
            QHashIterator<int,int> i(requestRecovery);
            bool doContinue=false;
            while(i.hasNext()){
                i.next();
                //auto reason_phrase=ListenRequestCode::reasonPhrase(response_status_code).toLower().simplified();
                if(response_status_reason_phrase.contains(i.value())){
                    doContinue=true;
                    break;
                }
            }
            if(!doContinue)
                break;
        }

        auto recoveryCount=requestRecovery.contains(response_status_code);

        if(recoveryCount<=0)//if status code invalid then break
            break;

        if(recoveryCount<=executeCount)//if recovery limit then break
            break;
    }
    QObject::disconnect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    this->qrpcResponse.setResponse(&job->response());
    this->writeLog(&job->response(), job->response().toVariant());
    job->release();

#ifdef Q_RPC_LOG
    if(!this->parent->response().isOk()){
        rWarning()<<this->parent->toString();
        this->qrpcHeader.print();
    }
#endif
    return this->qrpcResponse;
}

}
