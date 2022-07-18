#include "./qrpc_setting_manager.h"

namespace QRpc {

Q_GLOBAL_STATIC(SettingManager, staticSettingManager)

SettingManager &SettingManager::i()
{
    return *staticSettingManager;
}

}
