#pragma once

#include <QDebug>
#include <QList>
#include "../qrpc_controller_options.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_server.h"

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
    QByteArray settingsGroup = QByteArrayLiteral("RPCServer");
    QVariantHash settings;
    QList<const QMetaObject *> controllers;
    QList<const QMetaObject *> controllerParsers;

    QList<const QMetaObject *> &controllersSort();
};

} // namespace QRpc
