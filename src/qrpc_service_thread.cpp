#include "./qrpc_service_thread.h"
#include <QTimer>
#include <QMutex>

namespace QRpc {


static qlonglong serviceThreadCount=0;

class ServiceThreadPvt: public QObject{
public:
    QMutex mutex;
    QDateTime serviceStarted=QDateTime::currentDateTime();
    SettingManager manager;
    QString serviceName;
    QRpc::ServiceSetting setting;
    ServiceThread*service=nullptr;
    ServiceThread::Stats stats;

    explicit ServiceThreadPvt(ServiceThread*parent) : QObject{parent}, manager(parent), service(parent){
        parent->moveToThread(parent);
    }

    virtual ~ServiceThreadPvt(){
    }

    void setManager(const SettingManager&manager){
        this->manager.clear();
        this->manager.load(manager);
    }

public slots:
    void onRequestReceived(const QUuid &uuid, const QVariant &v){
        --stats.queue;
        emit service->request_state(uuid, service->Success, v);
        service->received(uuid, v);
    }
    void onRequestSuccess(const QUuid &uuid, const QVariant &detail){
        QMutexLocker<QMutex> locker(&this->mutex);
        ++stats.success;
        emit service->request_state(uuid, service->Success, detail);
    }
    void onRequestError(const QUuid &uuid, const QVariant &detail){
        QMutexLocker<QMutex> locker(&this->mutex);
        ++stats.error;
        emit service->request_state(uuid, service->Error, detail);
    }
    void onRequestDiscarted(const QUuid &uuid, const QVariant &detail){
        QMutexLocker<QMutex> locker(&this->mutex);
        ++stats.discated;
        emit service->request_state(uuid, service->Discarted, detail);
    }
    void onRequestCanceled(const QUuid &uuid, const QVariant &detail){
        QMutexLocker<QMutex> locker(&this->mutex);
        ++stats.canceled;
        emit service->request_state(uuid, service->Canceled, detail);
    }
};


void ServiceThread::Stats::clear()
{
    this->started=QDateTime::currentDateTime();
    this->received=0;
    this->success=0;
    this->error=0;
    this->discated=0;
    this->canceled=0;
}

QVariantHash ServiceThread::Stats::toMap()const
{
    return this->toHash();
}

QVariantHash ServiceThread::Stats::toHash() const
{
    QVariantHash v;
    v.insert(QStringLiteral("running" ),(this->service==nullptr)?false:this->service->isRunning());
    v.insert(QStringLiteral("started" ), this->started     );
    v.insert(QStringLiteral("received"), this->received    );
    v.insert(QStringLiteral("success" ), this->success     );
    v.insert(QStringLiteral("error"   ), this->error       );
    v.insert(QStringLiteral("discated"), this->discated    );
    v.insert(QStringLiteral("canceled"), this->canceled    );
    return v;

}

ServiceThread::ServiceThread(QObject *parent) : QThread{nullptr}
{
    Q_UNUSED(parent)
    this->p = new ServiceThreadPvt{this};

    p->service=this;
}

ServiceThread::ServiceThread(const QString &serviceName, QObject *parent) : QThread{nullptr}
{
    Q_UNUSED(parent)
    this->p = new ServiceThreadPvt{this};

    p->service=this;
    this->setServiceName(serviceName);
}

ServiceThread::ServiceThread(const SettingManager &manager, QObject *parent) : QThread{nullptr}
{
    Q_UNUSED(parent)
    this->p = new ServiceThreadPvt{this};

    p->service=this;
    p->setManager(manager);
}

ServiceThread::ServiceThread(const SettingManager &manager, const QString &serviceName, QObject *parent) : QThread{nullptr}
{
    Q_UNUSED(parent)
    this->p = new ServiceThreadPvt{this};

    p->service=this;
    p->setManager(manager);
    this->setServiceName(serviceName);
}

QRpc::ServiceSetting&ServiceThread::setting()
{

    if(!p->setting.isValid()){
        p->setting=p->manager.setting(p->serviceName);
    }
    return p->setting;
}

SettingManager &ServiceThread::manager()
{

    return p->manager;
}

void ServiceThread::run()
{

    p->serviceStarted=QDateTime::currentDateTime();

    QObject::connect(this, &ServiceThread::request_send    , this->p, &ServiceThreadPvt::onRequestReceived );
    QObject::connect(this, &ServiceThread::request_success , this->p, &ServiceThreadPvt::onRequestSuccess  );
    QObject::connect(this, &ServiceThread::request_error   , this->p, &ServiceThreadPvt::onRequestError    );
    QObject::connect(this, &ServiceThread::request_discated, this->p, &ServiceThreadPvt::onRequestDiscarted );
    QObject::connect(this, &ServiceThread::request_canceled, this->p, &ServiceThreadPvt::onRequestCanceled );

    QTimer::singleShot(5,this,[this](){emit ServiceThread::service_started(this);});
    this->exec();

    QObject::disconnect(this, &ServiceThread::request_received, this->p, &ServiceThreadPvt::onRequestReceived );
    QObject::disconnect(this, &ServiceThread::request_success , this->p, &ServiceThreadPvt::onRequestSuccess  );
    QObject::disconnect(this, &ServiceThread::request_error   , this->p, &ServiceThreadPvt::onRequestError    );
    QObject::disconnect(this, &ServiceThread::request_discated, this->p, &ServiceThreadPvt::onRequestDiscarted );
    QObject::disconnect(this, &ServiceThread::request_canceled, this->p, &ServiceThreadPvt::onRequestCanceled );

    emit this->service_terminate(this);
}

bool ServiceThread::start()
{

    if(!this->isRunning()){
        auto objectName=this->objectName().trimmed();

        if(objectName.isEmpty()){
            objectName=p->serviceName.trimmed();

            if(objectName.isEmpty())
                objectName=QThread::currentThread()->objectName().trimmed();

            if(objectName.isEmpty())
                objectName=QString::number(qlonglong(QThread::currentThreadId()), 16);

            objectName=QStringLiteral("%1_%2").arg(objectName).arg(++serviceThreadCount);
        }
        p->manager.clear();
        QThread::start();
        while(QThread::eventDispatcher()!=nullptr)
            QThread::msleep(1);
    }
    return false;
}

bool ServiceThread::start(const SettingManager&manager)
{

    if(!this->isRunning()){
        auto objectName=this->objectName().trimmed();

        if(objectName.isEmpty()){
            objectName=p->serviceName.trimmed();

            if(objectName.isEmpty())
                objectName=QThread::currentThread()->objectName().trimmed();

            if(objectName.isEmpty())
                objectName=QString::number(qlonglong(QThread::currentThreadId()), 16);

            objectName=QStringLiteral("%1_%2").arg(objectName).arg(++serviceThreadCount);
        }
        p->setManager(manager);
        QThread::start();
        while(QThread::eventDispatcher()!=nullptr)
            QThread::msleep(1);
    }
    return false;
}

bool ServiceThread::stop()
{
    if(this->isRunning()){
        this->quit();
        return true;
    }
    return false;
}

ServiceThread::Stats&ServiceThread::stats() const
{

    return p->stats;
}

void ServiceThread::received(const QUuid &uuid, const QVariant &v)
{
    Q_UNUSED(uuid)
    Q_UNUSED(v)
    emit this->request_success(uuid, QVariant());
}

void ServiceThread::dispatcher(const QUuid &uuid, const QVariant &v)
{

    QMutexLocker<QMutex> locker(&p->mutex);
    ++p->stats.queue;
    emit this->request_send(uuid, v);
}

QVariantHash ServiceThread::toMap() const
{

    QVariantHash v;
    v.insert(QStringLiteral("service"), this->objectName());
    v.insert(QStringLiteral("stats"  ), p->stats.toHash()   );
    return v;
}

QString ServiceThread::serviceName() const
{

    return p->serviceName;
}

void ServiceThread::setServiceName(const QString &value)
{

    p->serviceName = value.trimmed();
}

}
