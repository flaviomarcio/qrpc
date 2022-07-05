#pragma once

#include <QStm>
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
    Q_PROPERTY(QString protocol READ protocolName WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(int method READ ___method WRITE setMethod NOTIFY methodChanged )
    Q_PROPERTY(QString vHost READ vHost WRITE setVHost NOTIFY vHostChanged )
    Q_PROPERTY(QString driver READ driver WRITE setDriver NOTIFY driverChanged )
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged )
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged )
    Q_PROPERTY(QString passWord READ passWord WRITE setPassWord NOTIFY passWordChanged )
    Q_PROPERTY(QVariantHash parameter READ parameter WRITE setParameter NOTIFY parameterChanged )
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged )
    Q_PROPERTY(QString route READ route WRITE setRoute NOTIFY routeChanged )
    Q_PROPERTY(QString topic READ topic WRITE setTopic NOTIFY topicChanged )
    Q_PROPERTY(QVariant activityLimit READ activityLimit WRITE setActivityLimit NOTIFY activityLimitChanged )

    //!
    //! \brief RequestExchangeSetting
    //! \param parent
    //!
    Q_INVOKABLE explicit RequestExchangeSetting(QObject *parent=nullptr);

    //!
    //! \brief RequestExchangeSetting
    //! \param e
    //! \param parent
    //!
    explicit RequestExchangeSetting(RequestExchangeSetting &e, QObject *parent=nullptr);

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
    virtual RequestExchangeSetting &clear();

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantMap toMap() const;

    /**
     * @brief toMap
     * @return
     */
    virtual QVariantHash toHash() const;

    //!
    //! \brief url
    //! \return
    //!
    virtual QString url() const;

    //!
    //! \brief isValid
    //! \return
    //!
    virtual bool isValid() const;

    //!
    //! \brief print
    //! \param output
    //! \return
    //!
    virtual RequestExchangeSetting &print(const QString &output=QString());

    //!
    //! \brief printOut
    //! \param output
    //! \return
    //!
    virtual QStringList printOut(const QString &output=QString());

    //!
    //! \brief method
    //! \return
    //!
    virtual RequestMethod method() const;
    virtual void setMethod(const int &value);
    virtual void setMethod(const QString &value);

    //!
    //! \brief methodName
    //! \return
    //!
    virtual QString methodName() const;

    //!
    //! \brief protocol
    //! \return
    //!
    virtual Protocol protocol() const;
    virtual QString protocolName() const;
    virtual QString protocolUrlName() const;
    virtual void setProtocol(const Protocol &value);
    virtual void setProtocol(const QVariant &value);

    //!
    //! \brief driver
    //! \return
    //!
    virtual QString &driver() const;
    virtual void setDriver(const QString &value);

    //!
    //! \brief hostName
    //! \return
    //!
    virtual QString &hostName() const;
    virtual void setHostName(const QString &value);

    //!
    //! \brief vHost
    //! \return
    //!
    virtual QString &vHost() const;
    virtual void setVHost(const QString &value);

    //!
    //! \brief userName
    //! \return
    //!
    virtual QString &userName() const;
    virtual void setUserName(const QString &value);

    //!
    //! \brief passWord
    //! \return
    //!
    virtual QString &passWord() const;
    virtual void setPassWord(const QString &value);

    //!
    //! \brief route
    //! \return
    //!
    virtual QString &route() const;
    virtual void setRoute(const QVariant &value);

    //!
    //! \brief topic
    //! \return
    //!
    virtual QString &topic() const;
    virtual void setTopic(const QString &value);

    //!
    //! \brief port
    //! \return
    //!
    virtual int port() const;
    virtual void setPort(int port);

    //!
    //! \brief activityLimit
    //! \return
    //!
    virtual qlonglong activityLimit() const;
    virtual void setActivityLimit(const QVariant &value);

    //!
    //! \brief parameter
    //! \return
    //!
    virtual QVariantHash &parameter() const;
    virtual void setParameter(const QVariantHash &parameter);

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
