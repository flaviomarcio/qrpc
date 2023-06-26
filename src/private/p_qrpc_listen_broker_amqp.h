#pragma once

#include "../qrpc_listen.h"

namespace QRpc {
class ListenBrokerAMQPPvt;
//!
//! \brief The ListenBrokerAMQP class
//!
class Q_RPC_EXPORT ListenBrokerAMQP : public Listen
{
    Q_OBJECT
public:
    //!
    //! \brief ListenBrokerAMQP
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenBrokerAMQP(QObject *parent = nullptr);

private:
    ListenBrokerAMQPPvt *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(QRpc::Types::Amqp, ListenBrokerAMQP)

} // namespace QRpc
