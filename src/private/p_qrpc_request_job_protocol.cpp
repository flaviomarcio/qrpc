#include "./p_qrpc_request_job_protocol.h"

namespace QRpc {

RequestJobProtocol::RequestJobProtocol(QObject *parent):QObject{parent}/*, sslConfiguration(QSslConfiguration::defaultConfiguration())*/
{

}

RequestJobProtocol &RequestJobProtocol::clear()
{
    return *this;
}

bool RequestJobProtocol::call(RequestJobResponse *response)
{
    response->response_status_code=QNetworkReply::UnknownServerError;
    response->response_body.clear();
    emit this->callback({});
    return false;
}

}

