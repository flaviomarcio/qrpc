#pragma once

#include "../qrpc_listen.h"

namespace QRpc {
class ListenWebSocketPvt;
//!
//! \brief The ListenWebSocket class
//!
class Q_RPC_EXPORT ListenWebSocket : public Listen
{
    Q_OBJECT
public:
    //!
    //! \brief ListenWebSocket
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenWebSocket(QObject *parent = nullptr);

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
    ListenWebSocketPvt *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(QRpc::Types::WebSocket, ListenWebSocket)

} // namespace QRpc
