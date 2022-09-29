#pragma once

#include "../qrpc_listen.h"
#include "../qrpc_macro.h"

namespace QRpc {
class ListenHTTPPvt;
//!
//! \brief The ListenHTTP class
//!
class Q_RPC_EXPORT ListenHTTP : public Listen
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ListenHTTP(QObject *parent = nullptr);

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
    ListenHTTPPvt *p = nullptr;
};

QRPC_LISTTEN_AUTO_REGISTER(Http, ListenHTTP)

} // namespace QRpc
