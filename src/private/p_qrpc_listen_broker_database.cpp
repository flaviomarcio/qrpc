#include "./p_qrpc_listen_broker_database.h"
#include "../../../qstm/src/qstm_url.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_listen_protocol.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_cache.h"
#include "../qrpc_const.h"
#if Q_RPC_LOG
#include "../qrpc_macro.h"
#endif
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>


namespace QRpc {

class DataBaseListenerServer : public QObject
{
public:
    QHash<QString, QVariantHash> listenersOptions;
    QHash<QString, QSqlDatabase> listenersQSqlDatabase;
    QHash<QString, QSqlDriver *> listenersQSqlDrivers;
    QMutex lock;
    bool realMessageOnException = false;

    explicit DataBaseListenerServer(QObject *parent = nullptr) : QObject{parent}
    {
        Q_UNUSED(parent)
        this->realMessageOnException = false;
    }

    ListenBrokerDataBase *listen()
    {
        auto _listen = dynamic_cast<ListenBrokerDataBase *>(this->parent());
        return _listen;
    }

    bool queueStart()
    {
        auto colletions = this->listen()->colletions();
        auto &option = colletions->protocol(Protocol::DataBase);
        for (auto &v : option.port()) {
            auto port = v.toInt();
            if (port <= 0)
                continue;

            if (!this->realMessageOnException)
                this->realMessageOnException = option.realMessageOnException();
            auto settings = option.makeSettingsHash();
            settings[QStringLiteral("port")] = port;
            auto connection = queueMake(settings);
            if (connection.isValid() && connection.isOpen()) {
                this->listenersQSqlDatabase.insert(connection.connectionName(), connection);
                this->listenersQSqlDrivers.insert(connection.connectionName(), connection.driver());
            }
        }
        this->queueCheck();
        return this->isListening();
    }

    void queueRemove(const QString &connectionName)
    {
        auto __connection = listenersQSqlDatabase.value(connectionName);
        if (__connection.isValid()) {
            QObject::disconnect(
                __connection.driver(),
                QOverload<const QString &, QSqlDriver::NotificationSource, const QVariant &>::of(
                    &QSqlDriver::notification),
                this,
                &DataBaseListenerServer::service);
            auto name = __connection.connectionName();
            __connection.close();
            __connection = {};
            listenersQSqlDrivers.remove(connectionName);
            listenersQSqlDatabase.insert(connectionName, __connection);
            QSqlDatabase::removeDatabase(name);
        }
    }

    QSqlDatabase queueMake(const QVariantHash &options)
    {
        Q_UNUSED(options)

        auto driver = options[QStringLiteral("driver")].toString().trimmed();
        auto hostName = options[QStringLiteral("hostName")].toString().trimmed();
        auto userName = options[QStringLiteral("userName")].toString().trimmed();
        auto password = options[QStringLiteral("password")].toString().trimmed();
        auto dataBaseName = options[QStringLiteral("databaseName")].toString().trimmed();
        auto port = options[QStringLiteral("port")].toLongLong();
        auto connectOptions = options[QStringLiteral("connectionOptions")].toString().trimmed();

        auto connectionName = QStringLiteral("broker-%1-%2").arg(driver).arg(port).toLower();

        queueRemove(connectionName);

        this->listenersQSqlDatabase.insert(connectionName,
                                           QSqlDatabase::addDatabase(driver, connectionName));
        this->listenersOptions.insert(connectionName, options);

        auto __connection = this->listenersQSqlDatabase.value(connectionName);
        __connection.setHostName(hostName);
        __connection.setUserName(userName);
        __connection.setPassword(password);
        __connection.setDatabaseName(dataBaseName);
        __connection.setPort(port);
        __connection.setConnectOptions(connectOptions);
        if (__connection.open()) {
            auto sqlDriver = __connection.driver();
            this->listenersQSqlDrivers.insert(connectionName, sqlDriver);
            auto sqlConnectionName = __connection.connectionName();
            if (!sqlDriver->subscribeToNotification(sqlConnectionName)) {
                auto msg = __connection.lastError().text();
                rWarning() << msg;
            } else {
                QObject::connect(
                    sqlDriver,
                    QOverload<const QString &, QSqlDriver::NotificationSource, const QVariant &>::of(
                        &QSqlDriver::notification),
                    this,
                    &DataBaseListenerServer::service);
            }
        }
        return __connection;
    }

public slots:
    void queueCheck()
    {
        if (this->lock.tryLock(1000)) {
            Q_V_DATABASE_ITERATOR(this->listenersQSqlDatabase)
            {
                i.next();
                const auto command = QString("select 0--broker-check");
                auto q = i.value().exec(command);
                if (!q.isActive()) {
                    queueMake(this->listenersOptions[i.key()]);
                } else {
                    q.finish();
                    q.clear();
                }
            }
            this->lock.unlock();
        }
    }

public:
    void queueStop()
    {
        QMutexLocker<QMutex> locker(&this->lock);
        auto v = this->listenersQSqlDatabase;
        this->listenersQSqlDatabase.clear();
        Q_V_DATABASE_ITERATOR(v)
        {
            const auto connectionName = i.key().toUtf8();
            this->queueRemove(connectionName);
        }
    }

