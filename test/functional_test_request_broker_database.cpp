#ifndef Q_RPC_RequestFunctionalBrokerDatabase_H
#define Q_RPC_RequestFunctionalBrokerDatabase_H

#include <QtReforce/QRpc>
#include "./qrpc_test_functional.h"

namespace QRpc {

class Q_RPC_RequestFunctionalBrokerDatabase : public SDKGoogleTestFunctional {
public:

    bool checkResponseHeader(HttpResponse &response){

        const auto&responseHeader=response.header().rawHeader();
        static auto headersName=QStringList{QStringLiteral("Date"),QStringLiteral("Server"),QStringLiteral("Access-Control-Allow-Origin"),QStringLiteral("Access-Control-Allow-Methods"),QStringLiteral("Access-Control-Allow-Headers"),QStringLiteral("Access-Control-Max-Age"),QStringLiteral("Vary")};
        for(auto&v:headersName){
            if(!responseHeader.contains(v)){
                sWarning()<<v;
                return false;
            }
        }
        return true;
    }

};


QRPC_DECLARE_REQUEST_CLASS(Q_RPC_RequestFunctionalBrokerDatabaseV1,QRpc::AppJson, "/")

#define Q_RPC_RequestFunctionalBrokerDatabaseV1(request)\
        Q_RPC_RequestFunctionalBrokerDatabaseV1 request;\
        request.header().addRawHeader("Origin", "localhost");\
        request.header().addRawHeader("Accept", "*/*");\
        request.header().addRawHeader("Accept-Language: en-US,en;q=0.5", "*/*");\
        request.header().addRawHeader("Accept-Encoding: gzip, deflate", "*/*");\
        request.header().addRawHeader("Access-Control-Request-Method: POST", "*/*");\
        request.header().addRawHeader("Access-Control-Request-Headers: authorization,content-type", "*/*");\
        request.header().addRawHeader("Referer: http://localhost:9999", "*/*");\
        request.header().addRawHeader("Connection","keep-alive");\
        request.setProtocol(QRpc::DataBase);\
        request.setDriver(public_record.server_broker_database_driver);\
        request.setUserName(public_record.server_broker_database_username);\
        request.setPassword(public_record.server_broker_database_password);\
        request.setPort(public_record.server_broker_database_port);

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, serviceStart)
{
    EXPECT_TRUE(this->serviceStart())<<"fail: service start";
}
/*


TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_head)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);

    auto&response=request.call(QRpc::Options,"check_head",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_get)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);
    auto&response=request.call(QRpc::Options,"check_get",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_post)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);
    auto&response=request.call(QRpc::Options,"check_post",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_put)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);
    auto&response=request.call(QRpc::Options,"check_put",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_delete)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);
    auto&response=request.call(QRpc::Options,"check_delete",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}
*/

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, serviceStop)
{
    EXPECT_TRUE(this->serviceStop())<<"fail: service stop";
}

}

#endif
