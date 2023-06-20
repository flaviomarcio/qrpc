#include "./p_qrpc_http_response.h"
#include "./p_qrpc_listen_request_code.h"
#include "./p_qrpc_request_job_response.h"
#include "../qrpc_macro.h"
#include "../../qstm/src/qstm_util_variant.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtCborCommon>
#include <QCborStreamReader>
#include <QCborStreamWriter>
#include <QCborArray>
#include <QCborMap>
#include <QCborValue>
#include <QCborParserError>

namespace QRpc {

class HttpResponsePvt:public QObject{
public:
    HttpResponse *parent=nullptr;
    QRpc::HttpHeaders response_header;
    int response_status_code=0;
    QNetworkReply::NetworkError response_qt_status_code=QNetworkReply::NoError;
    QString response_reason_phrase;
    QByteArray response_body;
    explicit HttpResponsePvt(HttpResponse *parent): QObject{parent}, response_header{parent}
    {
    }
};

HttpResponse::HttpResponse(QObject *parent):QObject{parent}, p{new HttpResponsePvt{this}}
{
}

HttpHeaders &HttpResponse::header() const
{
    return p->response_header;
}

void HttpResponse::setBody(const QVariant &vBody)
{
    auto typeId=vBody.typeId();
    switch (typeId) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    {
        if(this->header().isContentType(AppJson)){
            auto doc=QJsonDocument::fromVariant(vBody);
            p->response_body=doc.toJson(doc.Compact);
            break;
        }

        if(this->header().isContentType(AppCBOR) || this->header().isContentType(AppOctetStream)){
            auto doc=QCborValue::fromVariant(vBody);
            p->response_body=doc.toByteArray();
            break;
        }
        auto doc=QJsonDocument::fromVariant(vBody);
        p->response_body=doc.toJson(doc.Compact);
        break;
    }
    case QMetaType::QString:
    case QMetaType::QByteArray:
    {
        if(this->header().isContentType(AppJson)){
            auto doc=QJsonDocument::fromJson(vBody.toByteArray());
            p->response_body=doc.toJson(doc.Compact);
            break;
        }

        if(this->header().isContentType(AppCBOR) || this->header().isContentType(AppOctetStream)){
            auto doc=QCborValue::fromCbor(vBody.toByteArray());
            p->response_body=doc.toByteArray();
            break;
        }

        auto doc=QJsonDocument::fromJson(vBody.toByteArray());
        p->response_body=doc.toJson(doc.Compact);
        break;
    }
    default:
        p->response_body=vBody.toByteArray();
    }
}

QByteArray &HttpResponse::body() const
{
    return p->response_body;
}

QVariant HttpResponse::bodyVariant() const
{
    QJsonParseError*error=nullptr;
    auto body=p->response_body.trimmed();
    if(body.isEmpty()){
        return {};
    }

    if(this->header().isContentType(AppJson)){
        auto vdoc=QJsonDocument::fromJson(p->response_body, error).toVariant();
        if(!vdoc.isNull() && !vdoc.isValid())
            return vdoc;
    }
    else if(this->header().isContentType(AppCBOR) || this->header().isContentType(AppOctetStream)){
        auto vdoc=QCborValue::fromVariant(p->response_body).toVariant();
        if(!vdoc.isNull() && !vdoc.isValid())
            return vdoc;
    }

    {
        auto body=p->response_body.trimmed();
        if(body.at(0)=='[' || body.at(0)=='{'){
            auto doc=QJsonDocument::fromJson(p->response_body, error);
            if(!doc.isNull() && (doc.isArray() || doc.isObject()))
                return doc.toVariant();
        }
        else{
            auto doc=QCborValue::fromVariant(p->response_body);
            if(!doc.isNull() && (doc.isArray() || doc.isMap()))
                return doc.toVariant();
        }
    }
    return p->response_body;
}

QVariantMap HttpResponse::bodyMap() const
{
    return this->bodyVariant().toMap();
}

QVariantHash HttpResponse::bodyHash() const
{
    return this->bodyVariant().toHash();
}

