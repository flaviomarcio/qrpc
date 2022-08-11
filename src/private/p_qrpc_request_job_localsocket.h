#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_listen_request.h"
#include <QTcpSocket>
#include <QUrl>

namespace QRpc {

//!
//! \brief The RequestJobLocalSocket class
//!
class RequestJobLocalSocket : public RequestJobProtocol
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RequestJobLocalSocket(QObject *parent);

    ~RequestJobLocalSocket();

    RequestJobResponse*response=nullptr;
    QTcpSocket*m_socket=nullptr;
    QByteArray buffer;


    virtual bool call(RequestJobResponse*response)override;

private slots:

    void onConnected();

    void onReplyError(QAbstractSocket::SocketError e);

    void onFinish();
};

}
