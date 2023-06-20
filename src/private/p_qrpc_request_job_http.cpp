#include "./p_qrpc_request_job_http.h"
#include "../qrpc_const.h"
#if Q_RPC_LOG
#include "../qrpc_macro.h"
#endif
#include <QStandardPaths>

namespace QRpc {

static const auto __underline="_";

RequestJobHttp::RequestJobHttp(QObject *parent):RequestJobProtocol(parent)
{
    this->tempLocation=QStringLiteral("%1/%2.%3.tmp").arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation), qAppName().replace(QStringLiteral(" "), __underline),QStringLiteral("%1"));
}

RequestJobHttp::~RequestJobHttp()
{
    this->fileFree();

    if(this->nam!=nullptr)
        this->nam->deleteLater();
}

void RequestJobHttp::fileMake()
{
    this->fileFree();
    static int fileTempNumber=0;

    auto fileTemp=this->tempLocation.arg(++fileTempNumber);
    this->fileTemp.setFileName(fileTemp);
}

void RequestJobHttp::fileFree()
{
    if(this->fileTemp.isOpen())
        this->fileTemp.close();

    if(this->fileTemp.exists())
        this->fileTemp.remove();
}

bool RequestJobHttp::call(RequestJobResponse *response)
{
    static const QStringList removeHeaders={{QStringLiteral("host")},{QStringLiteral("content-length")}};
    static const QStringList ignoreHeaders={{ContentDispositionName}, {ContentTypeName}, {QString(ContentDispositionName).toLower()}, {QString(ContentTypeName).toLower()}};

    if(this->nam==nullptr)
        this->nam = new QNetworkAccessManager(nullptr);

#if Q_RPC_LOG_SUPER_VERBOSE
    rWarning()<<tr("request.url: %1").arg(response->request_url);
#endif

    const auto &request_url=response->request_url.toUrl();
    this->response=response;

    const auto &requestBody = response->request_body;
    this->request=QNetworkRequest(QUrl(request_url));
    //this->request.setSslConfiguration(sslConfiguration);
    QHash<QByteArray,QByteArray> request_header_ignored;
    QHash<QByteArray,QByteArray> request_header;

    auto configureHeaders=[this, &request_header, &request_header_ignored, &response](){

        if(!response->request_header.isEmpty()){
            QHashIterator<QString, QVariant> i(response->request_header);
            while (i.hasNext()) {
                i.next();
                if(removeHeaders.contains(i.key().toLower()))
                    continue;

                auto v=i.value();
                QStringList headerValues;
                switch (v.typeId()) {
                case QMetaType::QVariantList:
                case QMetaType::QStringList:
                {
                    auto vList=v.toList();
                    for(auto &r:vList)
                        headerValues.append(r.toString().replace(QStringLiteral("\n"), QStringLiteral(";")));
                    break;
                }
                case QMetaType::QVariantHash:
                case QMetaType::QVariantMap:
                {
                    auto vMap=v.toHash();
                    QHashIterator<QString, QVariant> i(vMap);
                    while (i.hasNext()) {
                        i.next();
                        auto r=QStringLiteral("%1=%2").arg(i.value().toString(), v.toString()).replace(QStringLiteral("\n"), QStringLiteral(";"));
                        headerValues.append(r);
                    }
                    break;
                }
                default:
                    headerValues.append(v.toString());
                }

                v=headerValues.join(QStringLiteral("; "));
                auto k=i.key().toUtf8().trimmed();
                if(!ignoreHeaders.contains(k.toLower()))
                    request_header.insert(k, v.toByteArray());
                else
                    request_header_ignored.insert(k, v.toByteArray());
#if Q_RPC_LOG_SUPER_VERBOSE
                rInfo()<<":request.setRawHeader("<<i.key()<<", "<<i.value()<<")";
#endif
            }
        }

        if(!request_header.isEmpty()){
            QHashIterator<QByteArray,QByteArray> i(request_header);
            while (i.hasNext()) {
                i.next();
                const auto &k=i.key();
                const auto &v=i.value();
                this->request.setRawHeader(k, v);
            }
        }
    };

    configureHeaders();

    auto configureHeadersIgnored=[this, &request_header_ignored](){

        QHashIterator<QByteArray,QByteArray> i(request_header_ignored);
        while (i.hasNext()) {
            i.next();
            const auto &k=i.key();
            const auto &v=i.value();
            this->request.setRawHeader(k, v);
        }
    };

    switch (action) {
    case Request::acUpload:
    {
        if(this->fileUpload.isOpen())
            this->fileUpload.close();
        this->fileUpload.setFileName(this->action_fileName);
        if(!this->fileUpload.exists()){
            auto msg=QStringLiteral("invalid upload fileName:%1")+this->fileDownload.fileName();
            rWarning()<<msg;
            response->response_qt_status_code=QNetworkReply::NoError;
            response->response_status_code=-1;
            response->response_status_reason_phrase=msg.toUtf8();
        }
        else if(!this->fileUpload.open(this->fileUpload.ReadOnly)){
            auto msg=QStringLiteral("invalid upload fileName:%1, %2").arg(this->fileDownload.fileName(), this->fileUpload.errorString());
            rWarning()<<msg;
            response->response_qt_status_code=QNetworkReply::NoError;
            response->response_status_code=-1;
            response->response_status_reason_phrase=msg.toUtf8();
        }
        else{
            auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
            QHttpPart httpPart;
            QHashIterator<QByteArray,QByteArray> i(request_header_ignored);
            while (i.hasNext()) {
                i.next();
                const auto &k=i.key();
                const auto &v=i.value();
                httpPart.setRawHeader( k, v );
            }
            httpPart.setBodyDevice(&this->fileUpload);
            multiPart->append(httpPart);
            this->reply=nam->post(this->request, multiPart);
            multiPart->setParent(reply);
            QObject::connect(this->reply, &QNetworkReply::uploadProgress , this, &RequestJobHttp::onReplyProgressUpload);
        }
        break;
    }
    case Request::acDownload:
    {
        configureHeadersIgnored();
        this->fileMake();
        this->fileDownload.setFileName(this->action_fileName);
        if(!this->fileDownload.open(QFile::Truncate | QFile::WriteOnly | QFile::Unbuffered)){
            auto msg=QStringLiteral("invalid download fileName: %1").arg(this->fileDownload.fileName());
            rWarning()<<msg;
            response->response_qt_status_code=QNetworkReply::NoError;
            response->response_status_code=-1;
            response->response_status_reason_phrase = msg.toUtf8();
        }
        else if(!this->fileTemp.open(QFile::Truncate | QFile::Unbuffered | QFile::WriteOnly)){
            auto msg=QStringLiteral("invalid download temporary fileName: %1").arg(this->fileTemp.fileName());
            rWarning()<<msg;
            response->response_qt_status_code=QNetworkReply::NoError;
            response->response_status_code=-1;
            response->response_status_reason_phrase = msg.toUtf8();
        }
        else{
            this->reply=nam->get(this->request);
            QObject::connect(this->reply, &QNetworkReply::downloadProgress , this, &RequestJobHttp::onReplyProgressDownload);
        }
        break;
    }
    default:
        configureHeadersIgnored();
        auto method=response->request_exchange.call().method();
        auto methodName=response->request_exchange.call().methodName().toUtf8().toUpper();

        switch (method) {
        case QRpc::Post:
        case QRpc::Put:
        {
            auto v=this->request.header(QNetworkRequest::ContentTypeHeader);
            if(!v.isValid()){
                this->request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
            }
            break;
        }
        default:
            break;
        }

        switch (method) {
        case QRpc::Head:
            this->reply=nam->head(this->request);
            break;
        case QRpc::Options:
            this->reply=nam->sendCustomRequest(this->request, methodName);
            break;
        case QRpc::Get:
            this->reply=nam->get(this->request);
            break;
        case QRpc::Post:
            this->reply=nam->post(this->request, requestBody);
            break;
        case QRpc::Put:
            this->reply=nam->put(this->request, requestBody);
            break;
        case QRpc::Delete:
            this->reply=nam->deleteResource(this->request);
            break;
        default:
            this->reply=nam->sendCustomRequest(this->request, methodName, requestBody);
        }
    }


    response->request_start=QDateTime::currentDateTime();
    if(this->reply==nullptr){
        return false;
    }

    response->response_qt_status_code=QNetworkReply::NoError;
    response->response_status_code=0;

    QObject::connect(this->reply, &QNetworkReply::destroyed, this, &RequestJobHttp::onReplyDelete);
    QObject::connect(this->reply, &QNetworkReply::finished, this, &RequestJobHttp::onReplyFinish);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QObject::connect(this->reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), this, &RequestJobHttp::onReplyError);
