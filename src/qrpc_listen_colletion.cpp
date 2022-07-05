#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen.h"
#include "./qrpc_listen_protocol.h"
#include "./qrpc_server.h"
#include <QDir>
#include <QMutex>
#include <QSettings>

#include "./private/p_qrpc_server.h"
//#include "./private/p_qrpc_listen_tcp->h"
//#include "./private/p_qrpc_listen_udp->h"
//#include "./private/p_qrpc_listen_wss.h"
//#include "./private/p_qrpc_listen_broker_mqtt.h"
//#include "./private/p_qrpc_listen_broker_amqp->h"
//#include "./private/p_qrpc_listen_broker_kafka.h"
//#include "./private/p_qrpc_listen_broker_database.h"
//#include "./private/p_qrpc_listen_http->h"
//#include "./private/p_qrpc_listen_qrpc.h"

namespace QRpc {



class ListenColletionsPvt : public QObject
{
public:
    QMutex lockMake;
    QMutex lockWaitRun;
    QMutex lockWaitQuit;
    QMutex lockRunning;
    QHash<int, Listen *> listensActive;
    ListenProtocols listenProtocol;
    Server *server = nullptr;
    QVariantHash settings;

    ListenColletions *collections()
    {
        auto collections = dynamic_cast<ListenColletions *>(this->parent());
        return collections;
    }

    explicit ListenColletionsPvt(Server *server,
                                 const QVariantHash &settings,
                                 ListenColletions *parent)
        : QObject(parent)
    {
        this->server = server;
        this->settings = settings;
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

        auto settingsDefault = this->settings.value(qsl("default")).toHash();
        for (auto &v : this->listenProtocol) {
            auto optionName = v->optionName();
            auto settings = this->settings.value(optionName).toHash();
            if (!settings.isEmpty())
                v->setSettings(settings, settingsDefault);
        }

        auto list = this->listenProtocol.value(0);
        if (list != nullptr)
            list->setEnabled(true);
    }

    void makeListens()
    {
        auto &vList = Listen::listenList();
        for (auto &item : vList)
            this->makeOption(item.first, *item.second);
        this->loadSettings();
    }

    bool makeOption(int protocol, const QMetaObject &metaObject)
    {
        QMutexLOCKER locker(&this->lockMake);
        if (this->listenProtocol.contains(protocol))
            return true;

        auto option = new ListenProtocol(protocol, metaObject, this->parent());
        option->setObjectName(qsl("set_%1").arg(QString::fromUtf8(option->protocolName())));
        this->listenProtocol.insert(option->protocol(), option);
        return true;
    }

    void listenClear()
    {
        if (this->listensActive.isEmpty())
            return;

        auto aux = this->listensActive.values();
        this->listensActive.clear();
        for (auto &listen : aux) {
            if (!listen->isRunning())
                continue;

            listen->quit();
            listen->wait();
            listen->deleteLater();
        }
    }

    void listenStart()
    {
        this->listenClear();
        QVector<Listen*> listenStartOrder;
        for (auto &protocol : listenProtocol) {
            if (!protocol->enabled())
                continue;

            auto listen = protocol->makeListen();
            if (listen == nullptr)
                continue;

            listen->setServer(this->server);
            listen->setColletions(this->collections());
            this->listensActive.insert(protocol->protocol(), listen);
            listenStartOrder<<listen;
        }

        auto listenPool = this->collections()->listenPool();
        if (listenPool == nullptr) {
            qFatal("invalid pool");
        }

        for (auto &listen : this->listensActive) {
            listenPool->registerListen(listen);
            listen->setServer(this->server);
            listen->setColletions(this->collections());
        }

        for (auto &listen : listenStartOrder)
            listen->start();

        this->lockWaitRun.tryLock(10);
        this->lockWaitRun.unlock();
    }

    void listenQuit()
    {
        this->listenClear();
        this->lockRunning.tryLock(1);
        this->lockRunning.unlock();
    }
};

ListenColletions::ListenColletions(Server *server) : QThread{nullptr}
{
    this->p = new ListenColletionsPvt(server, {}, this);
}

ListenColletions::ListenColletions(const QVariantHash &settings, Server *server)
{
    this->p = new ListenColletionsPvt(server, settings, this);
}


ListenProtocol &ListenColletions::protocol()
{
    return this->protocol(Protocol::Http);
}

ListenProtocol &ListenColletions::protocol(const Protocol &protocol)
{
    static ListenProtocol staticDefaultProtocol;

    if (protocol < rpcProtocolMin)
        return staticDefaultProtocol;

    if(protocol > rpcProtocolMax)
        return staticDefaultProtocol;


    auto &listenProtocol = p->listenProtocol;
    auto ___return = listenProtocol.value(protocol);
    if (___return != nullptr)
        return *___return;

    if (protocol != Http && protocol != Https)
        return *___return;

    auto __protocolType = (protocol == Http) ? Https : protocol;
    ___return = listenProtocol.value(__protocolType);
    if (___return == nullptr)
        return staticDefaultProtocol;

    return *___return;
}

ListenProtocols &ListenColletions::protocols()
{

    return p->listenProtocol;
}

void ListenColletions::run()
{

    p->listenStart();
    this->exec();
    p->listenQuit();
    p->lockWaitQuit.tryLock(1);
    p->lockWaitQuit.unlock();
}

void ListenColletions::requestEnabled()
{
    //criar mutex de controle nos listens
}

void ListenColletions::requestDisable()
{
    //criar mutex de controle nos listens
}

Server *ListenColletions::server()
{

    return p->server;
}

void ListenColletions::setSettings(const QVariantHash &settings) const
{

    return p->setSettings(settings);
}

ListenQRPC *ListenColletions::listenPool()
{

    QHashIterator<int, Listen *> i(p->listensActive);
    while (i.hasNext()) {
        i.next();
        if (i.value() == nullptr)
            continue;

        auto listen = dynamic_cast<ListenQRPC *>(i.value());
        if (listen != nullptr)
            return listen;
    }
    return nullptr;
}

bool ListenColletions::start()
{

    bool __return = false;
    if (p->lockRunning.tryLock(1000)) {
        p->lockWaitRun.lock();
        QThread::start();
        QMutexLOCKER locker(&p->lockWaitRun);
        __return = this->isRunning();
    }
    return __return;
}

bool ListenColletions::stop()
{
    return this->quit();
}

bool ListenColletions::quit()
{

    p->lockWaitQuit.lock();
    QMutexLOCKER lockerRun(
        &p->lockWaitRun); //evitar crash antes da inicializacao de todos os listainers
    p->listenQuit();
    QThread::quit();
    QMutexLOCKER lockerQuit(&p->lockWaitQuit);
    QThread::wait();
    return true;
}

} // namespace QRpc
