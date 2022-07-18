#pragma once

#include "../qrpc_listen.h"
#include "../qrpc_macro.h"
#include <QSqlDriver>

namespace QRpc {

//!
//! \brief The ListenBrokerDataBase class
//!
class Q_RPC_EXPORT ListenBrokerDataBase : public Listen
{
    Q_OBJECT
public:
    //!
    //! \brief ListenBrokerDataBase
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenBrokerDataBase(QObject *parent = nullptr);

    //!
    //! \brief start
    //! \return
    //!
    bool start() override;

    //!
    //! \brief stop
    //! \return
    //!
    bool stop() override;

private:
    void *p = nullptr;
signals:
    //!
    //! \brief rpcResponseClient
    //! \param sqlDriver
    //! \param requestPath
    //! \param responseBody
    //!
    void rpcResponseClient(QSqlDriver *sqlDriver,
                           const QString &requestPath,
                           const QVariantHash &responseBody);
};

//QRPC_LISTTEN_AUTO_REGISTER(Database, ListenBrokerDataBase)

} // namespace QRpc
