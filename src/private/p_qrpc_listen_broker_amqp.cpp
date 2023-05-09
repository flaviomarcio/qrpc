#include "./p_qrpc_listen_broker_amqp.h"

namespace QRpc {

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