QVariantHash HttpResponse::bodyObject() const
{
    return this->bodyVariant().toHash();
}

QVariantList HttpResponse::bodyList() const
{
    return this->bodyVariant().toList();
}

QVariantList HttpResponse::bodyArray() const
{
    return this->bodyVariant().toList();
}

QVariantList HttpResponse::bodyToList() const
{
    auto v=this->bodyVariant();
    switch (v.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return v.toList();
    default:
        if(v.isValid())
            return QVariantList{v};
        return {};
    }
}

int &HttpResponse::statusCode() const
{
    return p->response_status_code;
}

QString &HttpResponse::reasonPhrase() const
{
    return p->response_reason_phrase;
}

QNetworkReply::NetworkError &HttpResponse::qtStatusCode() const
{
    return p->response_qt_status_code;
}

QVariantHash HttpResponse::toHash() const
{
    return QJsonDocument::fromJson(p->response_body).object().toVariantHash();
}

QVariantHash HttpResponse::toResponse() const
{
    auto response_body=QJsonDocument::fromJson(p->response_body).object().toVariantHash();
    return {
        {QStringLiteral("response_body"), response_body},
        {QStringLiteral("qt_status_code"), p->response_qt_status_code},
        {QStringLiteral("status_code"), p->response_status_code},
        {QStringLiteral("reason_phrase"), p->response_reason_phrase},
    };
}

bool HttpResponse::isOk() const
{
    if(p->response_status_code==200)
        return true;
    if(p->response_qt_status_code==QNetworkReply::NoError)
        return true;
    return false;
}

bool HttpResponse::isCreated() const
{
    if(p->response_qt_status_code!=QNetworkReply::NoError)
        return false;
    if(p->response_status_code!=0 && p->response_status_code!=201)
        return false;
    return true;
}

bool HttpResponse::isNotFound() const
{
    if(p->response_status_code==404)
        return true;
    return false;
}

bool HttpResponse::isUnAuthorized() const
{
    if(p->response_status_code==401)
        return true;
    return false;
}

HttpResponse &HttpResponse::setResponse(QObject *objectResponse)
{
    if(objectResponse!=nullptr && QRpc::RequestJobResponse::staticMetaObject.cast(objectResponse)){
        auto &response=*dynamic_cast<QRpc::RequestJobResponse*>(objectResponse);
        p->response_header.setRawHeader(response.responseHeader);
        p->response_status_code=response.response_status_code;
        p->response_qt_status_code=response.response_qt_status_code;
        p->response_reason_phrase=response.response_status_reason_phrase;
        p->response_body=response.response_body;
    }
    return *this;
}

QString HttpResponse::toString() const
{
    auto &response=*this;
    auto qt_text=ListenRequestCode::qt_network_error_phrase(p->response_qt_status_code);
    auto msg=QStringLiteral("QtStatus: Status:%1, %2, %3").arg(QString::number(response.qtStatusCode()), response.reasonPhrase(),qt_text);
    return msg;
}

HttpResponse::operator bool() const
{
    if(p->response_qt_status_code==QNetworkReply::NoError)
        return true;

    if(p->response_status_code==200)
        return true;

    return false;
}

HttpResponse &HttpResponse::print(const QString &output)
{
    for(auto &v:this->printOut(output))
        rInfo()<<v;
    return *this;
}

QStringList HttpResponse::printOut(const QString &output)
{
    auto space=output.trimmed().isEmpty()?"":QStringLiteral("    ");
    Q_DECLARE_VU;
    auto out=this->header().printOut(output);
    out<<QStringLiteral("%1%2.     %3").arg(space, output, this->toString().trimmed());
    out<<QStringLiteral("%1%2.     statusCode==%3").arg(space, output).arg(this->statusCode());
    out<<QStringLiteral("%1%2.     qtStatusCode==%3").arg(space, output).arg(this->qtStatusCode());
    out<<QStringLiteral("%1%2.     reasonPhrase==%3").arg(space, output, this->reasonPhrase());
    return out;
}

}
