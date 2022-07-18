#pragma once

#include "../qrpc_request.h"
#include "./p_qrpc_util.h"
#include "../qrpc_listen_request.h"
#include "./p_qrpc_request_job.h"
#include "./p_qrpc_http_response.h"
#include <QList>
#include <QMutex>
#include <QNetworkReply>
#include <QMetaProperty>
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
#include <QThread>
#include <QStringList>
#include <QDir>
#include <QtCore/QCoreApplication>

namespace QRpc {

class RequestPvt:public QObject{
    Q_OBJECT
public:
    Request*parent=nullptr;
    QHash<int,int> requestRecovery;

    RequestExchange exchange;
    QRpc::HttpHeaders qrpcHeader;
    Request::Body qrpcBody;
    QRpc::HttpResponse qrpcResponse;
    QRpc::LastError qrpcLastError;

    QUrl request_url;
    QVariant request_body;

    QNetworkReply::NetworkError response_qt_status_code=QNetworkReply::NoError;
    QString response_reason_phrase;
    ListenRequest*request=nullptr;

    QString fileLog;

    QSslConfiguration sslConfiguration;

    explicit RequestPvt(Request*parent);

    virtual ~RequestPvt();

    void setSettings(const ServiceSetting &setting);


    QString parseFileName(const QString &fileName);

    void writeLog(RequestJobResponse&response, const QVariant &request);

    HttpResponse &upload(const QString &route, const QString &fileName);

    HttpResponse &download(const QString &route, const QString &fileName);

    HttpResponse&call(const RequestMethod&method, const QVariant &vRoute, const QVariant &body);
signals:
    void runJob(const QSslConfiguration*sslConfig, const QVariantHash &headers, const QVariant &url, const QString &fileName, QRpc::Request*request);
};

}
