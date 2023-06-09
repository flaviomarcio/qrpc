#include "./qrpc_server.h"
#include "./qrpc_controller.h"
#include "./qrpc_macro.h"

namespace QRpc {

static const auto __protocol="protocol";
static const auto __controller="controller";

//!
//! \brief The ServerPvt class
//!
class Q_RPC_EXPORT ServerPvt : public QObject
{
public:
    Server *parent = nullptr;
    ControllerOptions controllerOptions;
    QString serverName;
    QVariant settingsFileName;
    ListenColletions *listenColletions = nullptr;
    bool enabledInterfaces = true;
    //!
    //! \brief ServerPvt
    //! \param server
    //!
    explicit ServerPvt(Server *parent = nullptr) : QObject{parent}//, settingFile{parent}
    {
        this->parent = parent;
        this->listenColletions = new ListenColletions(parent);
    }

    //!
    //! \brief ~ServerPvt
    //!
    virtual ~ServerPvt()
    {
        this->serverStop();
        delete this->listenColletions;
    }

    //!
    //! \brief start
    //! \return
    //!
    bool serverStart()
    {
        if (this->parent->objectName().isEmpty()) {
            static int countServerName = 0;
            auto name = QStringLiteral("Server_%1").arg(++countServerName);
            this->parent->setObjectName(name);
        }

        this->listenColletions->setObjectName(this->parent->objectName() + QStringLiteral("Pvt"));
        if (!this->listenColletions->isRunning())
            return this->listenColletions->start();
        return this->listenColletions->isRunning();
    }

    //!
    //! \brief stop
    //! \return
    //!
    bool serverStop()
    {
        this->listenColletions->quit();
        return false;
    }

    //!
    //! \brief load
    //! \param vSettings
    //! \return
    //!
    bool load(const QVariantHash &vSettings)
    {
        if (!vSettings.contains(__protocol)) {
            rWarning() << tr("Json property [protocol] not detected");
            return false;
        }

        auto protocol = vSettings.value(__protocol).toHash();
        auto controller = vSettings.value(__controller).toHash();

        if (protocol.isEmpty()) {
            rWarning() << tr("Json property [protocol] is empty");
            return false;
        }

        this->listenColletions->setSettings(protocol);
        this->controllerOptions.load(controller);
        return true;
    }

public:
    QVariantHash settings;
    QList<const QMetaObject *> controllers;
    QList<const QMetaObject *> controllerParsers;

    QList<const QMetaObject *> &controllersSort()
    {
        QHash<QString,const QMetaObject *> items;
        for(auto &metaObject:this->controllers)
            items.insert(metaObject->className(), metaObject);
        QStringList keys=items.keys();
        keys.sort();

        QList<const QMetaObject *> sorted;
        for(auto &key:keys)
            sorted.append(items.value(key));
        this->controllers=sorted;
        return this->controllers;
    }
};

Server::Server(QObject *parent) : QObject{parent}
{
    this->p = new ServerPvt{this};
}

bool Server::load(const QVariantHash &settings)
{
    return p->load(settings);
}

ControllerOptions &Server::controllerOptions()
{
    return p->controllerOptions;
}

bool Server::isFinished() const
{
    return p->listenColletions->isFinished();
}

bool Server::isRunning() const
{
    return p->listenColletions->isRunning();
}

bool Server::start()
{
    this->controllers().clear();
    for (auto &v : Controller::staticApiList())
        this->controllers().append(v);

    this->controllerParsers().clear();
    for (auto &v : Controller::staticApiParserList())
        this->controllerParsers().append(v);

    return p->serverStart();
}

bool Server::stop()
{
    return p->listenColletions->quit();
}

Server &Server::interfacesStart()
{
    p->enabledInterfaces = true;
    return *this;
}

Server &Server::interfacesStop()
{
    p->enabledInterfaces = false;
    return *this;
}

bool Server::interfacesRunning()
{
    return p->enabledInterfaces;
}

QList<const QMetaObject *> &Server::controllers()
{
    return p->controllersSort();
}

QList<const QMetaObject *> &Server::controllerParsers()
{
    return p->controllerParsers;
}

void Server::requestEnable()
{
    p->listenColletions->requestEnabled();
}

void Server::requestDisable()
{
    p->listenColletions->requestEnabled();
}

ListenColletions &Server::colletions()
{
    return *p->listenColletions;
}

bool Server::authorizationRequest(const ListenRequest &request) const
{
    Q_UNUSED(request)
    return true;
}

QString &Server::serverName() const
{
    if(p->serverName.trimmed().isEmpty())
        p->serverName=this->metaObject()->className();
    return p->serverName;
}

void Server::setServerName(const QString &value)
{
    p->serverName = value;
}

} // namespace QRpc
