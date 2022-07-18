#include "./qrpc_listen_request_cache.h"
#include "./qrpc_listen.h"
#include <QJsonDocument>
#include <QMap>
#include <QMutex>
#include <QUuid>

namespace QRpc {

class ListenRequestCachePvt : public QObject
{
public:
    QMutex lock;
    QHash<QString, ListenRequest *> cache;
    Listen *listen()
    {
        auto listen = dynamic_cast<Listen *>(this->parent()->parent());
        return listen;
    }

    explicit ListenRequestCachePvt(QObject *parent) : QObject{parent} {}

    virtual ~ListenRequestCachePvt() { this->clear(); }

    void clear()
    {
        QMutexLocker<QMutex> locker(&this->lock);
        auto aux = this->cache;
        this->cache.clear();
        qDeleteAll(aux);
    }
};

ListenRequestCache::ListenRequestCache(Listen *parent) : QObject{parent}
{
    this->p = new ListenRequestCachePvt{this};
}

void ListenRequestCache::clear()
{

    p->clear();
}

ListenRequest &ListenRequestCache::toRequest(const QUuid &uuid)
{

    QMutexLocker<QMutex> locker(&p->lock);
    static ListenRequest ___ListenRequest;
    auto request = p->cache.value(uuid.toString());
    if (request != nullptr)
        return *request;
    return ___ListenRequest;
}

ListenRequest &ListenRequestCache::createRequest()
{
    QVariant vRequest;
    return this->createRequest(vRequest);
}

ListenRequest &ListenRequestCache::createRequest(const QVariant &vRequest)
{

    QMutexLocker<QMutex> locker(&p->lock);
    auto request = new ListenRequest(vRequest);
    request->setListenUuid(p->listen()->uuid());
    if (request->isEmpty() || !request->isValid())
        request->setRequestBody(vRequest);

    auto uuid = request->requestUuid().isNull() ? request->makeUuid() : request->requestUuid();
    request->setRequestUuid(uuid);
    p->cache.insert(uuid.toString(), request);
    return *request;
}

} // namespace QRpc
