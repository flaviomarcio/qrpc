#pragma once

#include "./qrpc_controller.h"

namespace QRpc {

/**
 * @brief The QRPCInterfaceCheck class
 *
 * GET
 * http://localhost/RequestPath/ping
 *
 * POST/PUT
 * http://localhost/RequestPath/fullCheck
 * http://localhost/RequestPath/connectionsCheck
 * http://localhost/RequestPath/businessCheck
 *
 */
class QRPCInterfaceCheck : public QRpc::QRPCController
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCInterfaceCheck(QObject *parent = nullptr);
    Q_INVOKABLE ~QRPCInterfaceCheck();

    Q_INVOKABLE virtual QVariant ping();
    Q_INVOKABLE virtual QVariant fullCheck();
    Q_INVOKABLE virtual QVariant connectionsCheck();
    Q_INVOKABLE virtual QVariant businessCheck();
signals:
};

}
