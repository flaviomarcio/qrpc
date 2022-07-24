#include "./p_qrpc_listen_broker_kafka.h"

namespace QRpc {

class ListenBrokerKAFKAPvt:public QObject
{
public:
    explicit ListenBrokerKAFKAPvt(ListenBrokerKAFKA *parent):QObject{parent} {}
};

ListenBrokerKAFKA::ListenBrokerKAFKA(QObject *parent) : Listen{parent}
{
    this->p = new ListenBrokerKAFKAPvt{this};
}

} // namespace QRpc
