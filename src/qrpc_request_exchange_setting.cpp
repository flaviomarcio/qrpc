#include "./qrpc_request_exchange_setting.h"
#include <QMetaProperty>
#include <QStm>

namespace QRpc {

#define dPvt()\
    auto &p =*reinterpret_cast<RequestExchangeSettingPvt*>(this->p)

const static auto staticDefaultLimit=60000;

class RequestExchangeSettingPvt{
public:
    RequestMethod method=RequestMethod::Post;
    Protocol protocol=Protocol::Http;
    QString vHost="/";
    QString hostName="localhost";
    QString driver;
    QString userName="guest";
    QString passWord="guest";
    QString route;
    QString topic;
    QVariantHash parameter;
    int port=8080;
    QVariant activityLimit=staticDefaultLimit;
    RequestExchangeSetting*parent=nullptr;
    explicit RequestExchangeSettingPvt(RequestExchangeSetting*parent)
    {
        this->parent=parent;
    }

    virtual ~RequestExchangeSettingPvt()
    {
    }

    QString protocolName()const
    {
        const auto &v=ProtocolName.value(this->protocol);
        return v;
    }

    QString protocolUrlName()const
    {
        const auto &v=ProtocolUrlName.value(this->protocol);
        return v;
    }
};

RequestExchangeSetting::RequestExchangeSetting(QObject *parent):QObject{parent}
{
    this->p = new RequestExchangeSettingPvt{this};
}

RequestExchangeSetting::RequestExchangeSetting(RequestExchangeSetting &e, QObject *parent):QObject{parent}
{
    this->p = new RequestExchangeSettingPvt{this};
    *this=e;
}


RequestExchangeSetting&RequestExchangeSetting::operator=(const RequestExchangeSetting &e)
{
    QStm::MetaObjectUtil util(*e.metaObject());
    auto values=util.toHash(&e);\
    util.writeHash(this, values);
    return*this;
}

RequestExchangeSetting &RequestExchangeSetting::clear()
{
    auto &e=*this;
    for(int i = 0; i < e.metaObject()->propertyCount(); ++i) {
        auto property=e.metaObject()->property(i);
        if(QByteArray{property.name()} == qbl("objectName"))
            continue;

        property.write(this, {});
    }
    return*this;
}

RequestExchangeSetting &RequestExchangeSetting::operator=(const QVariantHash &e)
{
    QStm::MetaObjectUtil util;
    util.writeHash(this, e);
    return*this;
}

QVariantMap RequestExchangeSetting::toMap() const
{
    QVariantMap map;
    auto &e=*this;
    for(int i = 0; i < e.metaObject()->propertyCount(); ++i) {
        auto property=e.metaObject()->property(i);
        if(QByteArray{property.name()} == qbl("objectName"))
            continue;

        auto v=property.read(&e);
        if(v.isValid())
            map.insert(property.name(), v);
    }
    return map;
}

QVariantHash RequestExchangeSetting::toHash() const
{
    QVariantHash map;
    auto &e=*this;
    for(int i = 0; i < e.metaObject()->propertyCount(); ++i) {
        auto property=e.metaObject()->property(i);
        if(QByteArray{property.name()} == qbl("objectName"))
            continue;

        auto v=property.read(&e);
        if(v.isValid())
            map.insert(property.name(), v);
    }
    return map;
}

QString RequestExchangeSetting::url() const
{
    auto __return = qsl("%1:||%2:%3/%4").arg(this->protocolName(),this->hostName(),QString::number(this->port()),this->route());
    while(__return.contains(qsl("//")))
        __return=__return.replace(qsl("//"), qsl("/"));
    __return=__return.replace(qsl("||"), qsl("//"));

    return __return;
}

bool RequestExchangeSetting::isValid() const
{
    if(this->protocol()==QRpc::Amqp && !this->topic().isEmpty())
        return true;
    return false;

}

RequestExchangeSetting &RequestExchangeSetting::print(const QString &output)
{
    for(auto &v:this->printOut(output))
        sInfo()<<v;
    return*this;
}

QStringList RequestExchangeSetting::printOut(const QString &output)
{
    Q_DECLARE_VU;

    auto space=output.trimmed().isEmpty()?qsl_null:qsl("    ");
    auto vHash=this->toHash();
    if(vHash.isEmpty())
        return {};

    QStringList out;
    out<<qsl("%1%2 attributes").arg(space, output).trimmed();
    QHashIterator<QString, QVariant> i(vHash);
    while (i.hasNext()){
        i.next();
        if(i.key().toLower()==qsl("password"))
            continue;
        out<<qsl("%1%2     %2:%3").arg(space, output, i.key(), vu.toStr(i.value()));
    }
    return out;
}


RequestMethod RequestExchangeSetting::method()const
{

    return p->method;
}

void RequestExchangeSetting::setMethod(const int &value)
{

    auto method=RequestMethod(value);
    method=(method<Head || method>MaxMethod)?Post:method;
    p->method=method;
}

void RequestExchangeSetting::setMethod(const QString &value)
{

    const auto vv=value.trimmed().toLower();
    for (const auto &v : RequestMethodNameList){
        if(v.trimmed().toLower()!=vv)
            return;
        const auto &i = RequestMethodName.key(v);
        p->method=RequestMethod(i);
        return;
    }
    p->method=QRpc::Post;
}

QString RequestExchangeSetting::methodName() const
{

    return RequestMethodName[p->method];
}

Protocol RequestExchangeSetting::protocol() const
{

    return p->protocol;
}

QString RequestExchangeSetting::protocolName() const
{

    return p->protocolName();
}

QString RequestExchangeSetting::protocolUrlName() const
{

    return p->protocolUrlName();
}

void RequestExchangeSetting::setProtocol(const Protocol &value)
{

    p->protocol=value;
}

void RequestExchangeSetting::setProtocol(const QVariant &value)
{

    auto &v=p->protocol;
    if(value.isNull() || !value.isValid())
        v=Protocol::Http;
    else if(QString::number(value.toInt())==value)
        v=Protocol(value.toInt());
    else if(value.toString().trimmed().isEmpty())
        v=Protocol::Http;
    else
        v=Protocol(ProtocolType.value(value.toString().trimmed()));

    v=(v>rpcProtocolMax)?rpcProtocolMax:v;
    v=(v<rpcProtocolMin)?rpcProtocolMin:v;

}

QString &RequestExchangeSetting::driver() const
{

    return p->driver;
}

void RequestExchangeSetting::setDriver(const QString &value)
{

    p->driver=value;
}

QString &RequestExchangeSetting::hostName() const
{

    return p->hostName;
}

void RequestExchangeSetting::setHostName(const QString &value)
{

    p->hostName=value;
}

QString &RequestExchangeSetting::vHost() const
{

    return p->vHost;
}

void RequestExchangeSetting::setVHost(const QString &value)
{

    p->vHost=value;
}

QString &RequestExchangeSetting::userName() const
{

    return p->userName;
}

void RequestExchangeSetting::setUserName(const QString &value)
{

    p->userName=value;
}

QString &RequestExchangeSetting::passWord() const
{

    return p->passWord;
}

void RequestExchangeSetting::setPassWord(const QString &value)
{

    p->passWord=value;
}

QString &RequestExchangeSetting::route() const
{

    return p->route;
}

void RequestExchangeSetting::setRoute(const QVariant &value)
{

    auto typeId=qTypeId(value);
    switch (typeId) {
    case QMetaType_QUrl:
        p->route = value.toUrl().toString();
        break;
    default:
        p->route = value.toString().trimmed();
    }
    while(p->route.contains(qsl("//")))
        p->route=p->route.replace(qsl("//"),qsl("/"));
    while(p->route.endsWith(qsl("/")))
        p->route=p->route.left(p->route.length()-1);
}

QString &RequestExchangeSetting::topic() const
{

    return p->topic;
}

void RequestExchangeSetting::setTopic(const QString &value)
{

    p->topic=value;
}

int RequestExchangeSetting::port() const
{

    return p->port;
}

void RequestExchangeSetting::setPort(int port)
{

    p->port = port;
}

qlonglong RequestExchangeSetting::activityLimit() const
{

    auto l=p->activityLimit.toLongLong();
    l=(l<1000)?staticDefaultLimit:l;
    return l;
}

void RequestExchangeSetting::setActivityLimit(const QVariant &value)
{

    p->activityLimit=value;
}

QVariantHash &RequestExchangeSetting::parameter() const
{

    return p->parameter;
}

void RequestExchangeSetting::setParameter(const QVariantHash &parameter)
{

    p->parameter = parameter;
}

}
