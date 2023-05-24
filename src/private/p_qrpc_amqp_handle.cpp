#include "./p_qrpc_amqp_handle.h"
#include "./p_qrpc_request_job_amqp.h"
#include "./p_qrpc_request_job_protocol.h"
#include <QObject>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <cassert>
#include <iostream>
#include <QCoreApplication>
#include <Poco/Net/StreamSocket.h>

namespace
{
class Buffer
{
public:
    Buffer(size_t size) :
        m_data(size, 0),
        m_use(0)
    {
    }

    size_t write(const char* data, size_t size)
    {
        if (m_use == m_data.size())
        {
            return 0;
        }

        const size_t length = (size + m_use);
        size_t write =
                length < m_data.size() ? size : m_data.size() - m_use;
        memcpy(m_data.data() + m_use, data, write);
        m_use += write;
        return write;
    }

    void drain()
    {
        m_use = 0;
    }

    size_t available() const
    {
        return m_use;
    }

    const char* data() const
    {
        return m_data.data();
    }

    void shl(size_t count)
    {
        assert(count<m_use);

        const size_t diff = m_use - count;
        std::memmove(m_data.data(), m_data.data()+count, diff);
        m_use = m_use - count;
    }

private:
    std::vector<char> m_data;
    size_t m_use;
};
}


namespace QRpc {

struct AMQPHandleImpl
{
    AMQPHandleImpl() :
        connected(false),
        connection(nullptr),
        quit(false),
        inputBuffer(AMQPHandle::BUFFER_SIZE),
        outBuffer(AMQPHandle::BUFFER_SIZE),
        tmpBuff(AMQPHandle::TEMP_BUFFER_SIZE)
    {
    }

    Poco::Net::StreamSocket socket;
    bool connected=false;
    AMQP::Connection* connection;
    bool quit=false;
    Buffer inputBuffer;
    Buffer outBuffer;
    std::vector<char> tmpBuff;
};

AMQPHandle::AMQPHandle(const std::string& host, uint16_t port) :
    m_impl(new AMQPHandleImpl)
{
    const Poco::Net::SocketAddress address(host, port);
    m_impl->socket.connect(address);
    m_impl->socket.setKeepAlive(true);
}

AMQPHandle::~AMQPHandle()
{
    close();
}

void AMQPHandle::loop(const QDateTime&limit)
{
    m_impl->quit=false;
    try
    {
        while (!m_impl->quit)
        {
            if((limit.isValid() && QDateTime::currentDateTime()>=limit)){
                this->quit();
                continue;
            }
            else{
                this->go();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                qApp->processEvents();
            }
        }
        sendDataFromBuffer();

    } catch (const Poco::Exception& exc)
    {
        std::cerr<< "Poco exception " << exc.displayText();
    }

}

bool AMQPHandle::go()
{
    if(m_impl->quit){
        return false;
    }
    else{
        try
        {
            bool r=false;
            if (m_impl->socket.available() > 0)
            {
                r=true;
                size_t avail = m_impl->socket.available();
                if(m_impl->tmpBuff.size()<avail)
                {
                    m_impl->tmpBuff.resize(avail,0);
                }

                m_impl->socket.receiveBytes(&m_impl->tmpBuff[0], avail);
                m_impl->inputBuffer.write(m_impl->tmpBuff.data(), avail);

            }
            if(m_impl->socket.available()<0)
            {
                std::cerr<<"SOME socket error!!!"<<std::endl;
            }

            if (m_impl->connection && m_impl->inputBuffer.available())
            {
                size_t count = m_impl->connection->parse(m_impl->inputBuffer.data(),
                                                         m_impl->inputBuffer.available());

                if (count == m_impl->inputBuffer.available())
                {
                    m_impl->inputBuffer.drain();
                } else if(count >0 ){
                    m_impl->inputBuffer.shl(count);
                }
            }

            this->sendDataFromBuffer();

            return r;

        } catch (const Poco::Exception& exc)
        {
            std::cerr<< "Poco exception " << exc.displayText();
        }
        return false;
    }
}

void AMQPHandle::quit()
{
    m_impl->quit = true;
}

void AMQPHandle::AMQPHandle::close()
{
    m_impl->socket.close();
}

void AMQPHandle::onData(
        AMQP::Connection *connection, const char *data, size_t size)
{
    m_impl->connection = connection;
    const size_t writen = m_impl->outBuffer.write(data, size);
    if (writen != size)
    {
        sendDataFromBuffer();
        m_impl->outBuffer.write(data + writen, size - writen);
    }
}

void AMQPHandle::onConnected(AMQP::Connection *connection)
{
    Q_UNUSED(connection)
    m_impl->connected = true;
}

void AMQPHandle::onError(
        AMQP::Connection *connection, const char *message)
{
    Q_UNUSED(connection)
    std::cerr<<"AMQP error "<<message<<std::endl;
}

void AMQPHandle::onClosed(AMQP::Connection *connection)
{
    Q_UNUSED(connection)
    std::cout<<"AMQP closed connection"<<std::endl;
    m_impl->quit  = true;
}

bool AMQPHandle::connected() const
{
    return m_impl->connected;
}

bool AMQPHandle::sendDataFromBuffer()
{
    auto r=false;
    while(m_impl->outBuffer.available())
    {
        auto size=m_impl->outBuffer.available();
        auto data=m_impl->outBuffer.data();
        m_impl->socket.sendBytes(data, size);
        m_impl->outBuffer.drain();
        r=true;
    }
    return r;
}

}




