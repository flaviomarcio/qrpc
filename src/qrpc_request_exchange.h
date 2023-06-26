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
    RequestExchange &operator =(const RequestExchange &e);

    //!
    //! \brief clear
    //! \return
    //!
    RequestExchange &clear();

    //!
    //! \brief call
    //! \return
    //!
    RequestExchangeSetting &call() const;

    //!
    //! \brief back
    //! \return
    //!
    RequestExchangeSetting &back() const;

    //!
    //! \brief toHash
    //! \return
    //!
    QVariantHash toHash() const;

    //!
    //! \brief print
    //! \param output
    //! \return
    //!
    RequestExchange &print(const QString &output={});

    //!
    //! \brief printOut
    //! \param output
    //! \return
    //!
    QStringList printOut(const QString &output={});

    //!
    //! \brief protocol
    //! \return
    //!
    Types::Protocol protocol() const;
    RequestExchange &setProtocol(const QVariant &value);
    RequestExchange &protocol(const QVariant &value){return this->setProtocol(value);}

    //!
    //! \brief method
    //! \return
    //!
    Types::Method method() const;
    RequestExchange &setMethod(const QVariant &value);
    RequestExchange &method(const QString &value){ return this->setMethod(value);}

    //!
    //! \brief hostName
    //! \param value
    //! \return
    //!
    RequestExchange &hostName(const QString &value){ return this->setHostName(value);}
    RequestExchange &setHostName(const QString &value);

    //!
    //! \brief route
    //! \param value
    //! \return
    //!
    RequestExchange &route(const QVariant &value){ return this->setRoute(value);}
    RequestExchange &setRoute(const QVariant &value);

    //!
    //! \brief port
    //! \param value
    //! \return
    //!
    RequestExchange &port(int value){ return this->setPort(value);}
    RequestExchange &setPort(int value);

private:
    RequestExchangePvt *p = nullptr;
};

}
