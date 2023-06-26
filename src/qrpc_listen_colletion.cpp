#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen.h"
#include "./qrpc_listen_protocol.h"
#include "./private/p_qrpc_listen_qrpc.h"
#include "./qrpc_server.h"
#include "./qrpc_types.h"
#include "../../qstm/src/qstm_meta_enum.h"

//#include "./private/p_qrpc_listen_tcp->h"
//#include "./private/p_qrpc_listen_udp->h"
//#include "./private/p_qrpc_listen_wss.h"
//#include "./private/p_qrpc_listen_broker_mqtt.h"
//#include "./private/p_qrpc_listen_broker_amqp.h"
//#include "./private/p_qrpc_listen_broker_kafka.h"
//#include "./private/p_qrpc_listen_broker_database.h"
//#include "./private/p_qrpc_listen_http->h"
//#include "./private/p_qrpc_listen_qrpc.h"

namespace QRpc {

static const auto __default="default";

class ListenColletionsPvt : public QObject
{
public:
    bool lockWaitRun=false;
    ListenColletions *collections=nullptr;
    QHash<int, Listen *> listensActive;
    ListenProtocols listenProtocol;
    Server *server = nullptr;
    QVariantHash settings;

    explicit ListenColletionsPvt(Server *server, const QVariantHash &settings, ListenColletions *parent)
        :QObject{parent}, collections{parent}, server{server}, settings{settings}
    {
        this->makeListens();
    }

    void setSettings(const QVariantHash &settings)
    {
        this->settings = settings;
        this->loadSettings();
    }

    void loadSettings()
    {
        if(this->listenProtocol.isEmpty())
            return;

        auto settingsDefault = this->settings.value(__default).toHash();
        for (auto &v : this->listenProtocol) {
            auto optionName = v->optionName();
            auto settings = this->settings.value(optionName).toHash();
            if (!settings.isEmpty())
                v->setSettings(settings, settingsDefault);
        }

        auto list = this->listenProtocol.value(QRpc::Types::Rpc);
        if (list==nullptr)
            qFatal("QRpc::ListenQRPC no initialized");
        list->setEnabled(true);
    }

    void makeListens()
    {
        auto &vList = Listen::listenList();
        for (auto &item : vList){
            auto protocol=QRpc::Types::Protocol(item.first);
            this->makeOption(protocol, *item.second);
        }
        this->loadSettings();
    }

    bool makeOption(QRpc::Types::Protocol protocol, const QMetaObject &metaObject)
    {
        if (this->listenProtocol.contains(protocol))
            return true;

        auto option = new ListenProtocol(protocol, metaObject, this->parent());
        option->setObjectName(QStringLiteral("set_%1").arg(option->optionName()));
        this->listenProtocol.insert(option->protocol(), option);
        return true;
    }

    void listenStop()
    {
        if (this->listensActive.isEmpty())
            return;

        auto aux=this->listensActive;
        this->listensActive.clear();
        QHashIterator<int, Listen *> i(aux);
        while(i.hasNext()){
            i.next();
            auto listen=i.value();
            listen->quit();
            listen->wait();
            listen->deleteLater();
        }
    }

    void listenStart()
    {
        this->listenStop();
        QVector<Listen*> listenStartOrder;

        for (auto &protocol : listenProtocol) {
            if (!protocol->enabled())
                continue;

            auto listen=this->listensActive.value(protocol->protocol());

            if(listen==nullptr)
                listen = protocol->makeListen();

            if (listen == nullptr)
                continue;

            listen->setServer(this->server);
            listen->setColletions(this->collections);
            this->listensActive.insert(protocol->protocol(), listen);
            listenStartOrder.append(listen);
        }

        auto listenPool = this->collections->listenQRPC();
        if (listenPool == nullptr) {
            qFatal("invalid pool");
        }

        for (auto &listen : this->listensActive) {
            listenPool->registerListen(listen);
            listen->setServer(this->server);
            listen->setColletions(this->collections);
        }

        for (auto &listen : listenStartOrder)
            listen->start();

        this->lockWaitRun=true;
    }
};

ListenColletions::ListenColletions(Server *server) : QThread{nullptr}, p{new ListenColletionsPvt(server, {}, this)}
{
    this->moveToThread(this);
}

ListenColletions::ListenColletions(const QVariantHash &settings, Server *server):QThread{nullptr}, p{new ListenColletionsPvt(server, settings, this)}
{
    this->moveToThread(this);
}

ListenProtocol *ListenColletions::protocol()
{
    return this->protocol(QRpc::Types::Protocol::Http);
}

ListenProtocol *ListenColletions::protocol(const QVariant &protocol)
{
    QStm::MetaEnum<QRpc::Types::Protocol> eProtocol=protocol;
    auto &listenProtocol = p->listenProtocol;
    auto ___return = listenProtocol.value(eProtocol.type());
    if (___return)
        return ___return;
    return nullptr;
}

ListenProtocols &ListenColletions::protocols()
{
    return p->listenProtocol;
}

void ListenColletions::run()
{
    p->listenStart();
    this->exec();
    p->listenStop();
}

Server *ListenColletions::server()
{
    return p->server;
}

QVariantHash &ListenColletions::settings()
{
    return p->settings;
}

ListenColletions &ListenColletions::settings(const QVariantHash &settings)
{
    p->setSettings(settings);
    return *this;
}

ListenQRPC *ListenColletions::listenQRPC()
{
    auto listen = p->listensActive.value(QRpc::Types::Rpc);
    return dynamic_cast<ListenQRPC*>(listen);

}

bool ListenColletions::start()
{
    if(this->isRunning()){
        QThread::quit();
        QThread::wait();
    }
    QThread::start();
    while(this->eventDispatcher()!=nullptr)
        this->msleep(1);
    while(!p->lockWaitRun)
        this->msleep(1);
    return this->isRunning();
}

bool ListenColletions::stop()
{
    return this->quit();
}

bool ListenColletions::quit()
{
    p->listenStop();
    QThread::quit();
    QThread::wait();
    return true;
}

} // namespace QRpc
