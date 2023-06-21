#include "./qrpc_listen.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_request_cache.h"
#include "./qrpc_const.h"
#if Q_RPC_LOG
#include "./qrpc_macro.h"
#endif
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QProcess>

namespace QRpc {

typedef QHash<int, QPair<int, const QMetaObject *>> MetaObjectVectorHash;
typedef QVector<ListenMetaObject> ListenMetaObjectList;

Q_GLOBAL_STATIC(MetaObjectVectorHash, staticListenInstalledHash);
Q_GLOBAL_STATIC(ListenMetaObjectList, staticListenInstalledList);

class ListenPvt : public QObject
{
public:
    Listen *listen = nullptr;
    Listen *listenPool = nullptr;
    ListenRequestCache cacheRequest;
    ListenColletions *collections = nullptr;
    Server *server = nullptr;
    QUuid uuid;
    explicit ListenPvt(Listen *parent) : QObject{parent}, listen{parent}, cacheRequest(parent), uuid(QUuid::createUuid())
    {
    }

};

Listen::Listen(QObject *parent) : QThread{nullptr}, p{new ListenPvt{this}}
{
    Q_UNUSED(parent)
}


int Listen::install(const QVariant &type, const QMetaObject &metaObject)
{
    const auto itype = type.toInt();
    if (!staticListenInstalledHash->contains(itype)) {
#if Q_RPC_LOG_VERBOSE
        if (staticListenInstalledHash->isEmpty())
            rInfo() << QStringLiteral("interface registered: ") << metaObject.className();
        qInfo() << QByteArrayLiteral("interface: ") + metaObject.className();
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

QUuid Listen::uuid() const
{
    return p->uuid;
}

void Listen::run()
{
    this->exec();
}

bool Listen::start()
{
    QThread::start();
    while (!this->eventDispatcher())
        QThread::msleep(1);
    return true;
}

bool Listen::stop()
{
    if (!this->isRunning())
        return true;
    QThread::quit();
    while (!this->eventDispatcher())
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
