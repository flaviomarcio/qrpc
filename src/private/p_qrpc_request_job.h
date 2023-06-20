#pragma once

#include "./p_qrpc_request_job_response.h"

namespace QRpc {
class RequestJobPvt;
//!
//! \brief The RequestJob class
//!
class RequestJob : public QThread
{
    Q_OBJECT
public:

    explicit RequestJob();

    void run()override;

    static RequestJob *newJob(Request::Action action, const QString &action_fileName={});
    static RequestJob *runJob(RequestJob *job);

    RequestJob &start();

    RequestJob &release();

    RequestJobResponse &response();

    void setResponse(const RequestJobResponse &value);

public slots:

    void onRunJob(/*const QSslConfiguration *sslConfiguration, */const QVariantHash &headers, const QVariant &vUrl, const QString &fileName, QRpc::Request *request);

    void onRunCallback(const QVariant &v);

    void onRun();
private:
    RequestJobPvt *p=nullptr;
};

}
