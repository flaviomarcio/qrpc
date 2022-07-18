#include "./p_qrpc_thread.h"
#include <QMutex>
#include <QTimer>

namespace QRpc {

class ThreadPvt:public QObject
{
public:
    QMutex lockRunWait;
    QMutex mutexRunning;
    explicit ThreadPvt(QObject* parent):QObject{parent}{}
};

Thread::Thread(QObject *parent) : QThread{nullptr}
{
    Q_UNUSED(parent)
    this->p = new ThreadPvt{this};
}

void Thread::run()
{
    if (p->mutexRunning.tryLock(1000)) {
        QTimer::singleShot(10, this, &Thread::eventRun);
        this->exec();
        p->mutexRunning.unlock();
    }
}

void Thread::eventRun()
{
    QThread::sleep(1);
    this->quit();
}

bool Thread::start()
{

    if (!p->mutexRunning.tryLock(1000))
        return false;
    p->lockRunWait.lock();
    QThread::start();
    while (this->eventDispatcher() == nullptr) {
        QThread::msleep(1);
    }
    p->lockRunWait.lock();
    p->lockRunWait.unlock();
    return this->isRunning();
}

bool Thread::stop()
{
    return this->quit();
}

bool Thread::quit()
{
    QThread::quit();
    return true;
}

} // namespace QRpc
