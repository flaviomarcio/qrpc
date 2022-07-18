#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

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
    //! \brief ~ListenWebSocket
    //!
    ~ListenWebSocket();

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

//QRPC_LISTTEN_AUTO_REGISTER(WebSocket, ListenWebSocket)

} // namespace QRpc
