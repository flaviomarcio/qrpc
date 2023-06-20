#pragma once

#include "../../qstm/src/qstm_startup.h"
#include "./qrpc_global.h"

namespace QRpc {

#define Q_RPC_STARTUP_FUNCTION(FUNC) Q_STM_STARTUP_APPEND(QRpc, FUNC)

//!
//! \brief The StartUp class
//!
class Q_RPC_EXPORT StartUp : public QStm::StartUp
{
    Q_OBJECT
    Q_STM_STARTUP_CONSTUCTOR(1)
};

} // namespace QRpc
