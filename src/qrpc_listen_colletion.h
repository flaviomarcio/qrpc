#pragma once

#include "./qrpc_global.h"
#include "./qrpc_types.h"
#include <QThread>
#include <QVariant>
#include <QVariantHash>

class QSettings;

namespace QRpc {
class ListenProtocol;
class ListenQRPC;
class Server;
class Listen;
class ListenColletionsPvt;

//!
//! \brief ListenProtocols
//!
typedef QHash<int, ListenProtocol *> ListenProtocols;

//!
//! \brief The ListenColletions class
//!
class Q_RPC_EXPORT ListenColletions : public QThread
{
    Q_OBJECT
public:
    //!
    //! \brief ListenColletions
    //! \param parent
    //!
    explicit ListenColletions(Server *parent = nullptr);

    //!
    //! \brief ListenColletions
    //! \param settings
    //! \param server
    //!
    explicit ListenColletions(const QVariantHash &settings, Server *server = nullptr);

    //!
    //! \brief protocol
    //! \return
    //!
    virtual ListenProtocol &protocol();

    //!
    //! \brief protocol
    //! \param protocol
    //! \return
    //!
    virtual ListenProtocol &protocol(const QRpc::Protocol &protocol);

    //!
    //! \brief protocols
    //! \return
    //!
    virtual ListenProtocols &protocols();

    //!
    //! \brief run
    //!
    void run()override;

    //!
    //! \brief requestEnabled
    //!
    virtual void requestEnabled();

    //!
    //! \brief requestDisable
    //!
    virtual void requestDisable();

    //!
    //! \brief server
    //! \return
    //!
    virtual Server *server();

    //!
    //! \brief setSettings
    //! \param settings
    //!
    virtual void setSettings(const QVariantHash &settings) const;

    //!
    //! \brief listenPool
    //! \return
    //!
    virtual ListenQRPC *listenPool();

public slots:

    //!
    //! \brief start
    //! \return
    //!
    virtual bool start();

    //!
    //! \brief stop
    //! \return
    //!
    virtual bool stop();

    //!
    //! \brief quit
    //! \return
    //!
    virtual bool quit();

private:
    ListenColletionsPvt *p = nullptr;
signals:
};
} // namespace QRpc
