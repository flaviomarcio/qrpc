#include "./p_qrpc_request_job_response.h"
#include "./p_qrpc_util.h"

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

RequestJobResponse &RequestJobResponse::operator =(const RequestJobResponse &e)
{
    this->request_exchange=e.request_exchange;
    this->localRequest=e.localRequest;
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
    this->localRequest=nullptr;

    this->request_uuid=QUuid::createUuidV3(QUuid::createUuid(), QUuid::createUuid().toString().toUtf8());
    this->request_header.clear();
    this->request_parameters.clear();
    this->request_body.clear();
    this->activityLimit=120000;
    this->request_start={};
    this->request_finish={};
    this->response_status_code = QNetworkReply::NoError;
    this->response_status_reason_phrase.clear();
    this->response_body.clear();
    this->response_qt_status_code = QNetworkReply::NoError;
    this->responseHeader.clear();
}

QVariantHash RequestJobResponse::toVariant()
{
    static const auto __rpclog="rpclog";
    static const auto __finish="finish";
    static const auto __header="header";
    static const auto __status_code="status_code";
    static const auto __qt_status_code="status_code";
    static const auto __status_reason_phrase="status_reason_phrase";
    static const auto __body="body";

    Q_DECLARE_VU;
    QVariantHash toHashResquest=Util::toHashResquest(this->request_exchange.call().method(),request_url,request_body,request_parameters,response_body,request_header,request_start,request_finish);


    QVariantHash toHashResponse={
            {__finish, QDateTime::currentDateTime()},
            {__header, this->responseHeader},
            {__status_code, this->response_status_code},
            {__qt_status_code, this->response_qt_status_code},
            {__status_reason_phrase, this->response_status_reason_phrase},
            {__body, vu.toVariant(this->response_body)},
        };
    static const auto __resquest="response";
    static const auto __response="resquest";

    QVariantList rpclog =
        {
          QVariantHash{{__resquest, toHashResquest}},
          QVariantHash{{__response, toHashResponse}},
        };
    ///*{"openapi", this->toMapOpenAPI()}*,/
    return {{__rpclog, rpclog}};
}



}
