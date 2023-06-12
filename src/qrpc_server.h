#pragma once

#include "../../qstm/src/qstm_setting_manager.h"
#include "./qrpc_global.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_request.h"

namespace QRpc {
class ServerPvt;
//!
//! \brief The Server class
//!
class Q_RPC_EXPORT Server : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief Server
    //! \param parent
    //!
    Q_INVOKABLE explicit Server(QObject *parent = nullptr);

    //!
    //! \brief load
    //! \param settings
    //! \return
    //!
    bool load(const QVariantHash &settings);

    //!
    //! \brief settings
    //! \return
    //!
    virtual QStm::SettingManager &settings() const;
    virtual const QStm::SettingBase &settings(const QString &settingName);

    //!
    //! \brief isRunning
    //! \return
    //!
    Q_INVOKABLE virtual bool isRunning() const;

    //!
    //! \brief start
    //! \return
    //!
    Q_INVOKABLE virtual bool start();

    //!
    //! \brief stop
    //! \return
    //!
    Q_INVOKABLE virtual bool stop();

    //!
    //! \brief interfacesStart
    //! \return
    //!
    virtual Server &interfacesStart();

    //!
    //! \brief interfacesStop
    //! \return
    //!
    virtual Server &interfacesStop();

    //!
    //! \brief interfacesRunning
    //! \return
    //!
    Q_INVOKABLE virtual bool interfacesRunning();

    //!
    //! \brief controllers
    //! \return
    //!
    virtual QList<const QMetaObject *> &controllers();

    //!
    //! \brief controllerParsers
    //! \return
    //!
    virtual QList<const QMetaObject *> &controllerParsers();

    //!
    //! \brief colletions
    //! \return
    //!
    virtual ListenColletions &colletions();

    //!
    //! \brief authorizationRequest
    //! \param request
    //! \return
    //!
    virtual bool authorizationRequest(const ListenRequest &request) const;

    //!
    //! \brief serverName
    //! \return
    //!
    Q_INVOKABLE virtual QString &serverName() const;

    //!
    //! \brief setServerName
    //! \param value
    //!
    Q_INVOKABLE virtual Server &setServerName(const QString &value);

private:
    ServerPvt *p = nullptr;
};

//typedef QRpc::Server QRPCServer;

} // namespace QRpc
