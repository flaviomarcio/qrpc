#include "./p_qrpc_request_job_wss.h"

namespace QRpc {

RequestJobWSS::RequestJobWSS(QObject *parent):RequestJobProtocol(parent)
{
}

RequestJobWSS::~RequestJobWSS()
{
}

bool RequestJobWSS::call(RequestJobResponse *response)
{

    this->response=response;

    QFile certFile(QStringLiteral(":/sslconfiguration/rpc.cert"));
    QFile keyFile(QStringLiteral(":/sslconfiguration/rpc.key"));

    if(!certFile.open(QIODevice::ReadOnly)){
        sWarning()<<tr("WebsocketListener: Cannot load certfile : %1").arg(certFile.fileName());
    }
    else if(!keyFile.open(QIODevice::ReadOnly)){
        sWarning()<<tr("WebsocketListener: Cannot load keyfile : %s").arg(keyFile.fileName());
    }
    else{
        QSslCertificate certificate(&certFile, QSsl::Pem);
        QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
        certFile.close();
        keyFile.close();
        QSslConfiguration sslConfiguration;
        sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
        sslConfiguration.setLocalCertificate(certificate);
        sslConfiguration.setPrivateKey(sslKey);

        QUrl url(response->request_url.toUrl());
        m_socket = new QWebSocket();
        QObject::connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &RequestJobWSS::onReplyError);
        QObject::connect(m_socket, &QWebSocket::connected, this, &RequestJobWSS::onConnected);
        QObject::connect(m_socket, &QWebSocket::disconnected, this, &RequestJobWSS::onClosed);
        QObject::connect(m_socket, &QWebSocket::binaryMessageReceived, this, &RequestJobWSS::onBinaryMessageReceived);
        QObject::connect(m_socket, &QWebSocket::pong, this, &RequestJobWSS::onReplyPong);
        QObject::connect(m_socket, &QWebSocket::readChannelFinished, this, &RequestJobWSS::onReplyReadFinished);
        /*
            m_socket->setSslConfiguration(sslConfiguration);
*/
        m_socket->open(url);
        return true;
    }
    return false;

}

void RequestJobWSS::onConnected()
{
    ListenRequest request(response->request_body);
    auto body=request.toJson();
    m_socket->sendBinaryMessage(body);
}

void RequestJobWSS::onClosed()
{
    //emit this->callback(QVariant());
}

void RequestJobWSS::onReplyError(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e)
    response->response_qt_status_code=QNetworkReply::UnknownServerError;
    emit this->callback(QVariant());
}

void RequestJobWSS::onBinaryMessageReceived(const QByteArray &message)
{
    auto socket=dynamic_cast<QWebSocket*>(QObject::sender());
    if(socket!=nullptr){
        Q_UNUSED(message)
        this->buffer.append(message);
        socket->close();
    }
}

void RequestJobWSS::onReplyTimeout()
{
    response->response_qt_status_code=QNetworkReply::TimeoutError;
    emit this->callback(QVariant());
}

void RequestJobWSS::onReplyPong(quint64 elapsedTime, const QByteArray &payload)
{
    Q_UNUSED(elapsedTime)
    Q_UNUSED(payload)
}

void RequestJobWSS::onReplyReadFinished(){
    //        if(response->response_qt_status_code!=QNetworkReply::TimeoutError)
    //            response->response_qt_status_code = this->reply->error();
    this->onFinish();
}

void RequestJobWSS::onFinish()
{
    ListenRequest request(this->buffer);
    if(!request.isValid()){
        response->response_qt_status_code=QNetworkReply::InternalServerError;
        response->response_status_code=404;
        response->response_body.clear();
    }
    else if(response->response_status_code!=QNetworkReply::TimeoutError){
        auto code=request.responseCode();
        response->response_qt_status_code = QNetworkReply::NetworkError(code==200?0:code);
        response->response_body = request.responseBody().toByteArray();
        response->response_status_code = request.responseCode();
        response->response_status_reason_phrase = request.responsePhrase();
        if(response->response_status_code==0)
            response->response_status_code=response->response_qt_status_code;
    }

    emit this->callback(QVariant());
}

void RequestJobWSS::onReplySslError(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
#ifdef QT_DEBUG
    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    m_socket->ignoreSslErrors();
#endif
}

}
