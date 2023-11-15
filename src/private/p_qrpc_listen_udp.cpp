#include "./p_qrpc_listen_udp.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_listen_protocol.h"
#include <QNetworkDatagram>
#include <QUdpSocket>

namespace QRpc {

class ServerUDPSocket : public QObject
{
public:
    QHash<int, QUdpSocket *> servers;
    /**
     * @brief listen
     * @return
     */
    Listen &listen()
    {
        auto _listen = dynamic_cast<Listen *>(this->parent());
        return *_listen;
    }

    explicit ServerUDPSocket(QObject *parent) : QObject{parent} {}

    bool start()
    {
        auto &protocol = this->listen().colletions()->protocol(Protocol::UdpSocket);
        this->stop();

        if (!protocol.enabled())
            return false;

        bool __return = true;
        for (auto &sport : protocol.port()) {
            auto port = sport.toInt();
            if (port <= 0)
                continue;

            auto server = new QUdpSocket(this);
            connect(server, &QUdpSocket::readyRead, this, &ServerUDPSocket::onClientConnected);
            if (server->bind(QHostAddress::LocalHost, port))
                continue;
            __return = false;
            break;
        }

        if (!__return)
            this->stop();

        return __return;
    }

    bool stop()
    {
        for (auto &server : this->servers) {
            if (server == nullptr)
                continue;
            server->disconnect();
            delete server;
            server = nullptr;
        }
        return true;
    };

public slots:

    void sendRequest(const QByteArray &body)
    {
        QVariantHash request;
        request.insert("protocol", "udp");
        request.insert("body", body);
        emit this->listen().rpcRequest(request, QVariant());
    }

    void onClientConnected()
    {
        QByteArray body;
        auto server = dynamic_cast<QUdpSocket *>(QObject::sender());
        if (server == nullptr)
            return;

        while (server->hasPendingDatagrams()) {
            auto datagram = server->receiveDatagram();
            body.append(datagram.data());
        }
        this->sendRequest(body);
        server->close();
    }

    void onClientDisconnected()
    {
        auto socket = qobject_cast<QUdpSocket *>(QObject::sender());
        if (socket != nullptr)
            socket->deleteLater();
    }
};

class ListenUDPPvt : public QObject
{
public:
    ServerUDPSocket *listenServer = nullptr;

    explicit ListenUDPPvt(ListenUDP *parent) : QObject{parent}
    {
        this->listenServer = new ServerUDPSocket{parent};
    }

    virtual ~ListenUDPPvt()
    {
        this->listenServer->stop();
        delete this->listenServer;
    }
};

ListenUDP::ListenUDP(QObject *parent) : Listen{parent}, p{new ListenUDPPvt{this}}
{
}

bool ListenUDP::start()
{
    return p->listenServer->start();
}

bool ListenUDP::stop()
{
    return p->listenServer->stop();
}

} // namespace QRpc
