#include "./qrpc_request_exchange_setting.h"
#include "./qrpc_macro.h"
#include <QMetaProperty>

namespace QRpc {

const static auto staticDefaultLimit=60000;

class RequestExchangeSettingPvt:public QObject{
public:
    QVariantHash toHash;
    QString url;
    RequestMethod method=RequestMethod::Post;
    QString methodName;
    Protocol protocol=Protocol::Http;
    QString vHost="/";
    QString hostName="localhost";
    QString driver;
    QString userName="guest";
    QString passWord="guest";
    QString route;
    QString topic;
    QVariantHash parameter;
    QVariant port=8080;
    QVariant activityLimit=staticDefaultLimit;
    RequestExchangeSetting *parent=nullptr;
    QString __protocolName, __protocolUrlName;
    explicit RequestExchangeSettingPvt(RequestExchangeSetting *parent):QObject{parent}, parent{parent}
    {
    }

    const QString &protocolName()
    {
        return this->__protocolName=QRpc::ProtocolName.value(this->protocol);
    }

    const QString &protocolUrlName()
    {
        return this->__protocolUrlName=QRpc::ProtocolUrlName.value(this->protocol);
    }

    void load(const RequestExchangeSetting &e)
    {
        QStm::MetaObjectUtil util(*e.metaObject());
        auto values=util.toHash(&e);
        util.writeHash(this->parent, values);
    }
};

RequestExchangeSetting::RequestExchangeSetting(QObject *parent):QObject{parent}, p{new RequestExchangeSettingPvt{this}}
{
}

RequestExchangeSetting::RequestExchangeSetting(const RequestExchangeSetting &e, QObject *parent):QObject{parent}, p{new RequestExchangeSettingPvt{this}}
{
    p->load(e);
}

RequestExchangeSetting&RequestExchangeSetting::operator=(const RequestExchangeSetting &e)
{
    p->load(e);
    return *this;
}

RequestExchangeSetting &RequestExchangeSetting::clear()
{
    static const auto __objectName=QByteArrayLiteral("objectName");
    auto &e=*this;
    for(int i = 0; i < e.metaObject()->propertyCount(); ++i) {
        auto property=e.metaObject()->property(i);
        if(property.name() == __objectName)
            continue;
        if(property.isResettable())
            property.reset(this);
        else
            property.write(this, {});
    }
    return *this;
}

RequestExchangeSetting &RequestExchangeSetting::operator=(const QVariantHash &e)
{
    QStm::MetaObjectUtil util;
    util.writeHash(this, e);
    return *this;
}

const QVariantHash &RequestExchangeSetting::toHash() const
{
    static const auto __objectName=QByteArrayLiteral("objectName");
    QVariantHash vHash;
    auto &e=*this;
    for(int i = 0; i < e.metaObject()->propertyCount(); ++i) {
        auto property=e.metaObject()->property(i);
        if(property.name() == __objectName)
            continue;

        auto v=property.read(&e);
        if(v.isValid())
            vHash.insert(property.name(), v);
    }
    return p->toHash=vHash;
}

const QString &RequestExchangeSetting::url() const
{
    auto __return = QStringLiteral("%1:||%2:%3/%4").arg(this->protocolName(),this->hostName(),QString::number(this->port().toInt()),this->route());
    while(__return.contains(QStringLiteral("//")))
        __return=__return.replace(QStringLiteral("//"), QStringLiteral("/"));
    __return=__return.replace(QStringLiteral("||"), QStringLiteral("//"));

    return p->url=__return;
}

bool RequestExchangeSetting::isValid() const
{
    if(this->protocol()==QRpc::Amqp && !this->topic().isEmpty())
        return true;
    if(this->protocol()==QRpc::Http && !this->hostName().isEmpty())
        return true;
    return false;

}

RequestExchangeSetting &RequestExchangeSetting::print(const QString &output)
{
    for(auto &v:this->printOut(output))
        rInfo()<<v;
    return *this;
}

QStringList RequestExchangeSetting::printOut(const QString &output)
{
    static const auto __password=QStringLiteral("password");
    Q_DECLARE_VU;

    auto space=output.trimmed().isEmpty()?"":QStringLiteral("    ");
    auto vHash=this->toHash();
    if(vHash.isEmpty())
        return {};

    QStringList out;
    out<<QStringLiteral("%1%2 attributes").arg(space, output).trimmed();
    QHashIterator<QString, QVariant> i(vHash);
    while (i.hasNext()){
        i.next();
        auto key=i.key();
        if(key.toLower()==__password)
            continue;
        auto value=vu.toStr(i.value());
        out.append(QStringLiteral("%1%2     %3:%4").arg(space, output, key, value));
    }
    return out;
}


RequestMethod RequestExchangeSetting::method()const
{
    return p->method;
}

RequestExchangeSetting &RequestExchangeSetting::setMethod(const int &value)
{
    auto method=RequestMethod(value);
    method=(method<Head || method>MaxMethod)?Post:method;
    p->method=method;
    return *this;
}

RequestExchangeSetting &RequestExchangeSetting::setMethod(const QString &value)
{
    const auto vv=value.trimmed().toLower();
    for (const auto &v : RequestMethodNameList){
        if(v.trimmed().toLower()!=vv)
            return *this;
        const auto &i = RequestMethodName.key(v);
        p->method=RequestMethod(i);
        return *this;
    }
    p->method=QRpc::Post;
    return *this;
}

const QString &RequestExchangeSetting::methodName() const
{
    return p->methodName=RequestMethodName.value(p->method);
}

Protocol RequestExchangeSetting::protocol() const
{
    return p->protocol;
}

const QString &RequestExchangeSetting::protocolName() const
{
    return p->protocolName();
}

const QString &RequestExchangeSetting::protocolUrlName() const
{
    return p->protocolUrlName();
}

RequestExchangeSetting &RequestExchangeSetting::setProtocol(const Protocol &value)
{
    p->protocol=value;
    return *this;
}

RequestExchangeSetting &RequestExchangeSetting::setProtocol(const QVariant &value)
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
    return *this;
}

