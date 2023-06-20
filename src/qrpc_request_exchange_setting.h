#pragma once

#include "./qrpc_global.h"
#include "./qrpc_types.h"

namespace QRpc {
class RequestExchangeSettingPvt;
//!
//! \brief The RequestExchangeSetting class
//!
class Q_RPC_EXPORT RequestExchangeSetting: public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString protocol READ protocolName WRITE setProtocol RESET resetProtocol NOTIFY protocolChanged)
    Q_PROPERTY(int method READ ___method WRITE setMethod RESET resetMethod NOTIFY methodChanged )
    Q_PROPERTY(QString vHost READ vHost WRITE setVHost RESET resetVHost NOTIFY vHostChanged )
    Q_PROPERTY(QString driver READ driver WRITE setDriver RESET resetDriver NOTIFY driverChanged )
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName RESET resetHostName NOTIFY hostNameChanged )
    Q_PROPERTY(QString userName READ userName WRITE setUserName RESET resetUserName NOTIFY userNameChanged )
    Q_PROPERTY(QString passWord READ passWord WRITE setPassWord RESET resetPassWord NOTIFY passWordChanged )
    Q_PROPERTY(QVariantHash parameter READ parameter WRITE setParameter RESET resetParameter NOTIFY parameterChanged )
    Q_PROPERTY(QVariant port READ port WRITE setPort RESET resetPort NOTIFY portChanged )
    Q_PROPERTY(QString route READ route WRITE setRoute RESET resetRoute NOTIFY routeChanged )
    Q_PROPERTY(QString topic READ topic WRITE setTopic RESET resetTopic NOTIFY topicChanged )
    Q_PROPERTY(QVariant activityLimit READ activityLimit WRITE setActivityLimit RESET resetActivityLimit NOTIFY activityLimitChanged )

    //!
    //! \brief RequestExchangeSetting
    //! \param parent
    //!
    explicit RequestExchangeSetting(QObject *parent=nullptr);

    //!
    //! \brief RequestExchangeSetting
    //! \param e
    //! \param parent
    //!
    explicit RequestExchangeSetting(const RequestExchangeSetting &e, QObject *parent=nullptr);

    //!
    //! \brief operator =
    //! \param e
    //! \return
    //!
    RequestExchangeSetting &operator=(const RequestExchangeSetting &e);
    RequestExchangeSetting &operator=(const QVariantHash &e);

    //!
    //! \brief clear
    //! \return
    //!
    RequestExchangeSetting &clear();

    //!
    //! \brief toHash
    //! \return
    //!
    const QVariantHash &toHash() const;

    //!
    //! \brief url
    //! \return
    //!
    const QString &url() const;

    //!
    //! \brief isValid
    //! \return
    //!
    bool isValid() const;

    //!
    //! \brief print
    //! \param output
    //! \return
    //!
    RequestExchangeSetting &print(const QString &output={});

    //!
    //! \brief printOut
    //! \param output
    //! \return
    //!
    QStringList printOut(const QString &output={});

    //!
    //! \brief method
    //! \return
    //!
    RequestMethod method() const;
    RequestExchangeSetting &setMethod(const int &value);
    RequestExchangeSetting &setMethod(const QString &value);
    RequestExchangeSetting &resetMethod(){ return this->setMethod({});}
    RequestExchangeSetting &method(const int &value){ return this->setMethod(value);}
    RequestExchangeSetting &method(const QString &value){ return this->setMethod(value);}
    const QString &methodName() const;

    //!
    //! \brief protocol
    //! \return
    //!
    Protocol protocol() const;
    RequestExchangeSetting &setProtocol(const Protocol &value);
    RequestExchangeSetting &setProtocol(const QVariant &value);
    RequestExchangeSetting &resetProtocol(){ return this->setProtocol({});}
    RequestExchangeSetting &protocol(const Protocol &value){ return this->setProtocol(value);}
    RequestExchangeSetting &protocol(const QVariant &value){ return this->setProtocol(value);}
    const QString &protocolName() const;
    const QString &protocolUrlName() const;

    //!
    //! \brief driver
    //! \return
    //!
    const
        QString &driver() const;
    RequestExchangeSetting &setDriver(const QString &value);
    RequestExchangeSetting &resetDriver(){ return this->setDriver({});}
    RequestExchangeSetting &driver(const QString &value){ return this->setDriver(value);}

    //!
    //! \brief hostName
    //! \return
    //!
    const QString &hostName() const;
    RequestExchangeSetting &setHostName(const QString &value);
    RequestExchangeSetting &resetHostName(){ return this->setHostName({});}
    RequestExchangeSetting &hostName(const QString &value){ return this->setHostName(value);}

    //!
    //! \brief vHost
    //! \return
    //!
    const QString &vHost() const;
    RequestExchangeSetting &setVHost(const QString &value);
    RequestExchangeSetting &resetVHost(){ return this->setVHost({});}
    RequestExchangeSetting &vHost(const QString &value){ return this->setVHost(value);}

    //!
    //! \brief userName
    //! \return
    //!
    const QString &userName() const;
    RequestExchangeSetting &setUserName(const QString &value);
    RequestExchangeSetting &resetUserName(){ return this->setUserName({});}
    RequestExchangeSetting &userName(const QString &value){ return this->setUserName(value);}

    //!
    //! \brief passWord
    //! \return
    //!
    const QString &passWord() const;
    RequestExchangeSetting &setPassWord(const QString &value);
    RequestExchangeSetting &resetPassWord(){ return this->setPassWord({});}
    RequestExchangeSetting &passWord(const QString &value){ return this->setPassWord(value);}

    //!
    //! \brief route
    //! \return
    //!
    const QString &route() const;
    RequestExchangeSetting &setRoute(const QVariant &value);
    RequestExchangeSetting &resetRoute(){ return this->setRoute({});}
    RequestExchangeSetting &route(const QString &value){ return this->setRoute(value);}

    //!
    //! \brief topic
    //! \return
    //!
    const QString &topic() const;
    RequestExchangeSetting &setTopic(const QString &value);
    RequestExchangeSetting &resetTopic(){ return this->setTopic({});}
    RequestExchangeSetting &topic(const QString &value){ return this->setTopic(value);}

    //!
    //! \brief port
    //! \return
    //!
    const QVariant &port() const;
    RequestExchangeSetting &setPort(const QVariant &value);
    RequestExchangeSetting &resetPort(){ return this->setPort({});}
    RequestExchangeSetting &port(const QVariant &value){ return this->setPort(value);}

    //!
    //! \brief activityLimit
    //! \return
    //!
    qlonglong activityLimit() const;
    RequestExchangeSetting &setActivityLimit(const QVariant &value);
    RequestExchangeSetting &resetActivityLimit(){ return this->setActivityLimit({});}
    RequestExchangeSetting &activityLimit(const QVariant &value){ return this->setActivityLimit(value);}

    //!
    //! \brief parameter
    //! \return
    //!
    const QVariantHash &parameter() const;
    RequestExchangeSetting &setParameter(const QVariantHash &value);
    RequestExchangeSetting &resetParameter(){ return this->setParameter({});}
    RequestExchangeSetting &parameter(const QVariantHash &value){ return this->setParameter(value);}

signals:
    void protocolChanged();
    void methodChanged();
    void vHostChanged();
    void driverChanged();
    void hostNameChanged();
    void userNameChanged();
    void passWordChanged();
    void parameterChanged();
    void portChanged();
    void routeChanged();
    void topicChanged();
    void activityLimitChanged();

private:
    RequestExchangeSettingPvt *p = nullptr;
    int ___method() const{ return this->method();}
};

}
