#include "./qrpc_server.h"
#include "./private/p_qrpc_server.h"

namespace QRpc {

Server::Server(QObject *parent) : QObject{parent}
{
    this->p = new ServerPvt{this};
}

Server::Server(const QVariant &settings, QObject *parent) : QObject{parent}
{
    this->p = new ServerPvt{this};
    p->v_load(settings);
}

Server::~Server() {}

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
    return p->controllers;
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

QVariant Server::settingsFileName()
{
    return p->settingsFileName;
}

bool Server::setSettingsFileName(const QString &fileName)
{
    return p->v_load(qvsl{fileName});
}

bool Server::setSettings(const QVariant &settings) const
{
    return p->v_load(settings);
}

bool Server::authorizationRequest(const ListenRequest &request) const
{
    Q_UNUSED(request)
    return true;
}

QString &Server::serverName() const
{
    return p->serverName;
}

void Server::setServerName(const QString &value)
{
    p->serverName = value;
}

} // namespace QRpc
