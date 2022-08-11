#pragma once

#include "./qrpc_global.h"
#include "../../qstm/src/qstm_setting_base.h"
#include <QVariant>
#include <QVariantHash>

namespace QRpc {

//!
//! \brief The ControllerSetting class
//!
class Q_RPC_EXPORT ControllerSetting:public QStm::SettingBase
{
    Q_OBJECT
    QSTM_SETTING_OBJECT(ControllerSetting)
    Q_PROPERTY(QStringList notMethodCanAuthorization MEMBER notMethodCanAuthorization NOTIFY notMethodCanAuthorizationChanged)
public:
    //!
    //! \brief ControllerSetting
    //! \param parent
    //!
    Q_INVOKABLE explicit ControllerSetting(QObject *parent=nullptr):QStm::SettingBase{parent}
    {
    }
    ~ControllerSetting()
    {
    }
signals:
    void notMethodCanAuthorizationChanged();
public:
    QStringList notMethodCanAuthorization;
};


}

