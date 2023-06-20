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
    explicit RequestExchange(QObject *parent=nullptr);
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
    //! \brief setProtocol
    //! \param value
    //!
    RequestExchange &setProtocol(const QVariant &value);
    RequestExchange &setProtocol(const Protocol &value);
    RequestExchange &protocol(const QVariant &value){return this->setProtocol(value);}
    RequestExchange &protocol(const Protocol &value){return this->setProtocol(value);}

    //!
    //! \brief setMethod
    //! \param value
    //!
    RequestExchange &setMethod(const QString &value);
    RequestExchange &setMethod(const RequestMethod &value);
    RequestExchange &method(const QString &value){ return this->setMethod(value);}
    RequestExchange &method(const RequestMethod &value){ return this->setMethod(value);}

    //!
    //! \brief setHostName
    //! \param value
    //!
    RequestExchange &setHostName(const QString &value);
    RequestExchange &hostName(const QString &value){ return this->setHostName(value);}

    //!
    //! \brief setRoute
    //! \param value
    //!
    RequestExchange &setRoute(const QVariant &value);
    RequestExchange &route(const QVariant &value){ return this->setRoute(value);}

    //!
    //! \brief setPort
    //! \param value
    //!
    RequestExchange &setPort(int value);
    RequestExchange &port(int value){ return this->setPort(value);}

private:
    RequestExchangePvt *p = nullptr;
};

}
