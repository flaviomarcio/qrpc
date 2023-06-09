#pragma once

#include <QHash>
#include <QMap>
#include <QObject>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QString>
#include <QTemporaryFile>
#include <QVariant>

namespace QRpc {

class Listen;
class ListenProtocol;
class ListenQRPCSlot;

enum ContentType {
    AppNone = 1,
    AppXML = AppNone * 2,
    AppJson = AppXML * 2,
    AppCBOR = AppJson * 2,
    AppOctetStream = AppCBOR * 2,
    AppXwwwForm = AppOctetStream * 2,
    AppPNG = AppXwwwForm * 2,
    AppJPGE = AppPNG * 2,
    AppGIF = AppJPGE * 2,
    AppPDF = AppGIF * 2,
    AppTXT = AppPDF * 2,
    AppHTML = AppTXT * 2,
    AppCSS = AppHTML * 2,
    AppJS = AppCSS * 2,
    AppSVG = AppJS * 2,
    AppWOFF = AppSVG * 2,
    AppWOFF2 = AppWOFF * 2,
    AppTTF = AppWOFF2 * 2,
    AppEOT = AppTTF * 2,
    AppOTF = AppEOT * 2
};

//https://developer.mozilla.org/en-US/docs/Web/HTTP/Authentication#Authentication_schemes
enum AuthorizationType { Basic, Bearer, Digest, HOBA, Mutual, AWS4_HMAC_SHA256, Token, Service};

enum RequestMethod {
    Head = 1,
    Get = 2,
    Post = 4,
    Put = 8,
    Delete = 16,
    Options = 32,
    Patch = 64,
    Trace = 128,
    MinMethod = Head,
    MaxMethod = Trace
};

//!
//! \brief The Protocol enum
//!
enum Protocol {
    TcpSocket = 1,
    UdpSocket = 2,
    WebSocket = 4,
    Mqtt = 8,
    Amqp = 16,
    Http = 32,
    DataBase = 64,
    Kafka = 128,
    LocalSocket = 256,
    rpcProtocolMin = TcpSocket,
    rpcProtocolMax = LocalSocket
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

namespace Private {
const QHash<QString, int> &___ProtocolType();
const QHash<int, QString> &___ProtocolName();
const QHash<int, QString> &___ProtocolUrlName();
const QHash<int, QString> &___QSslProtocolToName();
const QHash<QString, QSsl::SslProtocol> &___QSslProtocolNameToProtocol();
const QHash<ContentType, QString> &___ContentTypeHeaderTypeToHeader();
const QHash<QString, ContentType> &___ContentTypeHeaderToHeaderType();
const QHash<ContentType, QString> &___ContentTypeHeaderToExtension();
const QHash<QString, ContentType> &___ContentTypeExtensionToHeader();
const QHash<int, QString> &___RequestMethodName();
const QHash<QString, RequestMethod> &___RequestMethodType();
} // namespace Private

static const auto &ContentTypeHeaderTypeToHeader = Private::___ContentTypeHeaderTypeToHeader();
static const auto &ContentTypeHeaderToHeaderType = Private::___ContentTypeHeaderToHeaderType();
static const auto &ContentTypeHeaderToExtension = Private::___ContentTypeHeaderToExtension();
static const auto &ContentTypeExtensionToHeader = Private::___ContentTypeExtensionToHeader();
static const auto &RequestMethodName = Private::___RequestMethodName();
static const auto &RequestMethodNameList = Private::___RequestMethodName().values();
static const auto &RequestMethodType = Private::___RequestMethodType();
static const auto &ProtocolName = Private::___ProtocolName();
static const auto &ProtocolType = Private::___ProtocolType();
static const auto &ProtocolUrlName = Private::___ProtocolUrlName();
static const auto &QSslProtocolToName = Private::___QSslProtocolToName();
static const auto &QSslProtocolNameToProtocol = Private::___QSslProtocolNameToProtocol();

} // namespace QRpc

namespace QRpc {
Q_DECLARE_FLAGS(Protocols, QRpc::Protocol);
} // namespace QRpc
Q_DECLARE_OPERATORS_FOR_FLAGS(QRpc::Protocols)
