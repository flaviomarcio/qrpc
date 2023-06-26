#include "./qrpc_listen_protocol.h"
#include "./qrpc_listen.h"
#include "./qrpc_types.h"
#include "../../qstm/src/qstm_macro.h"
#include "../../qstm/src/qstm_util_variant.h"
#include "../../qstm/src/qstm_envs.h"
#include "../../qstm/src/qstm_meta_enum.h"
//#include "./qrpc_listen_colletion.h"
#include <QMetaProperty>
#include <QSettings>
#include <QThreadPool>

namespace QRpc {

static const auto listen_tcp_port = "555";
static const auto listen_udp_port = "556";
static const auto listen_web_port = "8081";
static const auto listen_amqp_port = "5672";
static const auto listen_mqtt_port = "1883";
static const auto listen_http_port = "8080";
static const auto listen_database_port = "5434";
static const auto listen_kafka_port = "2181";
static const auto __path_separator="/";
static const auto __path_separatorTwo="//";

class ListenProtocolPvt : public QObject
{
public:
    QStm::MetaEnum<QRpc::Types::Protocol> protocol;
    QMetaObject protocolMetaObject;
    QVariantHash settingsHash;
    QSettings *settings = nullptr;
    QByteArray optionName;
    int minThreads = 1;
    int maxThreads = QThreadPool().maxThreadCount();
    int cleanupInterval = 5000;
    int maxRequestSize = 10000000;   //9,536743164 MB
    int maxMultiPartSize = 10000000; //9,536743164 MB
    QByteArray hostName;
    QVariant port;
    QByteArray driver;
    QByteArray userName;
    QByteArray password;
    QByteArray database;
    QByteArray options;
    QByteArray contextPath;
    QVariantList queue;
    bool enabled = false;
    QByteArray sslKeyFile;
    QByteArray sslCertFile;

    QStm::Envs env;

    explicit ListenProtocolPvt(QObject *parent) : QObject{parent}, env{parent}
    {
        this->settings = new QSettings(nullptr);
    }

    ~ListenProtocolPvt() { delete this->settings; }

    void setSettings(const QVariantHash &settings, const QVariantHash &defaultSettings)
    {
        QStm::Envs envs;
        static auto exceptionProperty = QStringList{QStringLiteral("protocol"),
                QStringLiteral("protocolname"),
                QStringLiteral("optionname")};
        this->settingsHash = settings.isEmpty() ? this->settingsHash : settings;
        const QMetaObject *metaObject = dynamic_cast<ListenProtocol *>(this->parent())->metaObject();
        for (int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); i++) {
            auto property = metaObject->property(i);
            auto propertyName = QString::fromUtf8(property.name()).toLower();

            if (exceptionProperty.contains(propertyName))
                continue;

            propertyName = QString::fromUtf8(property.name());
            auto value = this->settingsHash.value(propertyName);
            if (!value.isValid())
                value = defaultSettings.value(propertyName);

            value=envs.parser(value);

            if (property.write(this->parent(), value))
                continue;

            switch (property.typeId()) {
            case QMetaType::QUuid:
                if (property.write(this->parent(), value.toUuid()))
                    continue;
                break;
            case QMetaType::LongLong:
            case QMetaType::ULongLong:
                if (property.write(this->parent(), value.toLongLong()))
                    continue;
                break;
            case QMetaType::Int:
            case QMetaType::UInt:
                if (property.write(this->parent(), value.toInt()))
                    continue;
                break;
            case QMetaType::Bool:
                if (property.write(this->parent(), value.toBool()))
                    continue;
                break;
            default:
                break;
            }
        }
        this->makeHash();
    }

    ListenProtocolPvt &makeHash()
    {
        this->settingsHash.clear();
        this->settings->clear();
        const QMetaObject *metaObject = dynamic_cast<ListenProtocol *>(this->parent())->metaObject();
        for (int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); i++) {
            auto property = metaObject->property(i);
            auto key = property.name();
            auto value = property.read(this->parent());
            this->settingsHash.insert(key, value);
            this->settings->setValue(key, value);
        }
        return *this;
    }

