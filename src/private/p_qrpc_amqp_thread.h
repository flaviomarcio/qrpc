#ifndef QRPC_AMQPThread_H
#define QRPC_AMQPThread_H


#include <QThread>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <memory>
#ifdef COMPILE_AMQP
#include <amqpcpp.h>
#include <amqpcpp/message.h>
#endif
//#include "./p_qrpc_amqp_handle.h"
#include "./p_qrpc_thread_ping.h"

namespace QRpc {

class AMQPThread:public QThread{
    Q_OBJECT

public:
    Q_INVOKABLE explicit AMQPThread(QObject* parent=nullptr);

    virtual ~AMQPThread();

    void run();

    ThreadPing&threadPing();

public:
    ThreadPing*___threadPing=nullptr;

//    AMQPHandle*handle=nullptr;
//    AMQP::Connection*connection=nullptr;
//    AMQP::Channel*channel=nullptr;
    //public slots:
    //    void ___on_update(){
    //        this->connection->close();
    //    }
    //signals:
    //    void ___update();
private slots:
    virtual void on_ping();
};


}
#endif