    bool isListening()
    {
        QMutexLocker<QMutex> locker(&this->lock);
        Q_V_DATABASE_ITERATOR(this->listenersQSqlDatabase)
        {
            i.next();
            auto __connection = QSqlDatabase::database(i.key());
            if (__connection.isOpen())
                return true;
        }
        return false;
    }

    void service(const QString &requestPath, QSqlDriver::NotificationSource, const QVariant &payload)
    {
        auto listen = this->listen();
        auto &request = listen->cacheRequest()->createRequest(payload);
        auto &requestMethod = request.requestMethod();
        const auto time_start = QDateTime::currentDateTime();

        request.setRequestProtocol(QRpc::Types::DataBase);
        request.setRequestPath(requestPath.toUtf8());
        auto vMap = request.toHash();
        emit this->listen()->rpcRequest(vMap, QVariant());
        request.start();

        const auto mSecsSinceEpoch = double(QDateTime::currentDateTime().toMSecsSinceEpoch()
                                            - time_start.toMSecsSinceEpoch())
                                     / 1000.000;
        const auto mSecs = QString::number(mSecsSinceEpoch, 'f', 3);
        const auto responseCode = request.responseCode();
        const auto responsePhrase = QString::fromUtf8(request.responsePhrase());

        QByteArray body;

        static const auto staticUrlNames = QVector<int>()
                                           << QMetaType::QUrl << QMetaType::QVariantMap
                                           << QMetaType::QString << QMetaType::QByteArray
                                           << QMetaType::QChar << QMetaType::QBitArray;
        const auto &responseBody = request.responseBody();
        Url rpc_url;
        if (!staticUrlNames.contains(responseBody.typeId()))
            body = request.responseBodyBytes();
        else if (!rpc_url.read(responseBody).isValid())
            body = request.responseBodyBytes();
        else if (rpc_url.isLocalFile()) {
            QFile file(rpc_url.toLocalFile());
            if (!file.open(file.ReadOnly)) {
                request.co().setNotFound();
            } else {
                body = file.readAll();
                file.close();
            }
        }

        auto sqlDriver = dynamic_cast<QSqlDriver *>(QObject::sender());
        if (sqlDriver != nullptr) {
            auto v = request.toHash();
            emit this->listen()->rpcResponseClient(sqlDriver, requestPath, v);
        }

#if Q_RPC_LOG
        if (request.co().isOK()) {
            auto msgOut = QStringLiteral("%1%2:req:%3, ret: %4, %5 ms ")
                              .arg(QT_STRINGIFY2(QRpc::Server), requestMethod, requestPath)
                              .arg(responseCode)
                              .arg(mSecs);
            cInfo() << msgOut;
        } else {
            auto msgOut = QStringLiteral("%1%2:req:%3, ret: %4, %5 ms ")
                              .arg(QT_STRINGIFY2(QRpc::Server), requestMethod, requestPath)
                              .arg(responseCode)
                              .arg(mSecs);
            cWarning() << msgOut;
            msgOut = QStringLiteral("ret=") + responsePhrase;
            cWarning() << msgOut;
            {
                auto &requestHeader = request.requestHeader();
                {
                    QHashIterator<QString, QVariant> i(requestHeader);
                    while (i.hasNext()) {
                        i.next();
                        cWarning() << QStringLiteral("   header - %1 : %2").arg(i.key(), i.value().toString());
                    }
                }
#if Q_RPC_LOG_SUPER_VERBOSE
                {
                    auto &requestParameter = request.requestParameter();
                    QHashIterator<QByteArray, QVariant> i(requestParameter);
                    while (i.hasNext()) {
                        i.next();
                        cWarning() << QByteArrayLiteral("   parameter - ") + i.key() + QByteArrayLiteral(":") + i.value();
                    }
                }
#endif
#if Q_RPC_LOG_SUPER_VERBOSE
                cWarning() << QStringLiteral("       method:%1").arg(requestMethod);
                cWarning() << QByteArrayLiteral("       body:") + requestBody;
#endif
            }
        }
#endif
    }

public slots:

