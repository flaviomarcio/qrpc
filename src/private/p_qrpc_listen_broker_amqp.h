#pragma once

#include "../qrpc_listen.h"
#include "../qrpc_macro.h"

namespace QRpc {

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

    //!
    //! \brief ~ListenBrokerAMQP
    //!
    ~ListenBrokerAMQP();

private:
    void *p = nullptr;
};

//QRPC_LISTTEN_AUTO_REGISTER(Amqp, ListenBrokerAMQP)

} // namespace QRpc
