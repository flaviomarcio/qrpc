#ifndef QRPC_RequestHTTPFunctional_H
#define QRPC_RequestHTTPFunctional_H

#include "./qrpc_test_functional.h"
#include <QtReforce/QRpc>

namespace QRpc {

class Q_RPC_RequestHTTPFunctional : public SDKGoogleTestFunctional
{
public:
};

QRPC_DECLARE_REQUEST_CLASS(Q_RPC_RequestHTTPFunctionalV1, QRpc::AppJson, "/")

#define d_Q_RPC_RequestHTTPFunctionalV1(request) \
    Q_RPC_RequestHTTPFunctionalV1 request; \
    request.setPort(public_record.server_port_http);

TEST_F(Q_RPC_RequestHTTPFunctional, serviceStart)
{
    EXPECT_TRUE(this->serviceStart()) << "fail: service start";
}

TEST_F(Q_RPC_RequestHTTPFunctional, check_head)
{
    d_Q_RPC_RequestHTTPFunctionalV1(request);
    auto &response = request.call(QRpc::Head, "check_head", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    //auto dt=QDate::fromString(response.body(),Qt::ISODate);
    //EXPECT_EQ(dt,QDate::currentDate())<<"fail";
}

TEST_F(Q_RPC_RequestHTTPFunctional, check_get)
{
    d_Q_RPC_RequestHTTPFunctionalV1(request);
    auto &response = request.call(QRpc::Get, "check_get", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    auto dt = QDate::fromString(response.body(), Qt::ISODate);
    EXPECT_EQ(dt, QDate::currentDate()) << "fail";
}

TEST_F(Q_RPC_RequestHTTPFunctional, check_post)
{
    d_Q_RPC_RequestHTTPFunctionalV1(request);
    auto &response = request.call(QRpc::Post, "check_post", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    auto dt = QDate::fromString(response.body(), Qt::ISODate);
    EXPECT_EQ(dt, QDate::currentDate()) << "fail";
}

TEST_F(Q_RPC_RequestHTTPFunctional, check_put)
{
    d_Q_RPC_RequestHTTPFunctionalV1(request);
    auto &response = request.call(QRpc::Put, "check_put", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    auto dt = QDate::fromString(response.body(), Qt::ISODate);
    EXPECT_EQ(dt, QDate::currentDate()) << "fail";
}

TEST_F(Q_RPC_RequestHTTPFunctional, check_delete)
{
    d_Q_RPC_RequestHTTPFunctionalV1(request);
    auto &response = request.call(QRpc::Delete, "check_delete", QVariant());
    EXPECT_TRUE(response.isOk()) << "fail";
    auto dt = QDate::fromString(response.body(), Qt::ISODate);
    EXPECT_EQ(dt, QDate::currentDate()) << "fail";
}

TEST_F(Q_RPC_RequestHTTPFunctional, serviceStop)
{
    EXPECT_TRUE(this->serviceStop()) << "fail: service stop";
}

} // namespace QRpc

#endif
