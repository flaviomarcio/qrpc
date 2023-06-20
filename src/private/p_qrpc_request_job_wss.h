#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_macro.h"
#include "../qrpc_listen_request.h"
#include <QtWebSockets/QWebSocket>
#include <QFile>

namespace QRpc {

//!
//! \brief The RequestJobWSS class
//!
class RequestJobWSS : public RequestJobProtocol
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RequestJobWSS(QObject *parent);

    ~RequestJobWSS();

    RequestJobResponse*response=nullptr;
    QWebSocket*m_socket=nullptr;
    QByteArray buffer;


    virtual bool call(RequestJobResponse*response)override;

private slots:

    void onConnected();

    void onClosed();

    void onReplyError(QAbstractSocket::SocketError e);

    void onBinaryMessageReceived(const QByteArray &message);


    void onReplyTimeout();

    void onReplyPong(quint64 elapsedTime, const QByteArray &payload);

    void onReplyReadFinished();

    void onFinish();

    void onReplySslError(const QList<QSslError> &errors);
};

}
