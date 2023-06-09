#include "./qrpc_types.h"

namespace QRpc {

namespace Private {
const QHash<ContentType, QString> &___ContentTypeHeaderTypeToHeader()
{
    static QHash<ContentType, QString> __return{{AppNone, "application/x-www-form-urlencoded"},
                                                {AppXML, "application/xml"},
                                                {AppJson, "application/json"},
                                                {AppCBOR, "application/cbor"},
                                                {AppOctetStream, "application/octet-stream"},
                                                {AppXwwwForm, "application/x-www-form-urlencoded"},
                                                {AppPNG, "image/png"},
                                                {AppJPGE, "image/jpg"},
                                                {AppGIF, "image/gif"},
                                                {AppPDF, "application/pdf"},
                                                {AppTXT, "text/plain; charset=utf-8"},
                                                {AppHTML, "text/html; charset=utf-8"},
                                                {AppCSS, "text/css"},
                                                {AppJS, "text/javascript"},
                                                {AppSVG, "image/svg+xml"},
                                                {AppWOFF, "font/woff"},
                                                {AppWOFF2, "font/woff2"},
                                                {AppTTF, "application/x-font-ttf"},
                                                {AppEOT, "application/vnd.ms-fontobject"},
                                                {AppOTF, "application/font-otf"}};
    return __return;
}

const QHash<QString, ContentType> &___ContentTypeHeaderToHeaderType()
{
    static QHash<QString, ContentType> __return{{"application/x-www-form-urlencoded", AppNone},
                                                {"application/xml", AppXML},
                                                {"application/json", AppJson},
                                                {"application/cbor", AppCBOR},
                                                {"application/octet-stream", AppOctetStream},
                                                {"application/x-www-form-urlencoded", AppXwwwForm},
                                                {"image/png", AppPNG},
                                                {"image/jpg", AppJPGE},
                                                {"image/gif", AppGIF},
                                                {"application/pdf", AppPDF},
                                                {"text/plain; charset=utf-8", AppTXT},
                                                {"text/html; charset=utf-8", AppHTML},
                                                {"text/css", AppCSS},
                                                {"text/javascript", AppJS},
                                                {"image/svg+xml", AppSVG},
                                                {"font/woff", AppWOFF},
                                                {"font/woff2", AppWOFF2},
                                                {"application/x-font-ttf", AppTTF},
                                                {"application/vnd.ms-fontobject", AppEOT},
                                                {"application/font-otf", AppOTF}};
    return __return;
}

const QHash<ContentType, QString> &___ContentTypeHeaderToExtension()
{
    static QHash<ContentType, QString> __return{{AppXML, "xml"},
                                                {AppJson, "json"},
                                                {AppCBOR, "cbor"},
                                                {AppPNG, "png"},
                                                {AppJPGE, "jpg"},
                                                {AppGIF, "gif"},
                                                {AppPDF, "pdf"},
                                                {AppTXT, "txt"},
                                                {AppHTML, "html"},
                                                {AppCSS, "css"},
                                                {AppJS, "js"},
                                                {AppSVG, "svg"},
                                                {AppWOFF, "woff"},
                                                {AppWOFF2, "woff2"},
                                                {AppTTF, "ttf"},
                                                {AppEOT, "eot"},
                                                {AppOTF, "otf"}};
    return __return;
}

const QHash<QString, ContentType> &___ContentTypeExtensionToHeader()
{
    static QHash<QString, ContentType> __return{{"", AppOctetStream},
                                                {"xml", AppXML},
                                                {"json", AppJson},
                                                {"cbor", AppCBOR},
                                                {"png", AppPNG},
                                                {"jpg", AppJPGE},
                                                {"gif", AppGIF},
                                                {"pdf", AppPDF},
                                                {"txt", AppTXT},
                                                {"html", AppHTML},
                                                {"css", AppCSS},
                                                {"js", AppJS},
                                                {"svg", AppSVG},
                                                {"woff", AppWOFF},
                                                {"woff2", AppWOFF2},
                                                {"ttf", AppTTF},
                                                {"eot", AppEOT},
                                                {"otf", AppOTF}};
    return __return;
}

const QHash<int, QString> &___RequestMethodName()
{
    static QHash<int, QString> r{
        {Head, QT_STRINGIFY2(Head)},
        {Get, QT_STRINGIFY2(Get)},
        {Post, QT_STRINGIFY2(Post)},
        {Put, QT_STRINGIFY2(Put)},
        {Delete, QT_STRINGIFY2(Delete)},
        {Options, QT_STRINGIFY2(Options)},
        {Patch, QT_STRINGIFY2(Patch)},
        {Trace, QT_STRINGIFY2(Trace)},
    };
    return r;
}

const QHash<QString, RequestMethod> &___RequestMethodType()
{
    static QHash<QString, RequestMethod> r{{QT_STRINGIFY2(head), Head},
                                           {QT_STRINGIFY2(get), Get},
                                           {QT_STRINGIFY2(post), Post},
                                           {QT_STRINGIFY2(put), Put},
                                           {QT_STRINGIFY2(delete), Delete},
                                           {QT_STRINGIFY2(options), Options},
                                           {QT_STRINGIFY2(patch), Patch},
                                           {QT_STRINGIFY2(trace), Trace},
                                           {QT_STRINGIFY2(Head), Head},
                                           {QT_STRINGIFY2(Get), Get},
                                           {QT_STRINGIFY2(Post), Post},
                                           {QT_STRINGIFY2(Put), Put},
                                           {QT_STRINGIFY2(Delete), Delete},
                                           {QT_STRINGIFY2(Options), Options},
                                           {QT_STRINGIFY2(Patch), Patch},
                                           {QT_STRINGIFY2(Trace), Trace}};
    return r;
}

const QHash<QString, int> &___ProtocolType()
{
    static QHash<QString, int> r{{QT_STRINGIFY2(RPC), Protocol(0)},
                                 {QT_STRINGIFY2(TcpSocket), TcpSocket},
                                 {QT_STRINGIFY2(UdpSocket), UdpSocket},
                                 {QT_STRINGIFY2(WebSocket), WebSocket},
                                 {QT_STRINGIFY2(Mqtt), Mqtt},
                                 {QT_STRINGIFY2(Amqp), Amqp},
                                 {QT_STRINGIFY2(Http), Http},
                                 {QT_STRINGIFY2(DataBase), DataBase},
                                 {QT_STRINGIFY2(Kafka), Kafka},
                                 {QT_STRINGIFY2(LocalSocket), LocalSocket},
                                 {QT_STRINGIFY2(tcp), TcpSocket},
                                 {QT_STRINGIFY2(udp), UdpSocket},
                                 {QT_STRINGIFY2(wss), WebSocket},
                                 {QT_STRINGIFY2(mqtt), Mqtt},
                                 {QT_STRINGIFY2(amqp), Amqp},
                                 {QT_STRINGIFY2(http), Http},
                                 {QT_STRINGIFY2(database), DataBase},
                                 {QT_STRINGIFY2(dataBase), DataBase},
                                 {QT_STRINGIFY2(kafka), Kafka},
                                 {QT_STRINGIFY2(LocalSocket), LocalSocket}};
    return r;
}

const QHash<int, QString> &___ProtocolName()
{
    static QHash<int, QString> r{
        {int(0), QT_STRINGIFY2(rpc)},
        {TcpSocket, QT_STRINGIFY2(tcpsocket)},
        {UdpSocket, QT_STRINGIFY2(udpsocket)},
        {WebSocket, QT_STRINGIFY2(websocket)},
        {Mqtt, QT_STRINGIFY2(mqtt)},
        {Amqp, QT_STRINGIFY2(amqp)},
        {Http, QT_STRINGIFY2(http)},
        {DataBase, QT_STRINGIFY2(database)},
        {LocalSocket, QT_STRINGIFY2(localsocket)},
        {Kafka, QT_STRINGIFY2(kafka)},
    };
    return r;
}

const QHash<int, QString> &___ProtocolUrlName()
{
    static QHash<int, QString> r{{Protocol(0), QT_STRINGIFY2(rpc)},
                                 {TcpSocket, QT_STRINGIFY2(tcp)},
                                 {UdpSocket, QT_STRINGIFY2(udp)},
                                 {WebSocket, QT_STRINGIFY2(wss)},
                                 {Mqtt, QT_STRINGIFY2(mqtt)},
                                 {Amqp, QT_STRINGIFY2(amqp)},
                                 {Http, QT_STRINGIFY2(http)},
                                 {DataBase, QT_STRINGIFY2(database)},
                                 {LocalSocket, QT_STRINGIFY2(localsocket)},
                                 {Kafka, QT_STRINGIFY2(dpkafka)}};
    return r;
}

const QHash<int, QString> &___QSslProtocolToName()
{
    static QHash<int, QString> r
    {
#if QT_VERSION <= QT_VERSION_CHECK(5, 15, 0)
        {QSsl::SslProtocol::SslV3, QT_STRINGIFY2(SslV3)},
        {QSsl::SslProtocol::SslV2, QT_STRINGIFY2(SslV2)},
        {QSsl::SslProtocol::TlsV1SslV3, QT_STRINGIFY2(TlsV1SslV3)},
        {QSsl::SslProtocol::TlsV1_0, QT_STRINGIFY2(TlsV1_0)},
        {QSsl::SslProtocol::TlsV1_1, QT_STRINGIFY2(TlsV1_1)},
        {QSsl::SslProtocol::TlsV1_2, QT_STRINGIFY2(TlsV1_2)},
        {QSsl::SslProtocol::TlsV1_3, QT_STRINGIFY2(TlsV1_3)},
        {QSsl::SslProtocol::TlsV1_0OrLater, QT_STRINGIFY2(TlsV1_0OrLater)},
        {QSsl::SslProtocol::TlsV1_1OrLater, QT_STRINGIFY2(TlsV1_1OrLater)},
        {QSsl::SslProtocol::DtlsV1_0, QT_STRINGIFY2(DtlsV1_0)},
        {QSsl::SslProtocol::DtlsV1_0OrLater, QT_STRINGIFY2(DtlsV1_0OrLater)},
#endif
            {QSsl::SslProtocol::TlsV1_2, QT_STRINGIFY2(TlsV1_2)},
            {QSsl::SslProtocol::TlsV1_3, QT_STRINGIFY2(TlsV1_3)},
            {QSsl::SslProtocol::AnyProtocol, QT_STRINGIFY2(AnyProtocol)},
            {QSsl::SslProtocol::SecureProtocols, QT_STRINGIFY2(SecureProtocols)},
            {QSsl::SslProtocol::TlsV1_2OrLater, QT_STRINGIFY2(TlsV1_2OrLater)},
            {QSsl::SslProtocol::DtlsV1_2, QT_STRINGIFY2(DtlsV1_2)},
            {QSsl::SslProtocol::DtlsV1_2OrLater, QT_STRINGIFY2(DtlsV1_2OrLater)},
            {QSsl::SslProtocol::TlsV1_3OrLater, QT_STRINGIFY2(TlsV1_3OrLater)}
    };
    return r;
}

const QHash<QString, QSsl::SslProtocol> &___QSslProtocolNameToProtocol()
{
    static QHash<QString, QSsl::SslProtocol> r
    {
#if QT_VERSION <= QT_VERSION_CHECK(5, 15, 0)
        {QString(QT_STRINGIFY2(SslV3)).toLower(), QSsl::SslProtocol::SslV3},
            {QString(QT_STRINGIFY2(SslV2)).toLower(), QSsl::SslProtocol::SslV2},
            {QString(QT_STRINGIFY2(TlsV1SslV3)).toLower(), QSsl::SslProtocol::TlsV1SslV3},
            {QString(QT_STRINGIFY2(TlsV1_0)).toLower(), QSsl::SslProtocol::TlsV1_0},
            {QString(QT_STRINGIFY2(TlsV1_1)).toLower(), QSsl::SslProtocol::TlsV1_1},
            {QString(QT_STRINGIFY2(TlsV1_0OrLater)).toLower(), QSsl::SslProtocol::TlsV1_0OrLater},
            {QString(QT_STRINGIFY2(TlsV1_1OrLater)).toLower(), QSsl::SslProtocol::TlsV1_1OrLater},
            {QString(QT_STRINGIFY2(DtlsV1_0)).toLower(), QSsl::SslProtocol::DtlsV1_0},
            {QString(QT_STRINGIFY2(DtlsV1_0OrLater)).toLower(), QSsl::SslProtocol::DtlsV1_0OrLater},
#endif
            {QString(QT_STRINGIFY2(TlsV1_2)).toLower(), QSsl::SslProtocol::TlsV1_2},
            {QString(QT_STRINGIFY2(AnyProtocol)).toLower(), QSsl::SslProtocol::AnyProtocol},
            {QString(QT_STRINGIFY2(SecureProtocols)).toLower(), QSsl::SslProtocol::SecureProtocols},
            {QString(QT_STRINGIFY2(TlsV1_2OrLater)).toLower(), QSsl::SslProtocol::TlsV1_2OrLater},
            {QString(QT_STRINGIFY2(DtlsV1_2)).toLower(), QSsl::SslProtocol::DtlsV1_2},
            {QString(QT_STRINGIFY2(DtlsV1_2OrLater)).toLower(), QSsl::SslProtocol::DtlsV1_2OrLater},
            {QString(QT_STRINGIFY2(TlsV1_3)).toLower(), QSsl::SslProtocol::TlsV1_3},
        {
            QString(QT_STRINGIFY2(TlsV1_3OrLater)).toLower(), QSsl::SslProtocol::TlsV1_3OrLater
        }
    };
    return r;
}
} // namespace Private

} // namespace QRpc
