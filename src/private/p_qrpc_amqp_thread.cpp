#include "./p_qrpc_amqp_thread.h"
#include <QMutex>
#include <QTimer>

namespace QRpc {


static QMutex mutex_ping;


AMQPThread::AMQPThread(QObject *parent):QThread(nullptr){
    Q_UNUSED(parent)
}

AMQPThread::~AMQPThread(){
}

void AMQPThread::run(){

    auto on_ping=[this](){
        mutex_ping.lock();
        this->on_ping();
        mutex_ping.unlock();
    };

    QTimer::singleShot(100,[this, &on_ping](){
        this->___threadPing =&ThreadPing::start()->on(on_ping);
    });

    this->exec();
}

ThreadPing &AMQPThread::threadPing(){
    return*this->___threadPing;
}

void AMQPThread::on_ping(){
//    if(this->handle!=nullptr){
//        if(channel->usable()){
//            this->handle->goConsumer();
//        }
//    }
}

}
