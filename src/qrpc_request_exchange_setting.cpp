#include "./qrpc_request_exchange_setting.h"
#include "./qrpc_macro.h"
#include <QMetaProperty>
#include "../../../qstm/src/qstm_meta_enum.h"
#include "../../../qstm/src/qstm_util_meta_object.h"
#include "../../../qstm/src/qstm_util_variant.h"

namespace QRpc {

const static auto staticDefaultLimit=60000;

class RequestExchangeSettingPvt:public QObject{
public:
    QString url;
    QStm::MetaEnum<QRpc::Types::Method> method=QRpc::Types::Method::Post;
    QString methodName;
    QStm::MetaEnum<QRpc::Types::Protocol> protocol=QRpc::Types::Protocol::Http;
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
    explicit RequestExchangeSettingPvt(RequestExchangeSetting *parent):QObject{parent}, parent{parent}
    {
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

const QVariantHash RequestExchangeSetting::toHash() const
{
    static const auto __objectName=QByteArrayLiteral("objectName");
    QVariantHash vHash;
    auto &e=*this;
    for(int i = 0; i < e.metaObject()->propertyCount(); ++i) {
        auto property=e.metaObject()->property(i);
        if(!property.isReadable())
            continue;

        if(property.name() == __objectName)
            continue;

        auto v=property.read(&e);
        if(v.isValid())
            vHash.insert(property.name(), v);
    }
    return vHash;
}

const QString &RequestExchangeSetting::url() const
{
    auto e_port=(this->port().toInt()==80 || this->port().toInt()<=0)?"":QStringLiteral(":%1").arg(this->port().toInt());
    auto __return = QStringLiteral("%1:||%2%3/%4").arg(p->protocol.name().toLower(), this->hostName(), e_port,this->route());
    while(__return.contains(QStringLiteral("//")))
        __return=__return.replace(QStringLiteral("//"), QStringLiteral("/"));
    __return=__return.replace(QStringLiteral("||"), QStringLiteral("//"));

    return p->url=__return;
}

bool RequestExchangeSetting::isValid() const
{
    if(p->protocol.equal(QRpc::Types::Amqp) && !this->topic().isEmpty() )
        return true;

    if(p->protocol.equal(QRpc::Types::Http) && !this->hostName().isEmpty() )
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

Types::Method RequestExchangeSetting::method()const
{
    return p->method.type();
}

RequestExchangeSetting &RequestExchangeSetting::setMethod(const QVariant &value)
{
    p->method=value;
    return *this;
}

Types::Protocol RequestExchangeSetting::protocol() const
{
    return p->protocol.type();
}

RequestExchangeSetting &RequestExchangeSetting::setProtocol(const QVariant &value)
{
    p->protocol=value;
    return *this;
}

const QString RequestExchangeSetting::driver() const
{
    return p->driver;
}

RequestExchangeSetting &RequestExchangeSetting::setDriver(const QString &value)
{
    p->driver=value;
    return *this;
}

const QString RequestExchangeSetting::hostName() const
{
    return p->hostName;
}

RequestExchangeSetting &RequestExchangeSetting::setHostName(const QString &value)
{
    p->hostName=value;
    return *this;
}

const QString RequestExchangeSetting::vHost() const
{
    return p->vHost;
}

RequestExchangeSetting &RequestExchangeSetting::setVHost(const QString &value)
{
    p->vHost=value;
    return *this;
}

const QString RequestExchangeSetting::userName() const
{
    return p->userName;
}

RequestExchangeSetting &RequestExchangeSetting::setUserName(const QString &value)
{
    p->userName=value;
    return *this;
}

const QString RequestExchangeSetting::passWord() const
{
    return p->passWord;
}

RequestExchangeSetting &RequestExchangeSetting::setPassWord(const QString &value)
{
    p->passWord=value;
    return *this;
}

const QString RequestExchangeSetting::route() const
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

const QString RequestExchangeSetting::topic() const
{
    return p->topic;
}

RequestExchangeSetting &RequestExchangeSetting::setTopic(const QString &value)
{
    p->topic=value;
    return *this;
}

const QVariant RequestExchangeSetting::port() const
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

const QVariantHash RequestExchangeSetting::parameter() const
{
    return p->parameter;
}

RequestExchangeSetting &RequestExchangeSetting::setParameter(const QVariantHash &value)
{
    p->parameter = value;
    return *this;
}

}
