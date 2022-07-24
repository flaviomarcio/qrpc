#pragma once

#include "../qrpc_listen.h"

namespace QRpc {
class ListenLocalSocketPvt;
//!
//! \brief The ListenLocalSocket class
//!
class Q_RPC_EXPORT ListenLocalSocket : public Listen
{
    Q_OBJECT
public:
    //!
    //! \brief ListenLocalSocket
    //! \param parent
    //!
    explicit ListenLocalSocket(QObject *parent = nullptr);

    //!
    //! \brief start
    //! \return
    //!
    bool start() override;

    //!
    //! \brief stop
    //! \return
    //!
    bool stop() override;

private:
    ListenLocalSocketPvt *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(LocalSocket, ListenLocalSocket)

} // namespace QRpc
