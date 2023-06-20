#pragma once

#include <QObject>
#include "./qrpc_global.h"

namespace QRpc {

//!
//! \brief The LastError class
//!
class Q_RPC_EXPORT LastError : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief LastError
    //! \param parent
    //!
    Q_INVOKABLE explicit LastError(QObject *parent = nullptr);

    //!
    //! \brief nativeErrorCode
    //! \return
    //!
    Q_INVOKABLE virtual QString nativeErrorCode() const;

    //!
    //! \brief text
    //! \return
    //!
    Q_INVOKABLE virtual QString text() const;

    //!
    //! \brief isValid
    //! \return
    //!
    Q_INVOKABLE virtual bool isValid() const;

    //!
    //! \brief clear
    //!
    Q_INVOKABLE virtual void clear();

signals:
};

} // namespace QRpc
