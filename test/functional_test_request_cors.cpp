#ifndef Q_RPC_RequestFunctionalCORS_H
#define Q_RPC_RequestFunctionalCORS_H

#include "./qrpc_test_functional.h"
#include <QtReforce/QRpc>

namespace QRpc {

class Q_RPC_RequestFunctionalCORS : public SDKGoogleTestFunctional
{
public:
    bool checkResponseHeader(HttpResponse &response)
    {
        const auto &responseHeader = response.header().rawHeader();
        static auto headersName = QStringList{QStringLiteral("Date"),
                                              QStringLiteral("Server"),
                                              QStringLiteral("Access-Control-Allow-Origin"),
                                              QStringLiteral("Access-Control-Allow-Methods"),
                                              QStringLiteral("Access-Control-Allow-Headers"),
                                              QStringLiteral("Access-Control-Max-Age"),
                                              QStringLiteral("Vary")
    };
        for (auto &v : headersName) {
            if (!responseHeader.contains(v)) {
                sWarning() << v;
                return false;
            }
        }
        return true;
    }
};

QRPC_DECLARE_REQUEST_CLASS(Q_RPC_RequestFunctionalV1CORS, QRpc::AppJson, "/")

#define Q_RPC_RequestFunctionalCORSV1(request) \
    Q_RPC_RequestFunctionalV1CORS request; \
    request.header().addRawHeader("Origin", "localhost"); \
    request.header().addRawHeader("Accept", "*/*"); \
    request.header().addRawHeader("Accept-Language: en-US,en;q=0.5", "*/*"); \
    request.header().addRawHeader("Accept-Encoding: gzip, deflate", "*/*"); \
    request.header().addRawHeader("Access-Control-Request-Method: POST", "*/*"); \
    request.header().addRawHeader("Access-Control-Request-Headers: authorization,content-type", "*/*"); \
    request.header().addRawHeader("Referer: http://localhost:9999", "*/*"); \
    request.header().addRawHeader("Connection", "keep-alive"); \
    request.setPort(public_record.server_port_http);


TEST_F(Q_RPC_RequestFunctionalCORS, serviceStart)
{
    EXPECT_TRUE(this->serviceStart()) << "fail: service start";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_head)
{
    Q_RPC_RequestFunctionalCORSV1(request);

    auto &response = request.call(QRpc::Options, "check_head", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    EXPECT_EQ(this->checkResponseHeader(response), true) << "invalid response header";
    EXPECT_EQ(response.body().isEmpty(), true) << "fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_get)
{
    Q_RPC_RequestFunctionalCORSV1(request);
    auto &response = request.call(QRpc::Options, "check_get", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    EXPECT_EQ(this->checkResponseHeader(response), true) << "invalid response header";
    EXPECT_EQ(response.body().isEmpty(), true) << "fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_post)
{
    Q_RPC_RequestFunctionalCORSV1(request);
    auto &response = request.call(QRpc::Options, "check_post", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    EXPECT_EQ(this->checkResponseHeader(response), true) << "invalid response header";
    EXPECT_EQ(response.body().isEmpty(), true) << "fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_put)
{
    Q_RPC_RequestFunctionalCORSV1(request);
    auto &response = request.call(QRpc::Options, "check_put", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    EXPECT_EQ(this->checkResponseHeader(response), true) << "invalid response header";
    EXPECT_EQ(response.body().isEmpty(), true) << "fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_delete)
{
    Q_RPC_RequestFunctionalCORSV1(request);
    auto &response = request.call(QRpc::Options, "check_delete", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    EXPECT_EQ(this->checkResponseHeader(response), true) << "invalid response header";
    EXPECT_EQ(response.body().isEmpty(), true) << "fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, serviceStop)
{
    EXPECT_TRUE(this->serviceStop()) << "fail: service stop";
}

} // namespace QRpc

#endif
