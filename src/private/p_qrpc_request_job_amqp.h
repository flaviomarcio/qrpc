#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include <QVariantHash>

namespace QRpc {

class RequestJobAMQP : public RequestJobProtocol
{
    Q_OBJECT
public:

    RequestJobResponse*response=nullptr;

    Q_INVOKABLE explicit RequestJobAMQP(QObject *parent);

public slots:

};
}
