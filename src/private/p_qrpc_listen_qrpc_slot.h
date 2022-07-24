#pragma once

#include <QByteArray>
#include <QEventLoop>
#include <QVariant>
#include <QVariantHash>
#include "./p_qrpc_listen_qrpc.h"

namespace QRpc {
class ListenQRPCSlotPvt;

//!
//! \brief The ListenQRPCSlot class
//!
class Q_RPC_EXPORT ListenQRPCSlot : public QThread
{
    Q_OBJECT
public:
    explicit ListenQRPCSlot(ListenQRPC *listenQRPC = nullptr);

    //!
    //! \brief run
    //!
    void run() override;

    //!
    //! \brief canRequestInvoke
    //! \param v
    //! \param uploadedFiles
    //! \return
    //!
    bool canRequestInvoke(QVariantHash &v, const QVariant &uploadedFiles);

    //!
    //! \brief start
    //!
    void start();

    //!
    //! \brief lock
    //! \return
    //!
    virtual bool lock();

    //!
    //! \brief unlock
    //!
    virtual void unlock();

signals:

    //!
    //! \brief requestInvoke
    //! \param v
    //! \param uploadedFiles
    //!
    void requestInvoke(QVariantHash v, const QVariant &uploadedFiles);

private:
    ListenQRPCSlotPvt *p = nullptr;
};

} // namespace QRpc
