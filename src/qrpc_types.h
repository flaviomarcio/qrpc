#pragma once

#include <QHash>
#include <QMap>
#include <QObject>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QString>
#include <QTemporaryFile>
#include <QVariant>
#include <QMetaEnum>

namespace QRpc {

class Listen;
class ListenProtocol;
class ListenQRPCSlot;

//https://developer.mozilla.org/en-US/docs/Web/HTTP/Authentication#Authentication_schemes
#define Q_RPC_DECLARE_CONTENT_TYPE() \
enum ContentType {  \
    AppNone = 1,  \
    AppXML = AppNone * 2,  \
    AppJson = AppXML * 2,  \
    AppCBOR = AppJson * 2,  \
    AppOctetStream = AppCBOR * 2,  \
    AppXwwwForm = AppOctetStream * 2,  \
    AppPNG = AppXwwwForm * 2,  \
    AppJPGE = AppPNG * 2,  \
    AppGIF = AppJPGE * 2,  \
    AppPDF = AppGIF * 2,  \
    AppTXT = AppPDF * 2,  \
    AppHTML = AppTXT * 2,  \
    AppCSS = AppHTML * 2,  \
    AppJS = AppCSS * 2,  \
    AppSVG = AppJS * 2,  \
    AppWOFF = AppSVG * 2,  \
    AppWOFF2 = AppWOFF * 2,  \
    AppTTF = AppWOFF2 * 2,  \
    AppEOT = AppTTF * 2,  \
    AppOTF = AppEOT * 2,  \
}

//https://developer.mozilla.org/en-US/docs/Web/HTTP/Authentication#Authentication_schemes
#define Q_RPC_DECLARE_AUTHORIZATION_SCHEME() \
enum AuthorizationScheme { Basic, Bearer, Digest, HOBA, Mutual, AWS4_HMAC_SHA256, Token, Service}

#define Q_RPC_DECLARE_METHOD() \
enum Method { \
    Head = 1, \
    Get = 2, \
    Post = 4, \
    Put = 8, \
    Delete = 16, \
    Options = 32, \
    Patch = 64, \
    Trace = 128, \
}

#define Q_RPC_DECLARE_PROTOCOL() \
public: \
enum Protocol { \
    TcpSocket = 1, \
    UdpSocket = 2, \
    WebSocket = 4, \
    Mqtt = 8, \
    Amqp = 16, \
    Http = 32, \
    DataBase = 64, \
    Kafka = 128, \
    LocalSocket = 256, \
    Rpc=512, \
}

class Types : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief The ContentType enum
    //!
    Q_RPC_DECLARE_CONTENT_TYPE();
    Q_ENUM(ContentType)
    const QMetaEnum eContentType=QMetaEnum::fromType<ContentType>();
    static ContentType contentType(const QVariant &v);
    static QByteArray contentTypeName(const QVariant &contentType);
    static QString contentTypeExtension(const QVariant &contentType);
    static QString contentTypeHeader(const QVariant &contentType);
    static QVector<int> contentTypeHeaders(const QVariant &contentType);

    //!
    //! \brief The AuthorizationScheme enum
    //!
    Q_RPC_DECLARE_AUTHORIZATION_SCHEME();
    Q_ENUM(AuthorizationScheme)
    const QMetaEnum eAuthorizationScheme=QMetaEnum::fromType<AuthorizationScheme>();
    static QByteArray authorizationSchemeName(const QVariant &authorizationScheme);

    //!
    //! \brief The Protocol enum
    //!
    Q_RPC_DECLARE_PROTOCOL();
    Q_ENUM(Protocol)
    const QMetaEnum eProtocol=QMetaEnum::fromType<Protocol>();
    static QByteArray protocolName(const QVariant &protocol);

    //!
    //! \brief The Method enum
    //!
    Q_RPC_DECLARE_METHOD();
    Q_ENUM(Method)
    const QMetaEnum eMethod=QMetaEnum::fromType<Method>();
    static QByteArray methodName(const QVariant &method);

public:
    Q_INVOKABLE explicit Types(QObject *parent=nullptr);
};

typedef QHash<QByteArray, QMetaMethod> ControllerMethod;
typedef QHash<QByteArray, ControllerMethod> ControllerMethodCollection;

typedef QPair<int, const QMetaObject *> ListenMetaObject;
typedef QVector<ListenMetaObject> ListenMetaObjectList;
typedef QMultiHash<QByteArray, QMetaMethod> MethodsMultHash;
typedef QMultiHash<QByteArray, MethodsMultHash> MultStringMethod;
typedef QVector<ListenQRPCSlot *> ListenSlotList;
typedef QHash<QByteArray, ListenSlotList *> ListenSlotCache;
typedef QMultiHash<QByteArray, QStringList> MultStringList;
typedef QVector<const QMetaObject *> MetaObjectVector;
typedef QVector<QByteArray> ByteArrayVector;

static const auto ContentTypeName = "Content-Type";
static const auto ContentDispositionName = "Content-Disposition";
} // namespace QRpc
