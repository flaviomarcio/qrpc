#pragma once

#include "../../qstm/src/qstm_setting_manager.h"
#include "./qrpc_global.h"
#include "./qrpc_service_setting.h"
#include <QSettings>

namespace QRpc {

//!
//! \brief The SettingManager class
//!
class Q_RPC_EXPORT SettingManager : public QStm::SettingManager
{
    Q_OBJECT
public:
    Q_DECLARE_INSTANCE(SettingManager)
    Q_SETTING_MANAGER_REPLACE_METHODS(SettingManager, ServiceSetting)
};

}
