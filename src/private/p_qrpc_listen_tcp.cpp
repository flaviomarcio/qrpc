#include "./p_qrpc_listen_tcp.h"
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
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QTcpServer>

namespace QRpc {

class ServerTCPSocket : public QObject
{
public:
    QByteArray buffer;
    QHash<int, QTcpServer *> servers;
    QMap<QUuid, QTcpSocket *> clientsMap;

    Listen &listen()
    {
        auto _listen = dynamic_cast<Listen *>(this->parent());
        return *_listen;
    }

    explicit ServerTCPSocket(QObject *parent) : QObject{parent} {}

    bool start()
    {
        auto &protocol = this->listen().colletions()->protocol(Protocol::TcpSocket);
        this->stop();

        if (!protocol.enabled())
            return false;

        bool __return = false;
        for (auto &sport : protocol.port()) {
            auto port = sport.toInt();
            if (port <= 0)
                continue;

            auto server = new QTcpServer(this);
            connect(server,
                    &QTcpServer::newConnection,
                    this,
                    &ServerTCPSocket::onServerNewConnection);
            if (!server->listen(QHostAddress::LocalHost, port)) {
                rWarning() << tr("WebsocketListener: Cannot bind on port %1: %2")
                                  .arg(port)
                                  .arg(server->errorString());
                server->close();
                server->deleteLater();
                continue;
            }

            if (server->isListening()) {
                __return = true;
                sDebug() << QString("TcpsocketListener: Listening on port %i").arg(port);
                this->servers.insert(port, server);
                continue;
            }

            rWarning() << tr("TcpsocketListener: Cannot bind on port %1: %2")
                              .arg(port)
                              .arg(server->errorString());
            server->close();
            server->deleteLater();
            continue;
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
            if (server == nullptr)
                continue;
            server->disconnect();
            delete server;
            server = nullptr;
        }
        return true;
    }

public slots:

    void onRpcFinish(QRpc::ListenRequest *request)
    {
        Q_UNUSED(request)
        if (!request->isValid())
            return;
        auto socket = this->clientsMap.value(request->requestUuid());
        if (socket != nullptr) {
            socket->write(request->toJson());
            socket->flush();
            QThread::msleep(5);
            socket->close();
        }
        emit request->finish();
    }

    void onRpcResponse(const QUuid &uuid, const QVariantHash &vRequest)
    {
        auto &request = this->listen().cacheRequest()->toRequest(uuid);
        if (!request.isValid())
            return;
        if (!request.fromResponseMap(vRequest))
            request.co().setInternalServerError();
        this->onRpcFinish(&request);
    }

    void onRpcRequest(QRpc::ListenRequest *request)
    {
        Q_UNUSED(request)
        if (!request->isValid()) {
            request->co().setBadRequest();
            this->onRpcFinish(request);
            return;
        }
        emit this->listen().rpcRequest(request->toHash(), QVariant());
    }

    void onServerNewConnection()
    {
        auto server = dynamic_cast<QTcpServer *>(QObject::sender());
        if (server == nullptr)
            return;
        auto socket = server->nextPendingConnection();
        connect(socket, &QTcpSocket::disconnected, this, &ServerTCPSocket::deleteLater);
        connect(socket, &QTcpSocket::disconnected, this, &ServerTCPSocket::onClientDisconnected);
        while (socket->waitForReadyRead()) {
            this->buffer.append(socket->readAll());
            ListenRequest request;
            if (request.fromJson(this->buffer))
                break;
        }
        auto &request = this->listen().cacheRequest()->createRequest(this->buffer);
        this->clientsMap.insert(request.requestUuid(), socket);
        this->onRpcRequest(&request);
    }

    void onClientDisconnected()
    {
        auto socket = qobject_cast<QTcpSocket *>(QObject::sender());
        if (socket == nullptr)
            return;
        auto key = this->clientsMap.key(socket);
        if (!key.isNull())
            this->clientsMap.remove(key);
        socket->deleteLater();
    }
};

#define dPvt() auto &p = *reinterpret_cast<ListenTCPPvt *>(this->p)

class ListenTCPPvt : public QObject
{
public:
    ServerTCPSocket *_listenServer = nullptr;

    explicit ListenTCPPvt(ListenTCP *parent) : QObject{parent}
    {
        this->_listenServer = new ServerTCPSocket(parent);
    }

    virtual ~ListenTCPPvt()
    {
        this->_listenServer->stop();
        delete this->_listenServer;
        this->_listenServer = nullptr;
    }
};

ListenTCP::ListenTCP(QObject *parent) : Listen{parent}
{
    this->p = new ListenTCPPvt{this};
}

ListenTCP::~ListenTCP()
{
    dPvt();
    p._listenServer->stop();
    delete &p;
}

bool ListenTCP::start()
{
    dPvt();
    connect(this, &Listen::rpcResponse, p._listenServer, &ServerTCPSocket::onRpcResponse);
    return p._listenServer->start();
}

bool ListenTCP::stop()
{
    dPvt();
    return p._listenServer->stop();
}

} // namespace QRpc
