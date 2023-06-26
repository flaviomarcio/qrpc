#pragma once

#include "./qrpc_last_error.h"
#include "./qrpc_global.h"
#include "./qrpc_types.h"
#include "../../qstm/src/qstm_setting_base.h"
#include "../../qrpc/src/qrpc_request_exchange.h"
#include "./private/p_qrpc_http_headers.h"
#include "./private/p_qrpc_http_response.h"
#include <QFile>

namespace QRpc {

#define QRPC_DECLARE_CONTENT_TYPE(type)\
public:\
    Q_INVOKABLE virtual QRpc::Types::ContentType contentType(){return QRpc::Types::ContentType(type); }

#define QRPC_DECLARE_BASE_ROUTE(route)\
public:\
    Q_INVOKABLE virtual const QByteArray &baseRoute()const{\
        static const QByteArray __baseRoute=#route; \
        return __baseRoute;\
    }

#define QRPC_DECLARE_CONSTRUCTOR(classBase)\
public:\
    Q_INVOKABLE explicit classBase(QObject *parent=nullptr):QRpc::Request(parent){\
    }

#define QRPC_DECLARE_REQUEST_CLASS(classBase,ContentType,route)\
class classBase:public QRpc::Request{\
public:\
    QRPC_DECLARE_CONTENT_TYPE(ContentType)\
    QRPC_DECLARE_BASE_ROUTE(route)\
    QRPC_DECLARE_CONSTRUCTOR(classBase)\
};

#define QRPC_DECLARE_REQUEST_ROUTE(Controller, v1)\
public:\
    Q_INVOKABLE virtual const QByteArray &route()const{\
        static const auto __route=QByteArrayLiteral(#v1).replace(QByteArrayLiteral("\""), QByteArrayLiteral(""));\
        return __route;\
    }

class RequestPvt;
class ListenRequest;

//!
//! \brief The Request class
//!
class Q_RPC_EXPORT Request: public QObject
{
    Q_OBJECT
public:

    enum Action {
        acRequest=1, acDownload=2, acUpload=3
    };

    Q_ENUM(Action)

public:
    QRPC_DECLARE_CONTENT_TYPE(QRpc::Types::AppJson)
    QRPC_DECLARE_BASE_ROUTE(/)

    //!
    //! \brief The Body class
    //!
    class Q_RPC_EXPORT Body: public QObject{
        friend class RequestPvt;
    public:

        //!
        //! \brief Body
        //! \param parent
        //!
        explicit Body(QObject *parent=nullptr);

        //!
        //! \brief body
        //! \return
        //!
        const QVariant &body() const;

        //!
        //! \brief setBody
        //! \param value
        //!
        void setBody(const QVariant &value);

        //!
        //! \brief toString
        //! \return
        //!
        QString toString() const;

        //!
        //! \brief toHash
        //! \return
        //!
        QVariantHash toHash() const;

        //!
        //! \brief toList
        //! \return
        //!
        QVariantList toList() const;

        //!
        //! \brief rq
        //! \return
        //!
        Request &rq();
    private:
        RequestPvt *p = nullptr;
    };

public:
    Q_INVOKABLE explicit Request(QObject *parent=nullptr);

    //!
    //! \brief startsWith
    //! \param requestPath
    //! \param requestPathBase
    //! \return
    //!
    static bool startsWith(const QString &requestPath, const QVariant &requestPathBase);
    bool startsWith(const QString &requestPath);

    //!
    //! \brief isEqual
    //! \param requestPath
    //! \return
    //!
    bool isEqual(const QString &requestPath);
    bool isEqual(const QString &requestPath, const QVariant &requestPathBase);

    //!
    //! \brief canRequest
    //! \return
    //!
    bool canRequest() const;

    //!
    //! \brief setSettings
    //! \param setting
    //! \return
    //!
    Request &setSettings(const QStm::SettingBase &setting);
    Request &setSettings(const QVariantHash &setting);
    Request &settings(const QStm::SettingBase &setting){ return this->setSettings(setting);}
    Request &settings(const QVariantHash &setting){ return this->setSettings(setting);}

    //!
    //! \brief url
    //! \return
    //!
    QString url(const QString &path={}) const;

    //!
    //! \brief protocol
    //! \return
    //!
    QRpc::Types::Protocol protocol() const;
    Request &setProtocol(const QVariant &value);
    Request &protocol(const QVariant &value){ return this->setProtocol(value);};

    //!
    //! \brief method
    //! \return
    //!
    QRpc::Types::Method method() const;
    Request &setMethod(const QVariant &value);
    Request &method(const QVariant &value){ return this->setMethod(value);}
    //!
    //! \brief driver
    //! \return
    //!
    const QString driver() const;
    Request &setDriver(const QString &value);
    Request &driver(const QString &value){ return this->setDriver(value);}

    //!
    //! \brief hostName
    //! \return
    //!
    const QString hostName() const;
    Request &setHostName(const QString &value);
    Request &hostName(const QString &value){ return this->setHostName(value);}

    //!
    //! \brief userName
    //! \return
    //!
    const QString userName() const;
    Request &setUserName(const QString &value);
    Request &userName(const QString &value){ return this->setUserName(value);}

    //!
    //! \brief password
    //! \return
    //!
    const QString password() const;
    Request &setPassword(const QString &value);
    Request &password(const QString &value){ return this->setPassword(value);}

    //!
    //! \brief route
    //! \return
    //!
    const QString route() const;
    Request &setRoute(const QVariant &value);
    Request &route(const QVariant &value){ return this->setRoute(value);}

    //!
    //! \brief body
    //! \return
    //!
    Request::Body &body() const;
    Request &setBody(const QVariant &value);
    Request &body(const QVariant &value){ return this->setBody(value);};

    //!
    //! \brief port
    //! \return
    //!
    QVariant port() const;
    Request &setPort(const QVariant &value);
    Request &port(const QVariant &value){ return this->setPort(value);}

    //!
    //! \brief activityLimit
    //! \return
    //!
    qlonglong activityLimit() const;
    Request &setActivityLimit(const QVariant &value);
    Request &activityLimit(const QVariant &value){ return this->setActivityLimit(value);}

    //!
    //! \brief exchange
    //! \return
    //!
    RequestExchange &exchange()const;

    //!
    //! \brief header
    //! \return
    //!
    HttpHeaders &header() const;

    //!
    //! \brief response
    //! \return
    //!
    HttpResponse &response() const;

    //!
    //! \brief requestRecovery
    //! \return
    //! repeat call on return
    const QHash<int, int> &requestRecovery() const;

    //!
    //! \brief setRequestRecovery
    //! \param statusCode
    //! \param repeatCount
    //! \return
    //! re-run request on response specific status code
    Request &setRequestRecovery(int statusCode);
    Request &setRequestRecovery(int statusCode, int repeatCount=1);

    //!
    //! \brief setRequestRecoveryOnBadGateway
    //! \param repeatCount
    //! \return
    //! re-run request on response status code 502 ou message BadRequest
    Request &setRequestRecoveryOnBadGateway(int repeatCount=1);

    //!
    //! \brief lastError
    //! \return
    //!
    const LastError &lastError()const;

    //!
    //! \brief call
    //! \return
    //!
    HttpResponse &call();

    //!
    //! \brief call
    //! \param route
    //! \return
    //!
    HttpResponse &call(const QString &route);

    //!
    //! \brief call
    //! \param route
    //! \param body
    //! \return
    //!
    HttpResponse &call(const QVariant &route, const QVariant &body);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param body
    //! \return
    //!
    HttpResponse &call(QRpc::Types::Method method, const QString &route, const QVariant &body);

    //!
    //! \brief call
    //! \param method
    //! \return
    //!
    HttpResponse &call(QRpc::Types::Method method);

    //!
    //! \brief call
    //! \param route
    //! \param body
    //! \param method
    //! \param parameter
    //! \return
    //!
    HttpResponse &call(const QVariant &route, const QVariant &body, const QString &method, const QVariantHash parameter);

    //!
    //! \brief call
    //! \param route
    //! \param objectBody
    //! \return
    //!
    HttpResponse &call(const QVariant &route, const QObject &objectBody);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param objectBody
    //! \return
    //!
    HttpResponse &call(QRpc::Types::Method method, const QString &route, const QObject &objectBody);

    //!
    //! \brief call
    //! \param route
    //! \param ioDeviceBody
    //! \return
    //!
    HttpResponse &call(const QVariant &route, QIODevice &ioDeviceBody);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \return
    //!
    HttpResponse &call(QRpc::Types::Method method, const QString &route);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param ioDeviceBody
    //! \return
    //!
    HttpResponse &call(QRpc::Types::Method method, const QString &route, QIODevice &ioDeviceBody);

    //!
    //! \brief operator =
    //! \param value
    //! \return
    //!
    Request &operator=(const QStm::SettingBase &value);

    //!
    //! \brief upload
    //! \param file
    //! \return
    //!
    HttpResponse &upload(QFile &file);

    //!
    //! \brief upload
    //! \param route
    //! \param buffer
    //! \return
    //!
    HttpResponse &upload(const QVariant &route, const QByteArray &buffer);

    //!
    //! \brief upload
    //! \param route
    //! \param file
    //! \return
    //!
    HttpResponse &upload(const QVariant &route, QFile &file);

    //!
    //! \brief upload
    //! \param route
    //! \param fileName
    //! \param file
    //! \return
    //!
    HttpResponse &upload(const QVariant &route, QString &fileName, QFile &file);

    //!
    //! \brief download
    //! \param fileName
    //! \return
    //!
    HttpResponse &download(QString &fileName);

    //!
    //! \brief download
    //! \param route
    //! \param fileName
    //! \return
    //!
    HttpResponse &download(const QVariant &route, QString &fileName);

    //!
    //! \brief download
    //! \param route
    //! \param fileName
    //! \param parameter
    //! \return
    //!
    HttpResponse &download(const QVariant &route, QString &fileName, const QVariant &parameter);

    //!
    //! \brief autoSetCookie
    //! \return
    //!
    Request &autoSetCookie();

    //!
    //! \brief toString
    //! \return
    //!
    QString toString() const;

    //!
    //! \brief toResponse
    //! \return
    //!
    QVariantHash toResponse() const;

//    //!
//    //! \brief sslConfiguration
//    //! \return
//    //!
//    QSslConfiguration &sslConfiguration();
//    Request &setSslConfiguration(const QSslConfiguration &value);

    //!
    //! \brief print
    //! \return
    //!
    Request &print();

    //!
    //! \brief printOut
    //! \return
    //!
    QStringList printOut();

private:
    RequestPvt *p = nullptr;
};

}