const QString &RequestExchangeSetting::driver() const
{
    return p->driver;
}

RequestExchangeSetting &RequestExchangeSetting::setDriver(const QString &value)
{
    p->driver=value;
    return *this;
}

const QString &RequestExchangeSetting::hostName() const
{
    return p->hostName;
}

RequestExchangeSetting &RequestExchangeSetting::setHostName(const QString &value)
{
    p->hostName=value;
    return *this;
}

const QString &RequestExchangeSetting::vHost() const
{
    return p->vHost;
}

RequestExchangeSetting &RequestExchangeSetting::setVHost(const QString &value)
{
    p->vHost=value;
    return *this;
}

const QString &RequestExchangeSetting::userName() const
{
    return p->userName;
}

RequestExchangeSetting &RequestExchangeSetting::setUserName(const QString &value)
{
    p->userName=value;
    return *this;
}

const QString &RequestExchangeSetting::passWord() const
{
    return p->passWord;
}

RequestExchangeSetting &RequestExchangeSetting::setPassWord(const QString &value)
{
    p->passWord=value;
    return *this;
}

const QString &RequestExchangeSetting::route() const
{
    return p->route;
}

RequestExchangeSetting &RequestExchangeSetting::setRoute(const QVariant &value)
{
    switch (value.typeId()) {
    case QMetaType::QUrl:
        p->route = value.toUrl().toString();
        break;
    default:
        p->route = value.toString().trimmed();
    }
    while(p->route.contains(QStringLiteral("//")))
        p->route=p->route.replace(QStringLiteral("//"),QStringLiteral("/"));
    while(p->route.endsWith(QStringLiteral("/")))
        p->route=p->route.left(p->route.length()-1);
    return *this;
}

const QString &RequestExchangeSetting::topic() const
{
    return p->topic;
}

RequestExchangeSetting &RequestExchangeSetting::setTopic(const QString &value)
{
    p->topic=value;
    return *this;
}

const QVariant &RequestExchangeSetting::port() const
{
    return p->port;
}

RequestExchangeSetting &RequestExchangeSetting::setPort(const QVariant &value)
{
    p->port = value;
    return *this;
}

qlonglong RequestExchangeSetting::activityLimit() const
{
    auto l=p->activityLimit.toLongLong();
    l=(l<1000)?staticDefaultLimit:l;
    return l;
}

RequestExchangeSetting &RequestExchangeSetting::setActivityLimit(const QVariant &value)
{
    p->activityLimit=value;
    return *this;
}

const QVariantHash &RequestExchangeSetting::parameter() const
{
    return p->parameter;
}

RequestExchangeSetting &RequestExchangeSetting::setParameter(const QVariantHash &value)
{
    p->parameter = value;
    return *this;
}

}
