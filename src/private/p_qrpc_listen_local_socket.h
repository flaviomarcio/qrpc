#pragma once

#include "../qrpc_listen.h"
#include "../qrpc_macro.h"

namespace QRpc {

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
    ~ListenLocalSocket();

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

//QRPC_LISTTEN_AUTO_REGISTER(LocalSocket, ListenLocalSocket)

} // namespace QRpc
