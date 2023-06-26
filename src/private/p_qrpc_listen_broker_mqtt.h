#pragma once

#include "../qrpc_listen.h"

namespace QRpc {
class ListenBrokerMQTTPvt;
//!
//! \brief The ListenBrokerMQTT class
//!
class Q_RPC_EXPORT ListenBrokerMQTT : public Listen
{
    Q_OBJECT
public:
    //!
    //! \brief ListenBrokerMQTT
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenBrokerMQTT(QObject *parent = nullptr);

private:
    ListenBrokerMQTTPvt *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(QRpc::Types::Mqtt, ListenBrokerMQTT)

} // namespace QRpc
