#include "./p_qrpc_listen_local_socket.h"
#include "../qrpc_listen.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_listen_protocol.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_cache.h"
#include "../qrpc_const.h"
#if Q_RPC_LOG
#include "../qrpc_macro.h"
#endif
#include <QFile>
#include <QLocalServer>
#include <QLocalSocket>

namespace QRpc {

class LocalSocketServer : public QObject
{
public:
    QHash<QString, QLocalServer *> servers;

    QList<QLocalServer *> clients;
    QMap<QUuid, QLocalSocket *> clientsMap;

    ListenLocalSocket &listen()
    {
        auto __return = dynamic_cast<ListenLocalSocket *>(this->parent());
        return *__return;
    }

    explicit LocalSocketServer(QObject *parent = nullptr) : QObject{parent} {}

    bool start()
    {
        auto &protocol = this->listen().colletions()->protocol(Protocol::WebSocket);

        this->stop();

        if (!protocol.enabled())
            return false;

        bool __return = false;
        for (auto &sport : protocol.port()) {
            auto port = sport.toString().trimmed();
            if (port.isEmpty())
                continue;
            auto server = new QLocalServer(this);
            //server->setSslConfiguration(sslConfiguration);

            connect(server,
                    &QLocalServer::newConnection,
                    this,
                    &LocalSocketServer::onServerNewConnection);

            if (!server->listen(port)) {
                rWarning() << tr("LocalSocketServer: Cannot bind on port %1: %2")
                                  .arg(port, server->errorString());
                server->close();
                server->deleteLater();
                continue;
            }

            if (!server->isListening()) {
                rWarning() << tr("LocalSocketServer: Cannot bind on port %1: %2")
                                  .arg(port, server->errorString());
                server->close();
                server->deleteLater();
                continue;
            }

            __return = true;
            sDebug() << QString("LocalSocketServer: Listening on port %1").arg(port);
            this->servers.insert(port, server);
        }

        if (!__return)
            this->stop();

        return __return;
    }

    bool stop()
    {
        auto aux = this->clientsMap.values();
        this->clientsMap.clear();
        for (auto &client : aux) {
            client->close();
        }

        for (auto &server : this->servers) {
            if (server != nullptr) {
                server->disconnect();
                delete server;
                server = nullptr;
            }
        }
        return true;
    };

    void onRpcFinish(ListenRequest &request)
    {
        if (!request.isValid())
            return;

        auto socket = this->clientsMap.value(request.requestUuid());
        if (socket == nullptr)
            return;

        socket->write(request.toJson());
        socket->flush();
        //socket->close();//NO CLOSE
        emit request.finish();
    }

    void onRpcRequest(ListenRequest &request)
    {
        if (!request.isValid()) {
            request.co().setBadRequest();
            this->onRpcFinish(request);
            return;
        }
        emit this->listen().rpcRequest(request.toHash(), QVariant());
    }

public slots:

    void onRpcResponse(QUuid uuid, QVariantHash vRequest)
    {
        auto &request = this->listen().cacheRequest()->toRequest(uuid);
        if (!request.isValid())
            return;

        if (!request.fromResponseMap(vRequest))
            request.co().setInternalServerError();
        onRpcFinish(request);
    }

    void onServerNewConnection()
    {
        auto server = dynamic_cast<QLocalServer *>(QObject::sender());
        if (server == nullptr)
            return;

        auto socket = server->nextPendingConnection();
        connect(socket, &QLocalSocket::disconnected, this, &LocalSocketServer::onClientDisconnected);
        if (socket == nullptr)
            return;
        QByteArray bytes;
        while (socket->waitForBytesWritten())
            bytes += socket->readAll();
        auto &request = this->listen().cacheRequest()->createRequest(bytes);
        this->clientsMap.insert(request.requestUuid(), socket);
        this->onRpcRequest(request);
    }

    void onBodyText(QString bytes)
    {
        auto socket = qobject_cast<QLocalSocket *>(QObject::sender());
        if (socket == nullptr)
            return;
        auto &request = this->listen().cacheRequest()->createRequest(bytes);
        this->clientsMap.insert(request.requestUuid(), socket);
        this->onRpcRequest(request);
    }

    void onClientDisconnected()
    {
        auto socket = qobject_cast<QLocalSocket *>(QObject::sender());
        if (socket == nullptr)
            return;
        auto key = this->clientsMap.key(socket);
        if (!key.isNull())
            this->clientsMap.remove(key);
        socket->deleteLater();
    }
};

class ListenLocalSocketPvt : public QObject
{
public:
    LocalSocketServer *listenServer = nullptr;

    explicit ListenLocalSocketPvt(ListenLocalSocket *parent) : QObject{parent}
    {
        this->listenServer = new LocalSocketServer(parent);
    }
    virtual ~ListenLocalSocketPvt()
    {
        this->listenServer->stop();
        delete this->listenServer;
        this->listenServer = nullptr;
    }
};

ListenLocalSocket::ListenLocalSocket(QObject *parent) : Listen{parent}
{
    this->p = new ListenLocalSocketPvt{this};
}

bool ListenLocalSocket::start()
{
    this->stop();
    Listen::start();
    p->listenServer = new LocalSocketServer(this);
    connect(this, &Listen::rpcResponse, p->listenServer, &LocalSocketServer::onRpcResponse);
    return p->listenServer->start();
}

bool ListenLocalSocket::stop()
{
    return p->listenServer->stop();
}

} // namespace QRpc
