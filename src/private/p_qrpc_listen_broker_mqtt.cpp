#include "./p_qrpc_listen_broker_mqtt.h"

namespace QRpc {

class ListenBrokerMQTTPvt:public QObject
{
public:
    explicit ListenBrokerMQTTPvt(ListenBrokerMQTT *parent):QObject{parent} {}
};

ListenBrokerMQTT::ListenBrokerMQTT(QObject *parent) : Listen{parent}
{
    this->p = new ListenBrokerMQTTPvt{this};
}

} // namespace QRpc
