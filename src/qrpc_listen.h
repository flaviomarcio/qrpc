#pragma once

#include "./qrpc_global.h"
#include <QTemporaryFile>
#include <QThread>
#include <QUuid>
#include <QVariantHash>

namespace QRpc {
class Server;
class ListenQRPC;
class ListenColletions;
class ListenRequestCache;
class ListenPvt;
//!
//! \brief The Listen class
//!
class Q_RPC_EXPORT Listen : public QThread
{
    Q_OBJECT
    friend class ListenColletions;
    friend class ListenColletionsPvt;

public:
    //!
    //! \brief Listen
    //! \param parent
    //!
    Q_INVOKABLE explicit Listen(QObject *parent = nullptr);

    //!
    //! \brief install
    //! \param type
    //! \param metaObject
    //! \return
    //!
    static int install(const QVariant &type, const QMetaObject &metaObject);

    //!
    //! \brief listenList
    //! \return
    //!
    static ListenMetaObjectList &listenList();

    //!
    //! \brief uuid
    //! \return
    //!
    virtual QUuid &uuid() const;

    //!
    //! \brief parent
    //! \return
    //!
    QObject *parent();
    //!
    //! \brief setParent
    //! \param parent
    //!
    void setParent(QObject *parent);

    //!
    //! \brief run
    //!
    void run() override;

    //!
    //! \brief start
    //! \return
    //!
    virtual bool start();

    //!
    //! \brief stop
    //! \return
    //!
    virtual bool stop();

    //!
    //! \brief server
    //! \return
    //!
    virtual Server *server();

    //!
    //! \brief colletions
    //! \return
    //!
    virtual ListenColletions *colletions();

    //!
    //! \brief cacheRequest
    //! \return
    //!
    virtual ListenRequestCache *cacheRequest();

    //!
    //! \brief registerListenPool
    //! \param listenPool
    //!
    virtual void registerListenPool(Listen *listenPool);

    //!
    //! \brief listenPool
    //! \return
    //!
    virtual Listen &listenPool();
signals:

    //!
    //! \brief rpcRequest
    //! \param request
    //! \param uploadedFiles
    //!
    void rpcRequest(QVariantHash request, const QVariant &uploadedFiles);

    //!
    //! \brief rpcResponse
    //! \param uuid
    //! \param request
    //!
    void rpcResponse(QUuid uuid, const QVariantHash &request);

    //!
    //! \brief rpcCheck
    //!
    void rpcCheck();

protected:
    //!
    //! \brief setServer
    //! \param server
    //!
    void setServer(Server *server);

    //!
    //! \brief setColletions
    //! \param colletions
    //!
    void setColletions(ListenColletions *colletions);

private:
    ListenPvt *p = nullptr;
};

} // namespace QRpc
