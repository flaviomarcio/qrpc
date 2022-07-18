#pragma once

#include "./qrpc_global.h"
#include <QObject>
#include <QVariant>
#include <QSettings>

namespace QRpc {
class Listen;
class ListenProtocolPvt;
//!
//! \brief The ListenProtocol class
//!
class Q_RPC_EXPORT ListenProtocol : public QObject
{
    Q_OBJECT
    friend class ListenColletions;
    friend class ListenColletionsPvt;

public:
    Q_PROPERTY(int protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QByteArray protocolName READ protocolName NOTIFY protocolNameChanged)
    Q_PROPERTY(QByteArray optionName READ optionName WRITE setOptionName NOTIFY optionNameChanged)
    Q_PROPERTY(int cleanupInterval READ cleanupInterval WRITE setCleanupInterval NOTIFY
                   cleanupIntervalChanged)
    Q_PROPERTY(int minThreads READ minThreads WRITE setMinThreads NOTIFY minThreadsChanged)
    Q_PROPERTY(int maxThreads READ maxThreads WRITE setMaxThreads NOTIFY maxThreadsChanged)
    Q_PROPERTY(
        int maxRequestSize READ maxRequestSize WRITE setMaxRequestSize NOTIFY maxRequestSizeChanged)
    Q_PROPERTY(int maxMultiPartSize READ maxMultiPartSize WRITE setMaxMultiPartSize NOTIFY
                   maxMultiPartSizeChanged)
    Q_PROPERTY(QByteArray driver READ driver WRITE setDriver NOTIFY driverChanged)
    Q_PROPERTY(QByteArray hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(QByteArray userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QByteArray password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QByteArray database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(QByteArray options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QVariantList port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QVariantList queue READ queue WRITE setQueue NOTIFY queueChanged)
    Q_PROPERTY(QByteArray sslKeyFile READ sslKeyFile WRITE setSslKeyFile NOTIFY sslKeyFileChanged)
    Q_PROPERTY(QByteArray sslCertFile READ sslCertFile WRITE setSslCertFile NOTIFY sslCertFileChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool realMessageOnException READ realMessageOnException WRITE
                   setRealMessageOnException NOTIFY realMessageOnExceptionChanged)

    //!
    //! \brief ListenProtocol
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenProtocol(QObject *parent = nullptr);

private:
    //!
    //! \brief ListenProtocol
    //! \param protocol
    //! \param metaObject
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenProtocol(int protocol,
                                        const QMetaObject &metaObject,
                                        QObject *parent);

public:
    //!
    //! \brief isValid
    //! \return
    //!
    virtual bool isValid() const;

    //!
    //! \brief makeListen
    //! \return
    //!
    virtual Listen *makeListen();

    //!
    //! \brief protocol
    //! \return
    //!
    virtual int protocol();

private:
    //!
    //! \brief setProtocol
    //! \param value
    //!
    virtual void setProtocol(const int &value);

public:
    //!
    //! \brief protocolName
    //! \return
    //!
    virtual QByteArray protocolName();

    //!
    //! \brief optionName
    //! \return
    //!
    virtual QByteArray optionName();

private:
    //!
    //! \brief setOptionName
    //! \param value
    //!
    virtual void setOptionName(const QByteArray &value);

public:
    //!
    //! \brief minThreads
    //! \return
    //!
    virtual int minThreads() const;

    //!
    //! \brief setMinThreads
    //! \param value
    //!
    virtual void setMinThreads(int value);

    //!
    //! \brief maxThreads
    //! \return
    //!
    virtual int maxThreads() const;

    //!
    //! \brief setMaxThreads
    //! \param value
    //!
    virtual void setMaxThreads(int value);

    //!
    //! \brief cleanupInterval
    //! \return
    //!default value 10000ms
    virtual int cleanupInterval() const;

    //!
    //! \brief setCleanupInterval
    //! \param value
    //!
    virtual void setCleanupInterval(int value);

    //!
    //! \brief maxRequestSize
    //! \return
    //!
    virtual int maxRequestSize() const;

    //!
    //! \brief setMaxRequestSize
    //! \param value
    //!
    virtual void setMaxRequestSize(int value);

    //!
    //! \brief maxMultiPartSize
    //! \return
    //!
    virtual int maxMultiPartSize() const;

    //!
    //! \brief setMaxMultiPartSize
    //! \param value
    //!
    virtual void setMaxMultiPartSize(int value);

    //!
    //! \brief driver
    //! \return
    //!
    virtual QByteArray driver() const;

    //!
    //! \brief setDriver
    //! \param value
    //!
    virtual void setDriver(const QByteArray &value);

    //!
    //! \brief hostName
    //! \return
    //!
    virtual QByteArray hostName() const;

    //!
    //! \brief setHostName
    //! \param value
    //!
    virtual void setHostName(const QByteArray &value);

    //!
    //! \brief userName
    //! \return
    //!
    virtual QByteArray userName() const;

    //!
    //! \brief setUserName
    //! \param value
    //!
    virtual void setUserName(const QByteArray &value);

    //!
    //! \brief password
    //! \return
    //!
    virtual QByteArray password() const;

    //!
    //! \brief setPassword
    //! \param value
    //!
    virtual void setPassword(const QByteArray &value);

    //!
    //! \brief database
    //! \return
    //!
    virtual QByteArray database() const;

    //!
    //! \brief setDatabase
    //! \param value
    //!
    virtual void setDatabase(const QByteArray &value);

    //!
    //! \brief options
    //! \return
    //!
    virtual QByteArray options() const;

    //!
    //! \brief setOptions
    //! \param value
    //!
    virtual void setOptions(const QByteArray &value);

    //!
    //! \brief queue
    //! \return
    //!
    virtual QVariantList &queue();

    //!
    //! \brief setQueue
    //! \param value
    //!
    virtual void setQueue(const QByteArray &value);

    //!
    //! \brief setQueue
    //! \param value
    //!
    virtual void setQueue(const QVariantList &value);

    //!
    //! \brief port
    //! \return
    //!
    virtual QVariantList port() const;

    //!
    //! \brief setPort
    //! \param value
    //!
    virtual void setPort(const QVariant &value);

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantMap toMap() const;

    //!
    //! \brief toHash
    //! \return
    //!
    virtual QVariantHash &toHash() const;

    //!
    //! \brief settings
    //! \return
    //!
    virtual QSettings &settings() const;

    //!
    //! \brief setSettings
    //! \param settings
    //! \param defaultSettings
    //!
    virtual void setSettings(const QVariantHash &settings, const QVariantHash &defaultSettings);

    //!
    //! \brief makeSettings
    //! \param parent
    //! \return
    //!
    virtual QSettings *makeSettings(QObject *parent = nullptr);

    //!
    //! \brief makeSettingsHash
    //! \return
    //!
    virtual QVariantHash makeSettingsHash() const;

    //!
    //! \brief enabled
    //! \return
    //!
    virtual bool enabled() const;

    //!
    //! \brief setEnabled
    //! \param value
    //!
    virtual void setEnabled(bool value);

    //!
    //! \brief sslKeyFile
    //! \return
    //!
    virtual QByteArray sslKeyFile() const;
    virtual void setSslKeyFile(const QByteArray &value);

    //!
    //! \brief sslCertFile
    //! \return
    //!
    virtual QByteArray sslCertFile() const;

    //!
    //! \brief setSslCertFile
    //! \param value
    //!
    virtual void setSslCertFile(const QByteArray &value);

    //!
    //! \brief realMessageOnException
    //! \return
    //!true on debug
    virtual bool realMessageOnException() const;

    //!
    //! \brief setRealMessageOnException
    //! \param value
    //!
    virtual void setRealMessageOnException(bool value);

private:
    ListenProtocolPvt *p = nullptr;
signals:
    void protocolChanged();
    void protocolNameChanged();
    void optionNameChanged();
    void cleanupIntervalChanged();
    void minThreadsChanged();
    void maxThreadsChanged();
    void maxRequestSizeChanged();
    void maxMultiPartSizeChanged();
    void driverChanged();
    void hostNameChanged();
    void userNameChanged();
    void passwordChanged();
    void databaseChanged();
    void optionsChanged();
    void portChanged();
    void queueChanged();
    void sslKeyFileChanged();
    void sslCertFileChanged();
    void enabledChanged();
    void realMessageOnExceptionChanged();
};

} // namespace QRpc
