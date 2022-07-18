#pragma once

#include "../qrpc_global.h"
#include "../qrpc_listen.h"
#include "../qrpc_macro.h"

namespace QRpc {
class ListenQRPCPvt;
//!
//! \brief The ListenQRPC class
//!
class Q_RPC_EXPORT ListenQRPC : public Listen
{
    Q_OBJECT
    friend class ListenColletionsPvt;
    friend class ListenQRPCSlotPvt;

public:
    //!
    //! \brief ListenQRPC
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenQRPC(QObject *parent = nullptr);

    //!
    //! \brief run
    //!
    void run() override;

    //!
    //! \brief start
    //! \return
    //!
    virtual bool start()override;

    //!
    //! \brief controllers
    //! \return
    //!
    QHash<QByteArray, const QMetaObject *> &controllers();

    //!
    //! \brief controllerParsers
    //! \return
    //!
    QHash<QByteArray, const QMetaObject *> &controllerParsers();

    //!
    //! \brief controllerMethods
    //! \return
    //!
    ControllerMethodCollection &controllerMethods();

    //!
    //! \brief controllerRedirect
    //! \return
    //!
    MultStringList &controllerRedirect();
protected:
    //!
    //! \brief registerListen
    //! \param listen
    //!
    void registerListen(Listen *listen);

    //!
    //! \brief childrenListen
    //! \param uuid
    //! \return
    //!
    virtual Listen *childrenListen(QUuid uuid);

private:
    ListenQRPCPvt *p = nullptr;
};

QRPC_LISTTEN_AUTO_REGISTER(0, ListenQRPC)

} // namespace QRpc
