#pragma once

#include "../qrpc_listen.h"
#include "../qrpc_macro.h"

namespace QRpc {

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

    //!
    //! \brief ~ListenBrokerKAFKA
    //!
    ~ListenBrokerKAFKA();

private:
    void *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(Kafka, ListenBrokerKAFKA)

} // namespace QRpc
