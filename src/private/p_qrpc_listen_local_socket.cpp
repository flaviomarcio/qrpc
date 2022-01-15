#include "./p_qrpc_listen_local_socket.h"

#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include "../qrpc_macro.h"
#include "../qrpc_server.h"
#include "../qrpc_listen.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_listen_protocol.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_cache.h"


namespace QRpc {

class LocalSocketServer:public QObject
{

public:
    QHash<QString,QLocalServer*> servers;

    QList<QLocalServer*> clients;
    QMap<QUuid, QLocalSocket*> clientsMap;

    QRPCListenLocalSocket&listen()
    {
        auto _listen=dynamic_cast<QRPCListenLocalSocket*>(this->parent());
        return*_listen;
    }

    explicit LocalSocketServer(QObject*parent=nullptr):QObject(parent)
    {
    }

    bool start()
    {
        auto&protocol=this->listen().colletions()->protocol(QRPCProtocol::WebSocket);

        this->stop();

        if(!protocol.enabled())
            return false;

        bool __return=false;
        for(auto&sport:protocol.port()){
            auto port=sport.toString().trimmed();
            if(port.isEmpty())
                continue;
            auto server = new QLocalServer(this);
            //server->setSslConfiguration(sslConfiguration);

            connect(server, &QLocalServer::newConnection,this, &LocalSocketServer::onServerNewConnection);

            if (!server->listen(port)) {
                sWarning()<<tr("LocalSocketServer: Cannot bind on port %1: %2").arg(port, server->errorString());
                server->close();
                server->deleteLater();
                continue;
            }

            if(!server->isListening()){
                sWarning()<<tr("LocalSocketServer: Cannot bind on port %1: %2").arg(port, server->errorString());
                server->close();
                server->deleteLater();
                continue;
            }

            __return=true;
            sDebug()<<QString("LocalSocketServer: Listening on port %1").arg(port);
            this->servers.insert(port, server);
        }

        if(!__return)
            this->stop();

        return __return;

    }

    bool stop()
    {
        auto aux=this->clientsMap.values();
        this->clientsMap.clear();
        for(auto&client:aux){
            client->close();
        }

        for(auto&server:this->servers){
            if(server!=nullptr){
                server->disconnect();
                delete server;
                server=nullptr;
            }
        }
        return true;
    };

    void onRpcFinish(QRPCListenRequest&request)
    {
        if(!request.isValid())
            return;

        auto socket = this->clientsMap.value(request.requestUuid());
        if(socket==nullptr)
            return;

        socket->write(request.toJson());
        socket->flush();
        //socket->close();//NO CLOSE
        emit request.finish();
    }

    void onRpcRequest(QRPCListenRequest&request)
    {
        if(!request.isValid()){
            request.co().setBadRequest();
            this->onRpcFinish(request);
            return;
        }
        emit this->listen().rpcRequest(request.toHash(), QVariant());
    }

public slots:

    void onRpcResponse(QUuid uuid, QVariantHash vRequest)
    {
        auto&request=this->listen().cacheRequest()->toRequest(uuid);
        if(!request.isValid())
            return;

        if(!request.fromResponseMap(vRequest))
            request.co().setInternalServerError();
        onRpcFinish(request);
    }

    void onServerNewConnection()
    {
        auto server=dynamic_cast<QLocalServer*>(QObject::sender());
        if(server==nullptr)
            return;

        auto socket = server->nextPendingConnection();
        connect(socket, &QLocalSocket::disconnected, this, &LocalSocketServer::onClientDisconnected);
        if(socket==nullptr)
            return;
        QByteArray bytes;
        while(socket->waitForBytesWritten())
            bytes+=socket->readAll();
        auto&request=this->listen().cacheRequest()->createRequest(bytes);
        this->clientsMap.insert(request.requestUuid(), socket);
        this->onRpcRequest(request);
    }

    void onBodyText(QString bytes)
    {
        auto socket = qobject_cast<QLocalSocket*>(QObject::sender());
        if (socket==nullptr)
            return;
        auto&request=this->listen().cacheRequest()->createRequest(bytes);
        this->clientsMap.insert(request.requestUuid(), socket);
        this->onRpcRequest(request);
    }


    void onClientDisconnected()
    {
        auto socket = qobject_cast<QLocalSocket*>(QObject::sender());
        if (socket==nullptr)
            return;
        auto key=this->clientsMap.key(socket);
        if(!key.isNull())
            this->clientsMap.remove(key);
        socket->deleteLater();
    }

};



#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenLocalSocketPvt*>(this->p)

class QRPCListenLocalSocketPvt:public QObject{
public:
    LocalSocketServer*_listenServer=nullptr;

    explicit QRPCListenLocalSocketPvt(QRPCListenLocalSocket*parent):QObject(parent)
    {
        this->_listenServer = new LocalSocketServer(parent);
    }
    virtual ~QRPCListenLocalSocketPvt()
    {
        this->_listenServer->stop();
        delete this->_listenServer;
        this->_listenServer=nullptr;
    }
};

QRPCListenLocalSocket::QRPCListenLocalSocket(QObject *parent):QRPCListen(parent)
{
    this->p = new QRPCListenLocalSocketPvt(this);
}

QRPCListenLocalSocket::~QRPCListenLocalSocket()
{
    dPvt();
    p._listenServer->stop();
    delete&p;
}

bool QRPCListenLocalSocket::start()
{
    dPvt();
    this->stop();
    QRPCListen::start();
    p._listenServer = new LocalSocketServer(this);
    connect(this, &QRPCListen::rpcResponse, p._listenServer, &LocalSocketServer::onRpcResponse);
    return p._listenServer->start();
}

bool QRPCListenLocalSocket::stop()
{
    dPvt();
    return p._listenServer->stop();
}

}

