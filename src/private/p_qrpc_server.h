#pragma once

#include "../qrpc_controller.h"
#include "../qrpc_controller_options.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_server.h"
#include "./p_qrpc_listen_qrpc.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QMutex>

namespace QRpc {

//!
//! \brief The ServerPvt class
//!
class Q_RPC_EXPORT ServerPvt : public QObject
{
public:
    ControllerOptions controllerOptions;
    QString serverName;
    bool enabledInterfaces = true;
    QVariant settingsFileName;
    ListenColletions *listenColletions = nullptr;
    Server *server = nullptr;
    //!
    //! \brief ServerPvt
    //! \param server
    //!
    explicit ServerPvt(Server *server = nullptr);

    //!
    //! \brief ~ServerPvt
    //!
    virtual ~ServerPvt();

    //!
    //! \brief toMd5
    //! \param v
    //! \return
    //!
    static QByteArray toMd5(const QVariant &v);

    //!
    //! \brief start
    //! \return
    //!
    bool serverStart();

    //!
    //! \brief stop
    //! \return
    //!
    bool serverStop();

    //!
    //! \brief v_load
    //! \param v
    //! \return
    //!
    bool v_load(const QVariant &v);

    //!
    //! \brief load
    //! \param settingsFileName
    //! \return
    //!
    bool load(const QStringList &settingsFileName);

    //!
    //! \brief load
    //! \param settingsFileName
    //! \return
    //!
    bool load(const QString &settingsFileName);

    //!
    //! \brief load
    //! \param settings
    //! \return
    //!
    virtual bool load(const QVariantHash &settings) const;

public:
    QByteArray settingsGroup = qbl("RPCServer");
    QVariantHash settings;
    QList<const QMetaObject *> controllers;
    QList<const QMetaObject *> controllerParsers;
};

} // namespace QRpc
