#include "./p_qrpc_listen_http.h"
#include "../../../qstm/src/qstm_url.h"
#include "../3rdparty/qtwebapp/httpserver/httplistener.h"
#include "../3rdparty/qtwebapp/httpserver/httprequesthandler.h"
#include "../3rdparty/qtwebapp/httpserver/httpsessionstore.h"
#include "../3rdparty/qtwebapp/httpserver/staticfilecontroller.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_listen_protocol.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_cache.h"
#include "../qrpc_macro.h"
#include "../qrpc_server.h"
#include <QCoreApplication>
#include <QCryptographicHash>

namespace QRpc {

class HttpListeners3drparty:public QObject
{
public:
    explicit HttpListeners3drparty(QObject *parent):QObject{parent}{}

    QSettings *settings = nullptr;
    stefanfrings::HttpSessionStore *sessionStore = nullptr;
    stefanfrings::StaticFileController *fileController = nullptr;
    stefanfrings::HttpListener *listener = nullptr;

    static HttpListeners3drparty *make(stefanfrings::HttpRequestHandler *requestHandler, QSettings *settings)
    {
        auto __return = new HttpListeners3drparty{requestHandler};
        __return->settings = settings;
        settings->setParent(requestHandler);
        __return->listener = new stefanfrings::HttpListener{settings, requestHandler, requestHandler};
        __return->fileController = new stefanfrings::StaticFileController{settings, __return->listener};
        __return->sessionStore = new stefanfrings::HttpSessionStore(settings, __return->listener);
        QThread::msleep(100);
        return __return;
    };

};

class HttpServer3rdparty : public stefanfrings::HttpRequestHandler
{
public:
    HttpListeners3drparty *listener=nullptr;
    int port=-1;

    bool realMessageOnException = false;
    ListenHTTP *listen=nullptr;

    explicit HttpServer3rdparty(QSettings *settings, ListenHTTP *listen = nullptr)
        : stefanfrings::HttpRequestHandler{listen}
    {
        this->port=settings->value(qsl("port")).toInt();
        this->listen=listen;
        this->listener=HttpListeners3drparty::make(this, settings);
        QObject::connect(listen, &Listen::rpcResponse, this, &HttpServer3rdparty::onRpcResponse);
    }

    ~HttpServer3rdparty()
    {
        QObject::disconnect(listen, &Listen::rpcResponse, this, &HttpServer3rdparty::onRpcResponse);
    }

