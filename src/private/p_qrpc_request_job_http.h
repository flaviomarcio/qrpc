#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include <QHttpMultiPart>
#include <QTemporaryFile>
#include <QCoreApplication>
#include <QMutex>

namespace QRpc {

//!
//! \brief The RequestJobHttp class
//!
class RequestJobHttp : public RequestJobProtocol
{
    Q_OBJECT
public:
    QString tempLocation;
    QTimer*__timeout=nullptr;
    QMutex mutexRequestFinished;
    QFile fileDownload;
    QFile fileUpload;
    QFile fileTemp;
    QNetworkRequest request;
    RequestJobResponse*response=nullptr;
    QNetworkReply *reply = nullptr;
    QNetworkAccessManager*nam = nullptr;

    Q_INVOKABLE explicit RequestJobHttp(QObject *parent);

    ~RequestJobHttp();

    void fileMake();

    void fileFree();

    virtual bool call(RequestJobResponse*response)override;

    void timeout_start();

    void timeout_stop();

private slots:

    void onReplyError(QNetworkReply::NetworkError e);

    void onReplyFinish();
    void onReplyTimeout();

    void onReplyDelete();

    void onReplyProgressUpload(qint64 bytesSent, qint64 bytesTotal);

    void onReplyProgressDownload(qint64 bytesReceived, qint64 bytesTotal);

    void onFinish();

};

}
