#pragma once

#include "./p_qrpc_request_job_response.h"
#include <QTemporaryFile>

namespace QRpc {

//!
//! \brief The RequestJobProtocol class
//!
class RequestJobProtocol : public QObject
{
    Q_OBJECT
public:
    Request::Action action=Request::acRequest;
    QString action_fileName;
    QSslConfiguration sslConfiguration;

    Q_INVOKABLE explicit RequestJobProtocol(QObject *parent);

    ~RequestJobProtocol();

    virtual RequestJobProtocol&clear();

    virtual bool call(RequestJobResponse*response);

signals:
    void callback(const QVariant &vHash);
    void ____verify(const QVariant &v);
public slots:
    virtual void ____onVerify(const QVariant &v);
};

}
