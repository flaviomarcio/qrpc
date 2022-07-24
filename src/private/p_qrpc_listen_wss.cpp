#include "./p_qrpc_listen_wss.h"

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
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

namespace QRpc {

//!
//! \brief The WebSocketServer class
//!
class WebSocketServer : public QObject
{
    //Q_OBJECT
public:
    QHash<int, QWebSocketServer *> servers;

    QList<QWebSocket *> clients;
    QMap<QUuid, QWebSocket *> clientsMap;

    //!
    //! \brief listen
    //! \return
    //!
    ListenWebSocket &listen()
    {
        auto _listen = dynamic_cast<ListenWebSocket *>(this->parent());
        return *_listen;
    }

    explicit WebSocketServer(QObject *parent = nullptr) : QObject{parent} {}

    bool start()
    {
        auto &protocol = this->listen().colletions()->protocol(Protocol::WebSocket);

        this->stop();

        if (!protocol.enabled())
            return false;

        bool __return = false;
        for (auto &sport : protocol.port()) {
            auto port = sport.toInt();
            if (port <= 0)
                continue;

            QFile certFile(QStringLiteral(":/sslconfiguration/rpc.cert"));
            QFile keyFile(QStringLiteral(":/sslconfiguration/rpc.key"));

            if (!certFile.open(QIODevice::ReadOnly)) {
                rWarning() << tr("LocalServerListener: Cannot load certfile : %1")
                                  .arg(certFile.fileName());
                continue;
            }

            if (!keyFile.open(QIODevice::ReadOnly)) {
                rWarning() << tr("LocalServerListener: Cannot load keyfile : %s")
                                  .arg(keyFile.fileName());
                continue;
            }

            QSslCertificate certificate(&certFile, QSsl::Pem);
            QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
            certFile.close();
            keyFile.close();
            QSslConfiguration sslConfiguration;
            sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
            sslConfiguration.setLocalCertificate(certificate);
            sslConfiguration.setPrivateKey(sslKey);

            auto server = new QWebSocketServer(QStringLiteral("SSL QRpcServer"),
                                               QWebSocketServer::NonSecureMode,
                                               this);

            QObject::connect(server,
                             &QWebSocketServer::newConnection,
                             this,
                             &WebSocketServer::onServerNewConnection);
            QObject::connect(server,
                             &QWebSocketServer::closed,
                             this,
                             &WebSocketServer::onServerClosed);

            if (!server->listen(QHostAddress(QHostAddress::LocalHost), port)) {
                rWarning() << tr("LocalServerListener: Cannot bind on port %1: %2")
                                  .arg(port)
                                  .arg(server->errorString());
                server->close();
                server->deleteLater();
                continue;
            }

            if (!server->isListening()) {
                rWarning() << tr("LocalServerListener: Cannot bind on port %1: %2")
                                  .arg(port)
                                  .arg(server->errorString());
                server->close();
                server->deleteLater();
                continue;
            }

            __return = true;
            sDebug() << QString("LocalServerListener: Listening on port %1").arg(port);
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
            if (server == nullptr)
                continue;
            server->disconnect();
            delete server;
            server = nullptr;
        }
        return true;
    };

    void onRpcFinish(ListenRequest &request)
    {
        if (request.isValid())
            return;

        auto socket = this->clientsMap.value(request.requestUuid());
        if (socket == nullptr)
            return;

        socket->sendBinaryMessage(request.toJson());
        socket->flush();
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
        auto server = dynamic_cast<QWebSocketServer *>(QObject::sender());
        if (server == nullptr)
            return;
        auto socket = server->nextPendingConnection();
        QObject::connect(socket,
                         &QWebSocket::disconnected,
                         this,
                         &WebSocketServer::onClientDisconnected);
        QObject::connect(socket,
                         &QWebSocket::binaryMessageReceived,
                         this,
                         &WebSocketServer::onBodyBinary);
        QObject::connect(socket,
                         &QWebSocket::textMessageReceived,
                         this,
                         &WebSocketServer::onBodyText);
    }

    void onServerClosed() {}

    void onBodyText(QString bytes)
    {
        auto socket = qobject_cast<QWebSocket *>(QObject::sender());
        if (socket == nullptr)
            return;
        auto &request = this->listen().cacheRequest()->createRequest(bytes);
        this->clientsMap.insert(request.requestUuid(), socket);
        this->onRpcRequest(request);
    }

    void onBodyBinary(QByteArray bytes)
    {
        auto socket = qobject_cast<QWebSocket *>(QObject::sender());
        if (socket == nullptr)
            return;
        auto &request = this->listen().cacheRequest()->createRequest(bytes);
        this->clientsMap.insert(request.requestUuid(), socket);
        this->onRpcRequest(request);
    }

    void onClientDisconnected()
    {
        auto socket = qobject_cast<QWebSocket *>(QObject::sender());
        if (socket == nullptr)
            return;
        auto key = this->clientsMap.key(socket);
        if (!key.isNull())
            this->clientsMap.remove(key);
        socket->deleteLater();
    }
};

class ListenWebSocketPvt : public QObject
{
public:
    WebSocketServer *listenServer = nullptr;

    explicit ListenWebSocketPvt(ListenWebSocket *parent) : QObject{parent}
    {
        this->listenServer = new WebSocketServer{parent};
    }

    virtual ~ListenWebSocketPvt()
    {
        this->listenServer->stop();
        delete this->listenServer;
        this->listenServer = nullptr;
    }
};

ListenWebSocket::ListenWebSocket(QObject *parent) : Listen{parent}
{
    this->p = new ListenWebSocketPvt{this};
}

bool ListenWebSocket::start()
{
    this->stop();
    Listen::start();
    p->listenServer = new WebSocketServer{this};
    connect(this, &Listen::rpcResponse, p->listenServer, &WebSocketServer::onRpcResponse);
    return p->listenServer->start();
}

bool ListenWebSocket::stop()
{
    return p->listenServer->stop();
}

} // namespace QRpc
