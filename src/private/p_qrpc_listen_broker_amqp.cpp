#include "./p_qrpc_listen_broker_amqp.h"

namespace QRpc {

#define dPvt() auto &p = *reinterpret_cast<ListenBrokerAMQPPvt *>(this->p)

class ListenBrokerAMQPPvt
{
public:
    explicit ListenBrokerAMQPPvt(ListenBrokerAMQP *object) { Q_UNUSED(object) }
    virtual ~ListenBrokerAMQPPvt() {}
};

ListenBrokerAMQP::ListenBrokerAMQP(QObject *parent) : Listen{parent}
{
    this->p = new ListenBrokerAMQPPvt{this};
}

ListenBrokerAMQP::~ListenBrokerAMQP()
{
    dPvt();
    delete &p;
}

} // namespace QRpc