#else
    QObject::connect(this->reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &RequestJobHttp::onReplyError);
#endif
    this->reply->ignoreSslErrors();

    return true;
}

void RequestJobHttp::timeout_start()
{
    if(response->activityLimit<=0)
        return;

    this->__timeout=new QTimer(nullptr);
    this->__timeout->setInterval(response->activityLimit);
    QObject::connect(this->__timeout, &QTimer::timeout, this, &RequestJobHttp::onReplyTimeout);
    this->__timeout->start();
}

void RequestJobHttp::timeout_stop()
{
    auto timer=this->__timeout;
    this->__timeout=nullptr;
    if(timer!=nullptr){
        timer->stop();
        QObject::disconnect(timer, &QTimer::timeout, this, &RequestJobHttp::onReplyTimeout);
        timer->deleteLater();
    }
}

void RequestJobHttp::onReplyError(QNetworkReply::NetworkError e)
{
    if(response->response_qt_status_code==QNetworkReply::NoError)
        response->response_qt_status_code=e;

    this->onFinish();
}

void RequestJobHttp::onReplyFinish()
{
    if(this->reply==nullptr)
        return;

    QMutexLocker<QMutex> locker(&mutexRequestFinished);
    if(response->response_qt_status_code!=QNetworkReply::NoError)
        return;

    if(response->response_qt_status_code!=QNetworkReply::TimeoutError)
        response->response_qt_status_code = this->reply->error();
    this->onFinish();

}

