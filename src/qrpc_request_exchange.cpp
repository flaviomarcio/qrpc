#include "./qrpc_request_exchange.h"
#include "./private/p_qrpc_request_job.h"
namespace QRpc {

#define dPvt()\
    auto &p =*reinterpret_cast<RequestExchangePvt*>(this->p)

class RequestExchangePvt:public QObject{
public:
    RequestExchange*parent=nullptr;
    RequestExchangeSetting call;
    RequestExchangeSetting back;

    explicit RequestExchangePvt(RequestExchange*parent):QObject{parent}, call{parent}, back{parent}
    {
        this->parent=parent;
    }

    virtual ~RequestExchangePvt()
    {
    }
};

RequestExchange::RequestExchange(QObject *parent):QObject{parent}
{
    this->p = new RequestExchangePvt{this};
}

RequestExchange::RequestExchange(RequestExchange &exchange, QObject *parent):QObject{parent}
{
    this->p = new RequestExchangePvt{this};

    p->call=exchange.call();
    p->back=exchange.back();
}

RequestExchange&RequestExchange::operator =(RequestExchange &e)
{

    p->call=e.call();
    p->back=e.back();
    return*this;
}

RequestExchange &RequestExchange::clear()
{

    p->call.clear();
    p->back.clear();
    return*this;
}

RequestExchangeSetting &RequestExchange::call()
{

    return p->call;
}

RequestExchangeSetting &RequestExchange::back()
{

    return p->back;
}

QVariantMap RequestExchange::toMap() const
{

    return QVariantMap{{qsl("call"),p->call.toHash()}, {qsl("back"),p->back.toHash()}};
}

QVariantHash RequestExchange::toHash() const
{

    return QVariantHash{{qsl("call"),p->call.toHash()}, {qsl("back"),p->back.toHash()}};
}

RequestExchange &RequestExchange::print(const QString &output)
{
    for(auto &v:this->printOut(output))
        sInfo()<<v;
    return*this;
}

QStringList RequestExchange::printOut(const QString &output)
{

    QStringList out;
    auto space=output.trimmed().isEmpty()?qsl_null:qsl("    ");
    if(p->call.isValid()){
        out<<qsl("%1%2 exchange.call").arg(space, output).trimmed();
        auto outtext=space+qsl(".    ");
        for(auto &v:p->call.printOut(outtext))
            out<<v;
    }
    if(p->back.isValid()){
        out<<qsl("%1%2 exchange.back").arg(space, output).trimmed();
        auto outtext=space+qsl(".    ");
        for(auto &v:p->back.printOut(outtext))
            out<<v;
    }
    return out;
}

void RequestExchange::setProtocol(const QVariant &value)
{
    this->call().setProtocol(value);
}

void RequestExchange::setProtocol(const Protocol &value)
{
    this->call().setProtocol(value);
}

void RequestExchange::setMethod(const QString &value)
{
    return this->call().setMethod(value);
}

void RequestExchange::setMethod(const RequestMethod &value)
{
    this->call().setMethod(value);
}

void RequestExchange::setHostName(const QString &value)
{
    this->call().setHostName(value);
}

void RequestExchange::setRoute(const QVariant &value)
{
    this->call().setRoute(value);
}

void RequestExchange::setPort(const int &value)
{
    this->call().setPort(value);
}

}
