#pragma once

#include "./qrpc_global.h"
#include "../../qstm/src/qstm_setting_base.h"

namespace QRpc {

//!
//! \brief The ServiceSetting class
//!
class Q_RPC_EXPORT ServiceSetting:public QStm::SettingBase
{
    Q_OBJECT
    QSTM_SETTING_OBJECT(SettingBase)
public:
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(QVariantHash headers READ headers WRITE setHeaders NOTIFY headersChanged)
    Q_PROPERTY(QVariantHash parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
    Q_PROPERTY(QString method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QVariant protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QString driverName READ driverName WRITE setDriverName NOTIFY driverNameChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QVariant route READ route WRITE setRoute NOTIFY routeChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QVariant body READ body WRITE setBody NOTIFY bodyChanged)
    Q_PROPERTY(QVariant cacheInterval READ cacheInterval WRITE setCacheInterval NOTIFY cacheIntervalChanged)
    Q_PROPERTY(bool cacheCleanup READ cacheCleanup WRITE setCacheCleanup NOTIFY cacheCleanupChanged)
    Q_PROPERTY(QVariant cacheCleanupInterval READ cacheCleanupInterval WRITE setCacheCleanupInterval NOTIFY cacheCleanupIntervalChanged)
public:
    //!
    //! \brief ServiceSetting
    //! \param parent
    //!
    Q_INVOKABLE explicit ServiceSetting(QObject *parent=nullptr);
};


}
