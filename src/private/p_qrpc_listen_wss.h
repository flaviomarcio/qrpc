#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The QRPCListenWebSocket class
//!
class Q_RPC_EXPORT QRPCListenWebSocket:public QRPCListen{
    Q_OBJECT
public:
    //!
    //! \brief QRPCListenWebSocket
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListenWebSocket(QObject *parent=nullptr);

    //!
    //! \brief ~QRPCListenWebSocket
    //!
    ~QRPCListenWebSocket();

    //!
    //! \brief start
    //! \return
    //!
    bool start()override;

    //!
    //! \brief stop
    //! \return
    //!
    bool stop()override;
private:
    void*p=nullptr;
};

QRPC_LISTTEN_AUTO_REGISTER(WebSocket, QRPCListenWebSocket)

}
