#include "./p_qrpc_request_job_response.h"

namespace QRpc {

RequestJobResponse::RequestJobResponse(QObject *parent):QObject{parent}
{
}

RequestJobResponse::RequestJobResponse(const QVariantHash &request_header, const QVariant &vUrl, Request &request, QObject *parent):QObject{parent}
{
    this->request_url = vUrl;
    this->request_header = request_header;
    this->request_body = request.body().body().toByteArray();
    this->request_parameters = request.body().toHash();
    this->activityLimit = request.activityLimit();
    this->request_exchange = request.exchange();
}

RequestJobResponse::~RequestJobResponse()
{
}

RequestJobResponse &RequestJobResponse::operator =(RequestJobResponse &e)
{
    this->request_exchange=e.request_exchange;
    this->qrpcRequest=e.qrpcRequest;
    this->request_url=e.request_url;
    this->request_header=e.request_header;
    this->request_body=e.request_body;
    this->activityLimit=(e.activityLimit>0)?e.activityLimit:this->activityLimit;
    this->request_start=e.request_start;
    this->response_status_code=e.response_status_code;
    this->response_status_reason_phrase=e.response_status_reason_phrase;
    this->response_body=e.response_body;
    this->response_qt_status_code=e.response_qt_status_code;
    this->responseHeader=e.responseHeader;
    return *this;
}

void RequestJobResponse::clear()
{
    this->request_exchange.clear();
    this->qrpcRequest=nullptr;

    this->request_uuid=QUuid::createUuidV3(QUuid::createUuid(), QUuid::createUuid().toString().toUtf8());
    this->request_header.clear();
    this->request_parameters.clear();
    this->request_body.clear();
    this->activityLimit=120000;
    this->request_start=QDateTime();
    this->request_finish=QDateTime();
    this->response_status_code = QNetworkReply::NoError;
    this->response_status_reason_phrase.clear();
    this->response_body.clear();
    this->response_qt_status_code = QNetworkReply::NoError;
    this->responseHeader.clear();
}

QVariantHash RequestJobResponse::toMapResquest()
{
    auto method=this->request_exchange.call().method();
    return Util::toMapResquest(method,request_url,request_body,request_parameters,response_body,request_header,request_start,request_finish);
}

QVariantHash RequestJobResponse::toMapResponse()
{
    QVariantHash map;
    Q_DECLARE_VU;
    map.insert(QStringLiteral("finish"), QDateTime::currentDateTime());
    map.insert(QStringLiteral("header"), this->responseHeader);
    map.insert(QStringLiteral("status_code"), this->response_status_code);
    map.insert(QStringLiteral("qt_status_code"), this->response_qt_status_code);
    map.insert(QStringLiteral("status_reason_phrase"), this->response_status_reason_phrase);
    map.insert(QStringLiteral("body"), vu.toVariant(this->response_body));
    return map;
}

QVariantHash RequestJobResponse::toVariant()
{
    auto rpclog = QVariantList{QVariantHash{{QStringLiteral("resquest"), this->toMapResquest()}},QVariantHash{ {QStringLiteral("response"), this->toMapResponse()}}};
    ///*{"openapi", this->toMapOpenAPI()}*,/
    return qvh({{QStringLiteral("rpclog"), rpclog}});
}



}
