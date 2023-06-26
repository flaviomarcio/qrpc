#pragma once

#include "../qrpc_listen.h"

namespace QRpc {
class ListenUDPPvt;
//!
//! \brief The ListenUDP class
//!
class Q_RPC_EXPORT ListenUDP : public Listen
{
    Q_OBJECT
public:
    explicit ListenUDP(QObject *parent = nullptr);

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
    ListenUDPPvt *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(QRpc::Types::UdpSocket, ListenUDP)

} // namespace QRpc
