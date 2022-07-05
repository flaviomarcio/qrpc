#pragma once

#include <QObject>
#include <QThread>
#include <QUuid>
#include <QVariant>
#include <QVariantHash>
#include <QVariantList>
#include <QDateTime>
#include "./qrpc_service_manager.h"
#include "./qrpc_service_setting.h"

namespace QRpc {
class ServiceThreadPvt;
//!
//! \brief The ServiceThread class
//!
class Q_RPC_EXPORT ServiceThread : public QThread
{
    Q_OBJECT
public:

    //!
    //! \brief The State enum
    //!
    enum State{
        Success=0, Error=1, Discarted=2, Canceled=3
    };

    //!
    //! \brief The Stats struct
    //!
    struct Stats{
    public:
        QDateTime started=QDateTime::currentDateTime();
        int queue=0;
        int received=0;
        int success=0;
        int error=0;
        int discated=0;
        int canceled=0;
        void clear();
        QVariantHash toMap() const;
        QVariantHash toHash() const;
    private:
        ServiceThread*service=nullptr;
    };

    //!
    //! \brief ServiceThread
    //! \param parent
    //!
    Q_INVOKABLE explicit ServiceThread(QObject *parent = nullptr);
    explicit ServiceThread(const QString &serviceName, QObject *parent = nullptr);
    explicit ServiceThread(const ServiceManager&manager,QObject *parent = nullptr);
    explicit ServiceThread(const ServiceManager&manager, const QString &serviceName, QObject *parent = nullptr);

    //!
    //! \brief setting
    //! \return
    //!
    virtual QRpc::ServiceSetting &setting();

    //!
    //! \brief manager
    //! \return
    //!
    virtual QRpc::ServiceManager&manager();

    //!
    //! \brief run
    //!
    void run() override;

    //!
    //! \brief start
    //! \return
    //!
    virtual bool start();
    virtual bool start(const ServiceManager &manager);

    /**
     * @brief stop
     * @return
     */
    virtual bool stop();

    //!
    //! \brief stats
    //! \return
    //!
    virtual Stats &stats() const;

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantHash toMap() const;


    //!
    //! \brief serviceName
    //! \return
    //!
    QString serviceName() const;

    //!
    //! \brief setServiceName
    //! \param value
    //!
    void setServiceName(const QString &value);

    //!
    //! \brief received
    //! \param uuid
    //! \param v
    //!
    virtual void received(const QUuid &uuid, const QVariant &v);

    //!
    //! \brief dispatcher
    //! \param uuid
    //! \param v
    //!
    void dispatcher(const QUuid &uuid, const QVariant &v);

signals:

    //!
    //! \brief service_started
    //! \param thread
    //!
    void service_started(QRpc::ServiceThread*thread);

    //!
    //! \brief service_terminate
    //! \param thread
    //!
    void service_terminate(QRpc::ServiceThread*thread);

    //!
    //! \brief request_send
    //! \param uuid
    //! \param v
    //!
    void request_send(const QUuid &uuid, const QVariant &v);

    //!
    //! \brief request_received
    //! \param uuid
    //! \param v
    //!
    void request_received(const QUuid &uuid, const QVariant &v);

    //!
    //! \brief request_success
    //! \param uuid
    //! \param detail
    //!
    void request_success(const QUuid &uuid, const QVariant&detail);

    //!
    //! \brief request_error
    //! \param uuid
    //! \param detail
    //!
    void request_error(const QUuid &uuid, const QVariant&detail);

    //!
    //! \brief request_discated
    //! \param uuid
    //! \param detail
    //!
    void request_discated(const QUuid &uuid, const QVariant&detail);

    //!
    //! \brief request_canceled
    //! \param uuid
    //! \param detail
    //!
    void request_canceled(const QUuid&uuid, const QVariant&detail);

    //!
    //! \brief request_state
    //! \param uuid
    //! \param state
    //! \param detail
    //!
    void request_state(const QUuid &uuid, const QRpc::ServiceThread::State state, const QVariant&detail);
private:
    ServiceThreadPvt *p=nullptr;
};

}