    void service(stefanfrings::HttpRequest &req, stefanfrings::HttpResponse &ret)
    {
        const auto time_start = QDateTime::currentDateTime();

        const auto getHeaders = req.getHeaderMap();
        const auto getParameters = req.getParameterMap();

        QVariantHash requestHeaders;
        QVariantHash requestParameters;

        {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QHashIterator<QByteArray, QByteArray> i(getHeaders);
#else
            QMultiHashIterator<QByteArray, QByteArray> i(getHeaders);
#endif

            while (i.hasNext()) {
                i.next();
                requestHeaders[i.key()] = i.value();
#if Q_RPC_LOG_SUPER_VERBOSE
                sInfo() << "   header - " + i.key() + ":" + i.value();
#endif
            }
        }

        {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QHashIterator<QByteArray, QByteArray> i(getParameters);
#else
            QMultiHashIterator<QByteArray, QByteArray> i(getParameters);
#endif
            while (i.hasNext()) {
                i.next();
                requestParameters[i.key()] = i.value();
#if Q_RPC_LOG_SUPER_VERBOSE
                sInfo() << "   parameter - " + i.key() + ":" + i.value();
#endif
            }
        }


        auto &request = listen->cacheRequest()->createRequest();
        auto requestPath = QString{req.getPath()}.trimmed();
        auto requestBody = QString{req.getBody()}.trimmed();
        auto requestMethod = QString{req.getMethod()}.toLower();
        auto requestPort = this->port;

        if (!requestPath.isEmpty()) {
            auto c = requestPath.at(requestPath.length() - 1);
            if (c == '/')
                requestPath = requestPath.left(requestPath.length() - 1);
        }

        request.setRequestProtocol(QRpc::Http);
        request.setRequestPort(requestPort);
        request.setRequestPath(requestPath.toUtf8());
        request.setRequestHeader(requestHeaders);
        request.setRequestParameter(requestParameters);
        request.setRequestMethod(requestMethod);
        request.setRequestBody(requestBody);

        auto vHash = request.toHash();
        QStringList uploadedFiles;
        if (!req.uploadedFiles.isEmpty()) {
            QHashIterator<QByteArray, QTemporaryFile *> i(req.uploadedFiles);
            while (i.hasNext()) {
                i.next();
                uploadedFiles << i.value()->fileName();
            }
        }
        emit this->listen->rpcRequest(vHash, QVariant{uploadedFiles});
        request.start();

        const auto mSecsSinceEpoch = double(QDateTime::currentDateTime().toMSecsSinceEpoch()
                                            - time_start.toMSecsSinceEpoch())
                                     / 1000.000;
        const auto mSecs = QString::number(mSecsSinceEpoch, 'f', 3);
        const auto responseCode = request.responseCode();
        const auto responsePhrase = QString::fromUtf8(request.responsePhrase());


        static const auto staticUrlNames = QVector<int>{QMetaType_QUrl,
                                                        QMetaType_QVariantMap,
                                                        QMetaType_QVariantHash,
                                                        QMetaType_QString,
                                                        QMetaType_QByteArray,
                                                        QMetaType_QChar,
                                                        QMetaType_QBitArray};
        Url rpc_url;

        auto getBody=[&request, &rpc_url]()
        {
            const auto &responseBody = request.responseBody();
            if (!staticUrlNames.contains(qTypeId(responseBody)))
                return request.responseBodyBytes();

            if (!rpc_url.read(responseBody).isValid())
                return request.responseBodyBytes();

            if (!rpc_url.isLocalFile())
                return QByteArray{};

            QFile file(rpc_url.toLocalFile());
            if (!file.open(file.ReadOnly)) {
                request.co().setNotFound();
                return QByteArray{};
            }
            auto body = file.readAll();
            file.close();
            return body;
        };
        auto body=getBody();

#if Q_RPC_LOG

        if (request.co().isOK()) {
            auto msgOut = qsl("%1::%2:req:%3, ret: %4, %5 ms ")
                              .arg(QT_STRINGIFY2(QRpc::Server), requestMethod, requestPath)
                              .arg(responseCode)
                              .arg(mSecs);
            cInfo() << msgOut;
        } else {
            auto msgOut = qsl("%1::%2:req:%3, ret: %4, %5 ms ")
                              .arg(QT_STRINGIFY2(QRpc::Server), requestMethod, requestPath)
                              .arg(responseCode)
                              .arg(mSecs);
            cWarning() << msgOut;
            msgOut = qsl("ret=") + responsePhrase;
            cWarning() << msgOut;
            {
                {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                    QHashIterator<QByteArray, QByteArray> i(getHeaders);
#else
                    QMultiHashIterator<QByteArray, QByteArray> i(getHeaders);
#endif
                    while (i.hasNext()) {
                        i.next();
                        cWarning() << qsl("   header - %1 : %2").arg(i.key(), i.value());
                    }
                }
#if Q_RPC_LOG_SUPER_VERBOSE
                {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                    QHashIterator<QByteArray, QByteArray> i(getParameter);
#else
                    QHashIterator<QByteArray, QByteArray> i(getParameter);
#endif

                    while (i.hasNext()) {
                        i.next();
                        cWarning() << qbl("   parameter - ") + i.key() + qbl(":") + i.value();
                    }
                }
#endif
#if Q_RPC_LOG_SUPER_VERBOSE
                cWarning() << qsl("       method:%1").arg(requestMethod);
                cWarning() << qbl("       body:") + requestBody;
#endif
            }
        }
#endif
        if (request.co().isOK() && rpc_url.isValid()) { //se for download
            if (!rpc_url.isLocalFile()){
                ret.redirect(rpc_url.toString().toUtf8());
                return;
            }

            QHashIterator<QString, QVariant> i(request.responseHeader());
            while (i.hasNext()) {
                i.next();
                auto &k = i.key();
                auto &v = i.value();
                rpc_url.headers().insert(k, v);
            }

            HttpHeaders headers(rpc_url.headers());
            if (!headers.contentDisposition().isValid()) {
                auto fileName = rpc_url.toLocalFile().split(qsl("/")).last();
                rpc_url.headers().insert(QString(ContentDispositionName).toUtf8(),
                                         qsl("inline; filename=\"%1\"").arg(fileName).toUtf8());
                if (!headers.contentType().isValid())
                    headers.setContentType(rpc_url.url());
            }
            request.setResponseHeader(rpc_url.headers());
        }
        if (ret.isConnected()) {
            QHashIterator<QString, QVariant> i(request.responseHeader());
            while (i.hasNext()) {
                i.next();
                auto k = i.key().toUtf8();
                auto v = i.value().toByteArray();
                ret.setHeader(k, v);
            }
            if (this->realMessageOnException)
                ret.setStatus(request.responseCode(),
                              request.responsePhrase()); //mensagem do backend
            else
                ret.setStatus(request.responseCode(), request.responsePhrase(0)); //mensagem padrao
            ret.write(body, true);
        }
    }

public slots:

    void onRpcResponse(QUuid uuid, const QVariantHash &vRequest)
    {
        auto &request = this->listen->cacheRequest()->toRequest(uuid);
        if (!request.isValid())
            return;

        if (!request.fromResponseMap(vRequest))
            request.co().setInternalServerError();

        emit request.finish();
    }
};

class ListenHTTPPvt : public QObject
{
public:
    QMutex lock, started;
    QList<HttpServer3rdparty *> listens;
    ListenHTTP *parent = nullptr;

    explicit ListenHTTPPvt(ListenHTTP *parent) : QObject{parent} { this->parent = parent; }



    bool start()
    {
        {
            QMutexLocker locker(&this->lock);

            if(!this->started.tryLock())
                return {};

            auto colletions = this->parent->colletions();
            auto &option = colletions->protocol(Protocol::Http);
            for (auto &v : option.port()) {
                auto port = v.toInt();
                if (port <= 0)
                    continue;
                auto settings = option.makeSettings();
                settings->setValue(qsl("port"), port);
                auto listen = new HttpServer3rdparty{settings, this->parent};
                listen->realMessageOnException = option.realMessageOnException();
                listens.append(listen);
            }
        }

        if(!this->isListening()){
            this->stop();
            return {};
        }


        return true;
    }

    bool isListening()
    {
        QMutexLocker locker(&this->lock);
        for (auto &h : this->listens) {
            if (!h->listener->listener->isListening())
                continue;
            return true;
        }
        return false;
    }

    bool stop()
    {
        if(!this->started.tryLock(1)){
            QMutexLocker locker(&this->lock);
            auto aux=this->listens;
            this->listens.clear();
            qDeleteAll(aux);
            this->started.unlock();
        }
        return true;
    }
};

ListenHTTP::ListenHTTP(QObject *parent) : Listen{parent}
{
    this->p = new ListenHTTPPvt{this};
}

ListenHTTP::~ListenHTTP()
{
    p->stop();
}

bool ListenHTTP::start()
{
    Listen::start();
    return p->start();
}

bool ListenHTTP::stop()
{
    Listen::stop();

    return p->stop();
}

} // namespace QRpc
