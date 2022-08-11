#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_listen_request.h"
#include <QDebug>
#include <QTcpSocket>
#include <QUrl>
#include <QSqlDatabase>
#include <QSqlDriver>

namespace QRpc {

//!
//! \brief The RequestJobDataBase class
//!
class RequestJobDataBase : public RequestJobProtocol
{
    Q_OBJECT
public:
    QVariantHash settings;
    ListenRequest listen_request;
    ListenRequest listen_response;
    RequestJobResponse*response=nullptr;
    QSqlDriver::DbmsType dbDriverType=QSqlDriver::UnknownDbms;
    QString sqlConnectionName=nullptr;
    QString requestPath;
    QString responsePath;
    QSqlDriver*sqlDriver=nullptr;

    Q_INVOKABLE explicit RequestJobDataBase(QObject *parent);

    ~RequestJobDataBase();


    ListenRequest&requestMake(RequestJobResponse*response);

    void connectionClose();


    bool connectionMake(QSqlDatabase&outConnection);

    virtual bool call(RequestJobResponse*response)override;

private slots:

    void onReceiveBroker(const QString &responseTopic, QSqlDriver::NotificationSource, const QVariant &responseBody);

    void onBrokerError(const QString &error);

    void onFinish();
};

}
