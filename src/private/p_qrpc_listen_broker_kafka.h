#pragma once

#include "../qrpc_listen.h"

namespace QRpc {
class ListenBrokerKAFKAPvt;

//!
//! \brief The ListenBrokerKAFKA class
//!
class Q_RPC_EXPORT ListenBrokerKAFKA : public Listen
{
    Q_OBJECT
public:
    //!
    //! \brief ListenBrokerKAFKA
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenBrokerKAFKA(QObject *parent = nullptr);

private:
    ListenBrokerKAFKAPvt *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(Kafka, ListenBrokerKAFKA)

} // namespace QRpc
