#pragma once

#include "../qrpc_global.h"
#include <QThread>

namespace QRpc {
class ThreadPvt;
//!
//! \brief The QRPCThread class
//!
class Q_RPC_EXPORT Thread : public QThread
{
    Q_OBJECT
public:
    //!
    //! \brief QRPCThread
    //! \param parent
    //!
    Q_INVOKABLE explicit Thread(QObject *parent = nullptr);

    //!
    //! \brief run
    //!
    void run();

public slots:
    //!
    //! \brief eventRun
    //!
    virtual void eventRun();

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
    ThreadPvt *p = nullptr;
};
} // namespace QRpc