public slots:
    void changeHash() { this->makeHash(); }
};

ListenProtocol::ListenProtocol(QObject *parent) : QObject{parent}, p{new ListenProtocolPvt{this}}
{
}

ListenProtocol::ListenProtocol(QRpc::Types::Protocol protocol, const QMetaObject &metaObject, QObject *parent)
    : QObject{parent}, p{new ListenProtocolPvt{this}}
{

    QObject::connect(this, &ListenProtocol::protocolChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::protocolNameChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::optionNameChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this,
                     &ListenProtocol::cleanupIntervalChanged,
                     this->p,
                     &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::minThreadsChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::maxThreadsChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this,
                     &ListenProtocol::maxRequestSizeChanged,
                     this->p,
                     &ListenProtocolPvt::changeHash);
    QObject::connect(this,
                     &ListenProtocol::maxMultiPartSizeChanged,
                     this->p,
                     &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::driverChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::hostNameChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::userNameChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::passwordChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::databaseChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::optionsChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::portChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::queueChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::sslKeyFileChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::sslCertFileChanged, this->p, &ListenProtocolPvt::changeHash);
    QObject::connect(this, &ListenProtocol::enabledChanged, this->p, &ListenProtocolPvt::changeHash);

    p->protocol = protocol;
    p->optionName = p->protocol.name().toLower();
    p->protocolMetaObject = metaObject;
    p->makeHash();
}

bool ListenProtocol::isValid() const
{
    return p->protocol.isValid();
}

Listen *ListenProtocol::makeListen()
{
    auto object = p->protocolMetaObject.newInstance(Q_ARG(QObject *, this));
    if (object == nullptr)
        return nullptr;

    auto listen = dynamic_cast<Listen *>(object);
    if (listen == nullptr) {
        delete object;
        return nullptr;
    }

    object->setObjectName(QStringLiteral("lis_%1").arg(QString::fromUtf8(p->protocol.name().toLower())));
    return listen;
}

QRpc::Types::Protocol ListenProtocol::protocol()
{
    return p->protocol.type();
}

void ListenProtocol::setProtocol(const QVariant &value)
{
    p->protocol = value;
}

QByteArray ListenProtocol::optionName()
{
    return p->optionName;
}

void ListenProtocol::setSettings(const QVariantHash &settings, const QVariantHash &defaultSettings)
{
    p->setSettings(settings, defaultSettings);
}

void ListenProtocol::setOptionName(const QByteArray &value)
{
    p->optionName = value;
}

int ListenProtocol::minThreads() const
{
    return p->minThreads;
}

void ListenProtocol::setMinThreads(int value)
{
    p->minThreads = value;
}

int ListenProtocol::maxThreads() const
{
    return p->maxThreads;
}

void ListenProtocol::setMaxThreads(int value)
{
    p->maxThreads = value;
}

int ListenProtocol::cleanupInterval() const
{
    return p->cleanupInterval;
}

void ListenProtocol::setCleanupInterval(int value)
{
    p->cleanupInterval = value;
}

int ListenProtocol::maxRequestSize() const
{
    return p->maxRequestSize;
}

void ListenProtocol::setMaxRequestSize(int value)
{
    p->maxRequestSize = value;
}

int ListenProtocol::maxMultiPartSize() const
{
    return p->maxMultiPartSize;
}

void ListenProtocol::setMaxMultiPartSize(int value)
{
    p->maxMultiPartSize = value;
}

QByteArray ListenProtocol::driver() const
{
    return p->driver;
}

void ListenProtocol::setDriver(const QByteArray &value)
{
    p->driver = value;
}

QByteArray ListenProtocol::hostName() const
{
    return p->hostName;
}

void ListenProtocol::setHostName(const QByteArray &value)
{
    p->hostName = value;
}

QByteArray ListenProtocol::userName() const
{
    return p->userName;
}

void ListenProtocol::setUserName(const QByteArray &value)
{
    p->userName = value;
}

QByteArray ListenProtocol::password() const
{
    return p->password;
}

