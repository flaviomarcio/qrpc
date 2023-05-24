#ifndef QRPC_AMQP_HANDLE_H
#define QRPC_AMQP_HANDLE_H

#include <memory>
#include <amqpcpp.h>
#include <amqpcpp/message.h>
#include <QDateTime>

namespace QRpc {
class AMQPHandleImpl;
class AMQPHandle: public AMQP::ConnectionHandler
{
public:

    static constexpr size_t BUFFER_SIZE = 8 * 1024 * 1024; //8Mb
    static constexpr size_t TEMP_BUFFER_SIZE = 1024 * 1024; //1Mb

    AMQPHandle(const std::string& host, uint16_t port);
    virtual ~AMQPHandle();

    std::string hash;

    void loop(const QDateTime &limit);
    bool go();
    void quit();

    bool connected() const;

private:

    AMQPHandle(const AMQPHandle&) = delete;
    AMQPHandle& operator=(const AMQPHandle&) = delete;

    void close();

    virtual void onData(
            AMQP::Connection *connection, const char *data, size_t size);

    virtual void onConnected(AMQP::Connection *connection);

    virtual void onError(AMQP::Connection *connection, const char *message);

    virtual void onClosed(AMQP::Connection *connection);

    bool sendDataFromBuffer();

private:
    std::shared_ptr<AMQPHandleImpl> m_impl;
};

}

#endif
