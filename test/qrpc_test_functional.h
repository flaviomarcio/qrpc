#pragma once

#include "./qrpc_test.h"
#include "../src/qrpc_macro.h"
#include "../src/qrpc_server.h"
#include "../src/qrpc_types.h"
#include "../src/qrpc_listen_protocol.h"
#include "../src/qrpc_controller.h"

namespace QRpc {

class ServerTest : public QRpc::Server
{
    Q_OBJECT
    Q_DECLARE_INSTANCE(Server)
public:
    Q_INVOKABLE explicit ServerTest(QObject *parent = nullptr):QRpc::Server(parent)
    {
    }
private:
    void*p=nullptr;
};

class Q_RPC_Interface : public QRpc::QRPCController
{
    Q_OBJECT

    Q_ANNOTATION(Q_RPC_Interface, nl({apiBasePath("/")}))
    Q_RPC_DECLARE_INTERFACE_METHOD_CHECK()
public:
    Q_INVOKABLE explicit Q_RPC_Interface(QObject *parent=nullptr):QRpc::QRPCController(parent)
    {
    }

    Q_ANNOTATION(check_head, {opHead})
    Q_INVOKABLE QVariant check_head()
    {
        return QDate::currentDate();
    }

    Q_ANNOTATION(check_options, {opOptions})
    Q_INVOKABLE QVariant check_options()
    {
        return QDate::currentDate();
    }

    Q_ANNOTATION(check_get, {opGet})
    Q_INVOKABLE QVariant check_get()
    {
        return QDate::currentDate();
    }

    Q_ANNOTATION(check_post, {opPost})
    Q_INVOKABLE QVariant check_post()
    {
        return QDate::currentDate();
    }

    Q_ANNOTATION(check_put, {opPut})
    Q_INVOKABLE QVariant check_put()
    {
        return QDate::currentDate();
    }

    Q_ANNOTATION(check_delete, {opDelete})
    Q_INVOKABLE QVariant check_delete()
    {
        return QDate::currentDate();
    }

    Q_ANNOTATION(check_patch, {opPatch})
    Q_INVOKABLE QVariant check_patch()
    {
        return QDate::currentDate();
    }

    Q_ANNOTATION(check_trace, {opTrace})
    Q_INVOKABLE QVariant check_trace()
    {
        return QDate::currentDate();
    }
};

QRPC_CONTROLLER_AUTO_REGISTER(Q_RPC_Interface)

struct PublicRecord{
public:
    int server_port_http=9999;
    QByteArray server_protocol="http";
    QByteArray server_hostname="localhost";
    QByteArray server_broker_database_driver="QPSQL";
    QByteArray server_broker_database_username="erpv1";
    QByteArray server_broker_database_password="erpv1";
    int server_broker_database_port=25432;
};

Q_GLOBAL_STATIC(PublicRecord, __public_record);
static auto&public_record=*__public_record;

class SDKGoogleTestFunctional : public SDKGoogleTest{
public:

    PublicRecord &publicRecord=public_record;
    static QRpc::Server&service()
    {
        auto&instance=QRpc::ServerTest::instance();
        if(instance.isRunning())
            return instance;
        instance.setObjectName(QT_STRINGIFY(Server));
        auto&protocol=instance.colletions().protocol(QRpc::Http);
        protocol.setEnabled(true);
        protocol.setPort(public_record.server_port_http);
        return instance;
    }

    explicit SDKGoogleTestFunctional():SDKGoogleTest()
    {
    }

    virtual bool serviceStart()
    {
        auto __return=this->service().start();
        QThread::sleep(1);
        return __return;
    }

    virtual bool serviceStop()
    {
        return this->service().stop();
    }

public:
};

}
