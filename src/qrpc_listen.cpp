#include "./qrpc_listen.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_request_cache.h"
#include "./qrpc_startup.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QProcess>

namespace QRpc {

typedef QHash<int, QPair<int, const QMetaObject *>> MetaObjectVectorHash;
typedef QVector<ListenMetaObject> ListenMetaObjectList;

Q_GLOBAL_STATIC(MetaObjectVectorHash, staticListenInstalledHash);
Q_GLOBAL_STATIC(ListenMetaObjectList, staticListenInstalledList);
Q_GLOBAL_STATIC(QByteArray, baseUuid)

static void init()
{
    QProcess process;
    auto bytes = process.environment().join(qsl(",")).toUtf8()
                 + QDateTime::currentDateTime().toString().toUtf8();
    *baseUuid = QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
}

Q_RPC_STARTUP_FUNCTION(init);

class ListenPvt : public QObject
{
public:
    QUuid uuid;
    Listen *listenPool = nullptr;
    ListenRequestCache cacheRequest;
    ListenColletions *collections = nullptr;
    Server *server = nullptr;
    explicit ListenPvt(Listen *parent) : QObject{parent}, cacheRequest(parent)
    {
        this->uuid = QUuid::createUuidV5(QUuid::createUuid(), *baseUuid);
    }

    Listen *listen()
    {
        auto listen = dynamic_cast<Listen *>(this->parent());
        return listen;
    }
};

Listen::Listen(QObject *parent) : QThread{nullptr}
{
    Q_UNUSED(parent)
    this->p = new ListenPvt{this};
}


int Listen::install(const QVariant &type, const QMetaObject &metaObject)
{
    const auto itype = type.toInt();
    if (!staticListenInstalledHash->contains(itype)) {
#if Q_RPC_LOG_VERBOSE
        if (staticListenInstalledHash->isEmpty())
            sInfo() << qsl("interface registered: ") << metaObject.className();
        qInfo() << qbl("interface: ") + metaObject.className();
#endif
        QPair<int, const QMetaObject *> pair(itype, &metaObject);
        staticListenInstalledHash->insert(itype, pair);
        staticListenInstalledList->append(pair);
    }
    return staticListenInstalledHash->contains(itype);
}

ListenMetaObjectList &Listen::listenList()
{
    return *staticListenInstalledList;
}

QUuid &Listen::uuid() const
{

    return p->uuid;
}

QObject *Listen::parent()
{
    return QThread::parent();
}

void Listen::setParent(QObject *parent)
{
    QThread::setParent(parent);
}

void Listen::run()
{
    this->exec();
}

bool Listen::start()
{
    QThread::start();
    while (this->eventDispatcher() == nullptr)
        QThread::msleep(1);
    return true;
}

bool Listen::stop()
{
    if (!this->isRunning())
        return true;
    QThread::quit();
    while (this->eventDispatcher() == nullptr)
        QThread::msleep(1);
    if (this->wait(1000))
        return true;
    return false;
}

Server *Listen::server()
{
    return p->server;
}

ListenColletions *Listen::colletions()
{
    return p->collections;
}

ListenRequestCache *Listen::cacheRequest()
{
    return &p->cacheRequest;
}

void Listen::registerListenPool(Listen *listenPool)
{
    p->listenPool = listenPool;
}

Listen &Listen::listenPool()
{
    return *p->listenPool;
}

void Listen::setServer(Server *server)
{
    p->server = server;
}

void Listen::setColletions(ListenColletions *colletions)
{
    p->collections = colletions;
}

} // namespace QRpc
