#pragma once

#include "../qrpc_listen.h"

namespace QRpc {
class ListenTCPPvt;
//!
//! \brief The ListenTCP class
//!
class Q_RPC_EXPORT ListenTCP : public Listen
{
    Q_OBJECT
public:
    //!
    //! \brief ListenTCP
    //! \param parent
    //!
    explicit ListenTCP(QObject *parent = nullptr);

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
    ListenTCPPvt *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(QRpc::Types::TcpSocket, ListenTCP)

} // namespace QRpc
