#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The ListenUDP class
//!
class Q_RPC_EXPORT ListenUDP : public Listen
{
    Q_OBJECT
public:
    explicit ListenUDP(QObject *parent = nullptr);
    ~ListenUDP();

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
    void *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(UdpSocket, ListenUDP)

} // namespace QRpc