void ListenProtocol::setPassword(const QByteArray &value)
{
    p->password = value;
}

QByteArray ListenProtocol::database() const
{
    return p->database;
}

void ListenProtocol::setDatabase(const QByteArray &value)
{
    p->database = value;
}

QByteArray ListenProtocol::options() const
{
    return p->options;
}

void ListenProtocol::setOptions(const QByteArray &value)
{
    p->options = value;
}

QVariantList &ListenProtocol::queue()
{
    return p->queue;
}

void ListenProtocol::setQueue(const QByteArray &value)
{
    p->queue.clear();
    p->queue.append(value);
}

void ListenProtocol::setQueue(const QVariantList &value)
{
    p->queue = value;
}

QVariantList ListenProtocol::port() const
{
    auto value=p->env.parser(p->port);
    QVariantList l;
    switch (value.typeId()) {
    case QMetaType::QStringList:
    case QMetaType::QVariantList:
        l = value.toList();
        break;
    default:
        l.append(value);
    }

    if (value.isValid()){
        Q_DECLARE_VU;
        return vu.toList(value);
    }

    switch (p->protocol.type()) {
    case QRpc::Types::Protocol::TcpSocket:
        return QVariantList{listen_tcp_port};
    case QRpc::Types::Protocol::UdpSocket:
        return QVariantList{listen_udp_port};
    case QRpc::Types::Protocol::WebSocket:
        return QVariantList{listen_web_port};
    case QRpc::Types::Protocol::Http:
        return QVariantList{listen_http_port};
    case QRpc::Types::Protocol::Amqp:
        return QVariantList{listen_amqp_port};
    case QRpc::Types::Protocol::Mqtt:
        return QVariantList{listen_mqtt_port};
    case QRpc::Types::Protocol::DataBase:
        return QVariantList{listen_database_port};
    case QRpc::Types::Protocol::Kafka:
        return QVariantList{listen_kafka_port};
    default:
        return {};
    }
}

void ListenProtocol::setPort(const QVariant &ports)
{
    p->port = ports;
}

QVariantHash &ListenProtocol::toHash() const
{
    return p->makeHash().settingsHash;
}

QSettings &ListenProtocol::settings() const
{
    return *p->makeHash().settings;
}

QSettings *ListenProtocol::makeSettings(QObject *parent)
{
    auto settings = new QSettings(parent);
    Q_V_HASH_ITERATOR(p->makeHash().settingsHash){
        i.next();
        settings->setValue(i.key().toLower(), p->env.parser(i.value()));
    }
    return settings;
}

QVariantHash ListenProtocol::makeSettingsHash() const
{
    return p->makeHash().settingsHash;
}

bool ListenProtocol::enabled() const
{
    return p->enabled;
}

void ListenProtocol::setEnabled(bool value)
{
    p->enabled = value;
}

QByteArray ListenProtocol::sslKeyFile() const
{
    return p->sslKeyFile;
}

void ListenProtocol::setSslKeyFile(const QByteArray &value)
{
    p->sslKeyFile = value;
}

QByteArray ListenProtocol::sslCertFile() const
{
    return p->sslCertFile;
}

void ListenProtocol::setSslCertFile(const QByteArray &value)
{
    p->sslCertFile = value;
}

const QByteArray &ListenProtocol::contextPath() const
{
    return p->contextPath;
}

void ListenProtocol::setContextPath(const QByteArray &newContextPath)
{
    QByteArray contextPath=__path_separator+newContextPath.trimmed().toLower();
    if(contextPath==__path_separator || contextPath==__path_separatorTwo)
        contextPath={};

    while(contextPath.contains(__path_separatorTwo))
        contextPath=contextPath.replace(__path_separatorTwo, __path_separator);

    if(!contextPath.isEmpty() && contextPath.at(contextPath.length()-1)=='/')
        contextPath=contextPath.mid(0,contextPath.length()-1);

    if (p->contextPath == contextPath)
        return;
    p->contextPath = contextPath;
    emit contextPathChanged();
}

void ListenProtocol::resetContextPath()
{
    setContextPath({});
}

} // namespace QRpc
