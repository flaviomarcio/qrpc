#pragma once

#include "../qrpc_request.h"

#include <QUuid>
#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>

#include <QTimer>
#include <QThread>
#include <QUrl>
#include <QUrlQuery>

namespace QRpc {

//!
//! \brief The RequestJobResponse class
//!
class RequestJobResponse:public QObject{
    Q_OBJECT
public:

    RequestExchange request_exchange;
    Request *localRequest=nullptr;

    QUuid request_uuid=QUuid::createUuidV3(QUuid::createUuid(), QUuid::createUuid().toString().toUtf8());
    QVariant request_url;
    QVariantHash request_header;
    QVariantHash request_parameters;
    QByteArray request_body;
    qlonglong activityLimit=120000;
    QDateTime request_start;
    QDateTime request_finish;
    int response_status_code = QNetworkReply::NoError;
    QByteArray response_status_reason_phrase;
    QByteArray response_body;
    QNetworkReply::NetworkError response_qt_status_code = QNetworkReply::NoError;
    QHash<QString, QVariant> responseHeader;

    explicit RequestJobResponse(QObject *parent);

    explicit RequestJobResponse(const QVariantHash &request_header, const QVariant &vUrl, Request &request, QObject *parent);

    RequestJobResponse &operator =(const RequestJobResponse &e);

    void clear();

    QVariantHash toHashResquest();

    QVariantHash toHashResponse();

    QVariantHash toVariant();

};

}
