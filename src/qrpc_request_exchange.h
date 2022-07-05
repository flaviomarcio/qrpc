#pragma once

#include "./qrpc_global.h"
#include "./qrpc_request_exchange_setting.h"

namespace QRpc {
class RequestExchangePvt;
//!
//! \brief The RequestExchange class
//!
class Q_RPC_EXPORT RequestExchange: public QObject
{
    Q_OBJECT
public:

    //!
    //! \brief RequestExchange
    //! \param parent
    //!
    Q_INVOKABLE explicit RequestExchange(QObject *parent=nullptr);
    explicit RequestExchange(RequestExchange &exchange, QObject *parent);

    //!
    //! \brief operator =
    //! \param e
    //! \return
    //!
    RequestExchange &operator =(RequestExchange &e);

    //!
    //! \brief clear
    //! \return
    //!
    virtual RequestExchange &clear();

    //!
    //! \brief call
    //! \return
    //!
    virtual RequestExchangeSetting &call();

    //!
    //! \brief back
    //! \return
    //!
    virtual RequestExchangeSetting &back();

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantMap toMap() const;

    //!
    //! \brief toHash
    //! \return
    //!
    virtual QVariantHash toHash() const;

    //!
    //! \brief print
    //! \param output
    //! \return
    //!
    virtual RequestExchange &print(const QString &output=QString());

    //!
    //! \brief printOut
    //! \param output
    //! \return
    //!
    virtual QStringList printOut(const QString &output=QString());

    //!
    //! \brief setProtocol
    //! \param value
    //!
    virtual void setProtocol(const QVariant &value);
    virtual void setProtocol(const Protocol &value);

    //!
    //! \brief setMethod
    //! \param value
    //!
    virtual void setMethod(const QString &value);
    virtual void setMethod(const RequestMethod &value);

    //!
    //! \brief setHostName
    //! \param value
    //!
    virtual void setHostName(const QString &value);

    //!
    //! \brief setRoute
    //! \param value
    //!
    virtual void setRoute(const QVariant &value);

    //!
    //! \brief setPort
    //! \param value
    //!
    virtual void setPort(const int &value);

private:
    RequestExchangePvt *p = nullptr;
};

}
