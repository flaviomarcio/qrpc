#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen.h"
#include "./qrpc_listen_protocol.h"
#include "./private/p_qrpc_listen_qrpc.h"
#include "./qrpc_server.h"
#include <QDir>
#include <QMutex>
#include <QSettings>

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
        : QObject{parent}
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

        auto settingsDefault = this->settings.value(QStringLiteral("default")).toHash();
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
        QMutexLocker<QMutex> locker(&this->lockMake);
        if (this->listenProtocol.contains(protocol))
            return true;

        auto option = new ListenProtocol(protocol, metaObject, this->parent());
        option->setObjectName(QStringLiteral("set_%1").arg(QString::fromUtf8(option->protocolName())));
        this->listenProtocol.insert(option->protocol(), option);
        return true;
    }

    void listenStop()
    {
        if (this->listensActive.isEmpty())
            return;

        QHashIterator<int, Listen *> i(this->listensActive);
        while(i.hasNext()){
            i.next();
            auto listen=i.value();
            listen->quit();
            listen->wait();
        }
        auto aux=this->listensActive.values();
        this->listensActive.clear();
        qDeleteAll(aux);
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
            listen->setColletions(this->collections());
            this->listensActive.insert(protocol->protocol(), listen);
            listenStartOrder.append(listen);
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
        this->listenStop();
        this->lockRunning.tryLock(1);
        this->lockRunning.unlock();
    }
};

ListenColletions::ListenColletions(Server *server) : QThread{nullptr}
{
    this->p = new ListenColletionsPvt(server, {}, this);
    this->moveToThread(this);
}

ListenColletions::ListenColletions(const QVariantHash &settings, Server *server)
{
    this->p = new ListenColletionsPvt(server, settings, this);
    this->moveToThread(this);
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

    if (protocol != Http)
        return *___return;

    ___return = listenProtocol.value(protocol);
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
    if (p->lockRunning.tryLock(1000)) {
        p->lockWaitRun.lock();
        QThread::start();
        QMutexLocker<QMutex> locker(&p->lockWaitRun);
        if(this->isRunning())
            return true;
    }
    return false;
}

bool ListenColletions::stop()
{
    return this->quit();
}

bool ListenColletions::quit()
{
    p->lockWaitQuit.lock();
    QMutexLocker<QMutex> lockerRun(
        &p->lockWaitRun); //evitar crash antes da inicializacao de todos os listainers
    p->listenQuit();
    QThread::quit();
    QMutexLocker<QMutex> lockerQuit(&p->lockWaitQuit);
    QThread::wait();
    return true;
}

} // namespace QRpc
