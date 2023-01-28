#include "./p_qrpc_listen_http.h"
#include "../../../qstm/src/qstm_url.h"
#include "../../../qstm/src/qstm_crypto_util.h"
#include "../3rdparty/qtwebapp/httpserver/httplistener.h"
#include "../3rdparty/qtwebapp/httpserver/httprequesthandler.h"
#include "../3rdparty/qtwebapp/httpserver/httpsessionstore.h"
#include "../3rdparty/qtwebapp/httpserver/staticfilecontroller.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_listen_protocol.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_cache.h"
#include "../qrpc_const.h"
#include "../qrpc_macro.h"
//#include "../qrpc_server.h"
#include <QCoreApplication>
#include <QCryptographicHash>

static const auto __set_crypt_mode="set-crypt-mode";
static const auto __ENCRYPTED="ENCRYPTED";
//static const auto __DECRYPTED="DECRYPTED";

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
    QByteArray contextPath;
    HttpListeners3drparty *listener=nullptr;
    int port=-1;

    bool realMessageOnException = false;
    ListenHTTP *listen=nullptr;

    explicit HttpServer3rdparty(QSettings *settings, ListenHTTP *listen = nullptr)
        : stefanfrings::HttpRequestHandler{listen}
    {
        this->contextPath=settings->value("contextPath").toByteArray().trimmed();
        if(this->contextPath.isEmpty())
            this->contextPath="/";
        this->port=settings->value(QStringLiteral("port")).toInt();
        this->listen=listen;
        rWarning()<<QString(" listen: port %1, contextPath: %2").arg(this->port).arg(this->contextPath);
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
            QMultiHashIterator<QByteArray, QByteArray> i(getHeaders);
            while (i.hasNext()) {
                i.next();
                requestHeaders.insert(i.key(), i.value());
#if Q_RPC_LOG_SUPER_VERBOSE
                rInfo() << "   header - " + i.key() + ":" + i.value();
#endif
            }
        }

        {
            QMultiHashIterator<QByteArray, QByteArray> i(getParameters);
            while (i.hasNext()) {
                i.next();
                requestParameters.insert(i.key(), i.value());
#if Q_RPC_LOG_SUPER_VERBOSE
                rInfo() << "   parameter - " + i.key() + ":" + i.value();
#endif
            }
        }


        auto &request = listen->cacheRequest()->createRequest();
        auto requestPath = QString{req.getPath()}.trimmed().toLower();
        auto requestBody = QByteArray{req.getBody()};
        auto requestMethod = QString{req.getMethod()}.toLower();
        auto requestPort = this->port;

        if (!requestPath.isEmpty()) {
            auto c = requestPath.at(requestPath.length() - 1);
            if (c == '/')
                requestPath = requestPath.left(requestPath.length() - 1);
        }

        if(!this->contextPath.isEmpty() && requestPath.startsWith(this->contextPath))
            requestPath = requestPath.right(requestPath.length()-this->contextPath.length());

        auto isENCRYPTED=requestHeaders.value(__set_crypt_mode).toString().trimmed().toUpper()==__ENCRYPTED;
        if(isENCRYPTED){
            QStm::CryptoUtil cu;
            requestBody=cu.CryptoUtil::decrypt(requestBody);
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
        const auto responsePhrase = QString::fromUtf8(request.responsePhrase());


        static const auto staticUrlNames = QVector<int>{QMetaType::QUrl,
                                                        QMetaType::QVariantMap,
                                                        QMetaType::QVariantHash,
                                                        QMetaType::QString,
                                                        QMetaType::QByteArray,
                                                        QMetaType::QChar,
                                                        QMetaType::QBitArray};
        Url rpc_url;

        auto getBody=[&request, &rpc_url]()
        {
            const auto &responseBody = request.responseBody();
            if (!staticUrlNames.contains(responseBody.typeId()))
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
            auto msgOut = QStringLiteral("%1::%2:req:%3, ret: %4, %5 ms ")
                              .arg(QT_STRINGIFY2(QRpc::Server), requestMethod, requestPath)
                              .arg(request.responseCode())
                              .arg(mSecs);
            rInfo() << msgOut;
        } else {
            auto msgOut = QStringLiteral("%1::%2:req:%3, ret: %4, %5 ms ")
                              .arg(QT_STRINGIFY2(QRpc::Server), requestMethod, requestPath)
                              .arg(request.responseCode())
                              .arg(mSecs);
            rWarning() << msgOut;
            msgOut = QStringLiteral("ret=") + responsePhrase;
            rWarning() << msgOut;
            {
                {
                    QMultiHashIterator<QByteArray, QByteArray> i(getHeaders);
                    while (i.hasNext()) {
                        i.next();
                        rWarning() << QStringLiteral("   header - %1 : %2").arg(i.key(), i.value());
                    }
                }
#if Q_RPC_LOG_SUPER_VERBOSE
                {
                    QHashIterator<QByteArray, QByteArray> i(getParameter);

                    while (i.hasNext()) {
                        i.next();
                        rWarning() << QByteArrayLiteral("   parameter - ") + i.key() + QByteArrayLiteral(":") + i.value();
                    }
                }
#endif
#if Q_RPC_LOG_SUPER_VERBOSE
                rWarning() << QStringLiteral("       method:%1").arg(requestMethod);
                rWarning() << QByteArrayLiteral("       body:") + requestBody;
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
                auto fileName = rpc_url.toLocalFile().split(QStringLiteral("/")).last();
                rpc_url.headers().insert(QString(ContentDispositionName).toUtf8(),
                                         QStringLiteral("inline; filename=\"%1\"").arg(fileName).toUtf8());
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
            if(isENCRYPTED){
                ret.setHeader(__set_crypt_mode, __ENCRYPTED);
                QStm::CryptoUtil cu;
                body=cu.encrypt(body);
            }
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

    ~ListenHTTPPvt()
    {
        this->stop();
    }

    bool start()
    {
        {
            QMutexLocker<QMutex> locker(&this->lock);

            if(!this->started.tryLock())
                return {};

            auto colletions = this->parent->colletions();
            auto &option = colletions->protocol(Protocol::Http);
            for (auto &v : option.port()) {
                auto port = v.toInt();
                if (port <= 0)
                    continue;
                auto settings = option.makeSettings();
                settings->setValue(QStringLiteral("port"), port);
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
        QMutexLocker<QMutex> locker(&this->lock);
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
            QMutexLocker<QMutex> locker(&this->lock);
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