    void onRpcResponse(QUuid uuid, const QVariantHash &vRequest)
    {
        auto &request = this->listen()->cacheRequest()->toRequest(uuid);
        if (!request.isValid())
            return;
        if (!request.fromResponseMap(vRequest))
            request.co().setInternalServerError();
        emit request.finish();
    }
    void onRPCResponseClient(QSqlDriver *sqlDriver,
                             const QString &requestPath,
                             const QVariantHash &responseBody)
    {
        auto connectionName = this->listenersQSqlDrivers.key(sqlDriver);
        auto connection = this->listenersQSqlDatabase.value(connectionName);
        if (!connection.isValid())
            return;
        connectionName = connectionName + QStringLiteral("_response");
        connection = QSqlDatabase::cloneDatabase(connection.connectionName(), connectionName);
        if (!connection.isOpen()) {
            connection.open();
        }
        if (!connection.isOpen())
            return;
        auto responseBytes = QJsonDocument::fromVariant(responseBody).toJson();
        QString command;
        auto dbDriverType = sqlDriver->dbmsType();
        if (dbDriverType == QSqlDriver::PostgreSQL) {
            command = QString("select pg_notify('%1', '%2');").arg(requestPath, responseBytes);
        }
        if (!command.isEmpty()) {
            auto q = connection.exec(command);
            q.finish();
            q.clear();
        }
        connection.close();
    }
};

class ListenBrokerDataBasePvt : public QObject
{
public:
    DataBaseListenerServer *listenServer = nullptr;
    ListenBrokerDataBase *parent = nullptr;

    explicit ListenBrokerDataBasePvt(ListenBrokerDataBase *parent) : QObject{parent}
    {
        this->parent = parent;
    }

    virtual ~ListenBrokerDataBasePvt() { this->stop(); }

    bool start()
    {
        auto &p = *this;
        p.listenServer = new DataBaseListenerServer(this->parent);
        QObject::connect(this->parent,
                         &ListenBrokerDataBase::rpcResponse,
                         p.listenServer,
                         &DataBaseListenerServer::onRpcResponse);
        QObject::connect(this->parent,
                         &ListenBrokerDataBase::rpcCheck,
                         p.listenServer,
                         &DataBaseListenerServer::queueCheck);
        QObject::connect(this->parent,
                         &ListenBrokerDataBase::rpcResponseClient,
                         p.listenServer,
                         &DataBaseListenerServer::onRPCResponseClient);

        QObject::connect(p.listenServer, &DataBaseListenerServer::destroyed, [&p]() {
            p.listenServer = nullptr;
        });

        return p.listenServer->queueStart();
    }

    bool stop()
    {
        auto &p = *this;
        if (p.listenServer != nullptr) {
            p.listenServer->queueStop();
            QObject::disconnect(this->parent,
                                &ListenBrokerDataBase::rpcResponse,
                                p.listenServer,
                                &DataBaseListenerServer::onRpcResponse);
            QObject::disconnect(this->parent,
                                &ListenBrokerDataBase::rpcCheck,
                                p.listenServer,
                                &DataBaseListenerServer::queueCheck);
            QObject::disconnect(this->parent,
                                &ListenBrokerDataBase::rpcResponseClient,
                                p.listenServer,
                                &DataBaseListenerServer::onRPCResponseClient);
            delete p.listenServer;
        }
        p.listenServer = nullptr;
        return true;
    }
};

ListenBrokerDataBase::ListenBrokerDataBase(QObject *parent) : Listen{parent}
{
    this->p = new ListenBrokerDataBasePvt{this};
}

bool ListenBrokerDataBase::start()
{
    Listen::start();
    return p->start();
}

bool ListenBrokerDataBase::stop()
{
    Listen::stop();
    return p->stop();
}

} // namespace QRpc
