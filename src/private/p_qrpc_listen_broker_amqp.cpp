#include "./p_qrpc_listen_broker_amqp.h"

namespace QRpc {

#define dPvt() auto &p = *reinterpret_cast<ListenBrokerAMQPPvt *>(this->p)

class ListenBrokerAMQPPvt:public QObject
{
public:
    explicit ListenBrokerAMQPPvt(ListenBrokerAMQP *parent):QObject{parent} {}
};

ListenBrokerAMQP::ListenBrokerAMQP(QObject *parent) : Listen{parent}
{
    this->p = new ListenBrokerAMQPPvt{this};
}

} // namespace QRpc