void RequestJobHttp::onReplyTimeout()
{
    {
        QMutexLocker<QMutex> locker(&mutexRequestFinished);
        if(response->response_qt_status_code==QNetworkReply::NoError){
            response->response_qt_status_code=QNetworkReply::TimeoutError;
#if Q_RPC_LOG
            rWarning()<<":local timeout forced, url:"<<this->response->request_url;
#endif
        }
    }
    //gera crash
    //if(this->reply!=nullptr)
    //    this->reply->abort();
    this->reply=nullptr;
    emit this->callback(QVariant());
}

void RequestJobHttp::onReplyDelete()
{
    this->reply=nullptr;
}

void RequestJobHttp::onReplyProgressUpload(qint64 bytesSent, qint64 bytesTotal)
{
    Q_UNUSED(bytesSent)
    Q_UNUSED(bytesTotal)
}

void RequestJobHttp::onReplyProgressDownload(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived)
    Q_UNUSED(bytesTotal)
    if(this->reply==nullptr)
        return;

    if(!this->fileTemp.isOpen()){
        rWarning()<<tr("invalid fileTemp: %1").arg(this->fileTemp.fileName());
        return;
    }
    const auto bytes=this->reply->read(bytesReceived);
    this->fileTemp.write(bytes);
    this->fileTemp.flush();
}

void RequestJobHttp::onFinish()
{
    this->timeout_stop();

    response->request_finish=QDateTime::currentDateTime();
    if(this->fileTemp.isOpen()){
        auto fileName=this->fileTemp.fileName();
        this->fileDownload.close();
        QFile::remove(this->fileDownload.fileName());
        QFile::rename(fileName, this->fileDownload.fileName());
        QFile::remove(this->fileTemp.fileName());
        this->fileFree();
    }


    auto localNan=this->nam;
    this->nam=nullptr;

    auto localReply=this->reply;
    this->reply=nullptr;

    if(localReply!=nullptr){
        for(auto &v:localReply->rawHeaderPairs())
            response->responseHeader.insert(v.first, v.second);

        if(response->response_status_code!=QNetworkReply::TimeoutError){
            response->response_qt_status_code = localReply->error();
            response->response_body = localReply->readAll();
            response->response_status_code = localReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            response->response_status_reason_phrase = localReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
            if(response->response_qt_status_code==QNetworkReply::ConnectionRefusedError)
                response->response_status_code=502;
            if(response->response_status_code==0)
                response->response_status_code=response->response_qt_status_code;
        }

        localReply->abort();
        delete localReply;
    }
    if(localNan!=nullptr){
        delete localNan;
    }
    emit this->callback(QVariant());
}

}
