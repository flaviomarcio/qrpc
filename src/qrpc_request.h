#pragma once

#include "./qrpc_last_error.h"
#include "./qrpc_global.h"
#include "./qrpc_types.h"
#include "../../qrpc/src/qrpc_service_setting.h"
#include "../../qrpc/src/qrpc_request_exchange.h"
#include "./private/p_qrpc_http_headers.h"
#include "./private/p_qrpc_http_response.h"
#include <QStm>
#include <QFile>
#include <QSslConfiguration>

namespace QRpc {

#define QRPC_DECLARE_CONTENT_TYPE(type)\
public:\
    Q_INVOKABLE virtual QRpc::ContentType contentType(){\
        return QRpc::ContentType(type);\
    }\

#define QRPC_DECLARE_BASE_ROUTE(route)\
public:\
    Q_INVOKABLE virtual QByteArray baseRoute(){\
        return #route;\
    }\

#define QRPC_DECLARE_CONSTRUCTOR(classBase)\
public:\
    Q_INVOKABLE explicit classBase(QObject *parent=nullptr):QRpc::Request(parent){\
    }\
    ~classBase(){\
    }

#define QRPC_DECLARE_REQUEST_CLASS(classBase,ContentType,route)\
class classBase:public QRpc::Request{\
public:\
    QRPC_DECLARE_CONTENT_TYPE(ContentType)\
    QRPC_DECLARE_BASE_ROUTE(route)\
    QRPC_DECLARE_CONSTRUCTOR(classBase)\
};

#define QRPC_DECLARE_METHOD(route)\
    Q_INVOKABLE virtual QByteArray baseRoute(){\
        return #route;\
    }\


#define QRPC_DECLARE_REQUEST_ROUTE(Controller, v1)\
public:\
    Q_INVOKABLE virtual QByteArray route(){\
        return QByteArrayLiteral(#v1).replace(QByteArrayLiteral("\""), QByteArrayLiteral(""));\
    }\

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
    QRPC_DECLARE_CONTENT_TYPE(QRpc::AppJson)
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
        explicit Body(QObject*parent=nullptr);

        //!
        //! \brief body
        //! \return
        //!
        virtual QVariant &body() const;

        //!
        //! \brief setBody
        //! \param value
        //!
        virtual void setBody(const QVariant &value);

        //!
        //! \brief toString
        //! \return
        //!
        virtual QString toString() const;

        //!
        //! \brief toMap
        //! \return
        //!
        virtual QVariantMap toMap() const;

        //!
        //! \brief toHash
        //! \return
        //!
        virtual QVariantHash toHash() const;

        //!
        //! \brief toList
        //! \return
        //!
        virtual QVariantList toList() const;

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
    ~Request();

    //!
    //! \brief startsWith
    //! \param requestPath
    //! \param requestPathBase
    //! \return
    //!
    static bool startsWith(const QString &requestPath, const QVariant &requestPathBase);
    virtual bool startsWith(const QString &requestPath);

    //!
    //! \brief isEqual
    //! \param requestPath
    //! \return
    //!
    virtual bool isEqual(const QString &requestPath);
    virtual bool isEqual(const QString &requestPath, const QVariant &requestPathBase);

    //!
    //! \brief canRequest
    //! \return
    //!
    virtual bool canRequest() const;

    //!
    //! \brief setSettings
    //! \param setting
    //! \return
    //!
    virtual Request &setSettings(const ServiceSetting &setting);
    virtual Request &setSettings(const QVariantHash &setting);

    //!
    //! \brief url
    //! \return
    //!
    virtual QString url() const;
    virtual QString url(const QString &path) const;

    //!
    //! \brief protocol
    //! \return
    //!
    virtual Protocol protocol() const;
    virtual QString protocolName() const;
    virtual Request &setProtocol(const QVariant &value);

    //!
    //! \brief method
    //! \return
    //!
    virtual RequestMethod method() const;
    virtual Request &setMethod(const QString &value);
    virtual Request &setMethod(const QByteArray &value);
    virtual Request &setMethod(const int &value);

    //!
    //! \brief driver
    //! \return
    //!
    virtual QString driver() const;
    virtual Request &setDriver(const QString &value);

    //!
    //! \brief hostName
    //! \return
    //!
    virtual QString hostName() const;
    virtual Request &setHostName(const QString &value);

    //!
    //! \brief userName
    //! \return
    //!
    virtual QString userName() const;
    virtual Request &setUserName(const QString &value);

    //!
    //! \brief password
    //! \return
    //!
    virtual QString password() const;
    virtual Request &setPassword(const QString &value);

    //!
    //! \brief route
    //! \return
    //!
    virtual QString &route() const;
    virtual Request &setRoute(const QVariant &value);

    //!
    //! \brief body
    //! \return
    //!
    virtual QVariant body() const;
    virtual Request &setBody(const QVariant &value);

    //!
    //! \brief port
    //! \return
    //!
    virtual QVariant port() const;
    virtual Request &setPort(const QVariant &value);

    //!
    //! \brief activityLimit
    //! \return
    //!
    virtual qlonglong activityLimit() const;
    virtual Request &setActivityLimit(const QVariant &value);

    //!
    //! \brief exchange
    //! \return
    //!
    virtual RequestExchange &exchange();

    //!
    //! \brief header
    //! \return
    //!
    virtual HttpHeaders &header();

    //!
    //! \brief body
    //! \return
    //!
    virtual Request::Body &body();

    //!
    //! \brief response
    //! \return
    //!
    virtual HttpResponse &response();

    //!
    //! \brief requestRecovery
    //! \return
    //! repeat call on return
    virtual QHash<int,int> requestRecovery() const;

    //!
    //! \brief setRequestRecovery
    //! \param statusCode
    //! \param repeatCount
    //! \return
    //! re-run request on response specific status code
    virtual Request &setRequestRecovery(int statusCode);
    virtual Request &setRequestRecovery(int statusCode, int repeatCount=1);

    //!
    //! \brief setRequestRecoveryOnBadGateway
    //! \param repeatCount
    //! \return
    //! re-run request on response status code 502 ou message BadRequest
    virtual Request &setRequestRecoveryOnBadGateway(int repeatCount=1);

    //!
    //! \brief lastError
    //! \return
    //!
    virtual LastError &lastError();

    //!
    //! \brief call
    //! \return
    //!
    virtual HttpResponse &call();

    //!
    //! \brief call
    //! \param route
    //! \return
    //!
    virtual HttpResponse &call(const QVariant &route);

    //!
    //! \brief call
    //! \param route
    //! \param body
    //! \return
    //!
    virtual HttpResponse &call(const QVariant &route, const QVariant &body);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param body
    //! \return
    //!
    virtual HttpResponse &call(const RequestMethod &method, const QString &route, const QVariant &body);

    //!
    //! \brief call
    //! \param method
    //! \return
    //!
    virtual HttpResponse &call(const RequestMethod &method);

    //!
    //! \brief call
    //! \param route
    //! \param body
    //! \param method
    //! \param parameter
    //! \return
    //!
    virtual HttpResponse &call(const QVariant &route, const QVariant &body, const QString &method, const QVariantHash parameter);

    //!
    //! \brief call
    //! \param route
    //! \param objectBody
    //! \return
    //!
    virtual HttpResponse &call(const QVariant &route, const QObject &objectBody);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param objectBody
    //! \return
    //!
    virtual HttpResponse &call(const RequestMethod &method, const QString &route, const QObject &objectBody);

    //!
    //! \brief call
    //! \param route
    //! \param ioDeviceBody
    //! \return
    //!
    virtual HttpResponse &call(const QVariant &route, QIODevice &ioDeviceBody);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \return
    //!
    virtual HttpResponse &call(const RequestMethod &method, const QString &route);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param ioDeviceBody
    //! \return
    //!
    virtual HttpResponse &call(const RequestMethod &method, const QString &route, QIODevice &ioDeviceBody);

    //!
    //! \brief operator =
    //! \param value
    //! \return
    //!
    Request &operator=(const ServiceSetting &value);

    //!
    //! \brief upload
    //! \param file
    //! \return
    //!
    virtual HttpResponse &upload(QFile &file);

    //!
    //! \brief upload
    //! \param route
    //! \param buffer
    //! \return
    //!
    virtual HttpResponse &upload(const QVariant &route, const QByteArray &buffer);

    //!
    //! \brief upload
    //! \param route
    //! \param file
    //! \return
    //!
    virtual HttpResponse &upload(const QVariant &route, QFile &file);

    //!
    //! \brief upload
    //! \param route
    //! \param fileName
    //! \param file
    //! \return
    //!
    virtual HttpResponse &upload(const QVariant &route, QString &fileName, QFile &file);

    //!
    //! \brief download
    //! \param fileName
    //! \return
    //!
    virtual HttpResponse &download(QString &fileName);

    //!
    //! \brief download
    //! \param route
    //! \param fileName
    //! \return
    //!
    virtual HttpResponse &download(const QVariant &route, QString &fileName);

    //!
    //! \brief download
    //! \param route
    //! \param fileName
    //! \param parameter
    //! \return
    //!
    virtual HttpResponse &download(const QVariant &route, QString &fileName, const QVariant &parameter);

    //!
    //! \brief autoSetCookie
    //! \return
    //!
    virtual Request &autoSetCookie();

    //!
    //! \brief toString
    //! \return
    //!
    virtual QString toString() const;

    //!
    //! \brief toResponse
    //! \return
    //!
    virtual QVariantHash toResponse() const;

    //!
    //! \brief sslConfiguration
    //! \return
    //!
    virtual QSslConfiguration &sslConfiguration();
    virtual Request &setSslConfiguration(const QSslConfiguration &value);

    //!
    //! \brief print
    //! \return
    //!
    virtual Request &print();

    //!
    //! \brief printOut
    //! \return
    //!
    virtual QStringList printOut();

private:
    RequestPvt *p = nullptr;
};

QT_DEPRECATED_X("Use QRpc::Request")
typedef Request QRPCListen;

}
