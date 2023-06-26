#include "./qrpc_types.h"
#include "../../qstm/src/qstm_meta_enum.h"
#include "../../qstm/src/qstm_util_variant.h"
#include <QUrl>
#include <QHash>
#include <QCoreApplication>
#include <QStringList>
#include <QVariantList>
#include <QVariantHash>

namespace QRpc {

typedef QHash<QString, QRpc::Types::ContentType> ContentHashStrType;
typedef QHash<QRpc::Types::ContentType, QString> ContentHashTypeStr;

Q_GLOBAL_STATIC(ContentHashTypeStr, staticContentTypeHeader)
Q_GLOBAL_STATIC(ContentHashStrType, staticContentTypeExtension)


static void init()
{
    *staticContentTypeHeader={{QRpc::Types::AppNone, "application/x-www-form-urlencoded"},
                                                   {QRpc::Types::AppXML, "application/xml"},
                                                   {QRpc::Types::AppJson, "application/json"},
                                                   {QRpc::Types::AppCBOR, "application/cbor"},
                                                   {QRpc::Types::AppOctetStream, "application/octet-stream"},
                                                   {QRpc::Types::AppXwwwForm, "application/x-www-form-urlencoded"},
                                                   {QRpc::Types::AppPNG, "image/png"},
                                                   {QRpc::Types::AppJPGE, "image/jpg"},
                                                   {QRpc::Types::AppGIF, "image/gif"},
                                                   {QRpc::Types::AppPDF, "application/pdf"},
                                                   {QRpc::Types::AppTXT, "text/plain; charset=utf-8"},
                                                   {QRpc::Types::AppHTML, "text/html; charset=utf-8"},
                                                   {QRpc::Types::AppCSS, "text/css"},
                                                   {QRpc::Types::AppJS, "text/javascript"},
                                                   {QRpc::Types::AppSVG, "image/svg+xml"},
                                                   {QRpc::Types::AppWOFF, "font/woff"},
                                                   {QRpc::Types::AppWOFF2, "font/woff2"},
                                                   {QRpc::Types::AppTTF, "application/x-font-ttf"},
                                                   {QRpc::Types::AppEOT, "application/vnd.ms-fontobject"},
                                                   {QRpc::Types::AppOTF, "application/font-otf"},
                                };

    *staticContentTypeExtension={
                                  {"", QRpc::Types::AppOctetStream},
                                  {"xml", QRpc::Types::AppXML},
                                  {"json", QRpc::Types::AppJson},
                                  {"cbor", QRpc::Types::AppCBOR},
                                  {"png", QRpc::Types::AppPNG},
                                  {"jpg", QRpc::Types::AppJPGE},
                                  {"gif", QRpc::Types::AppGIF},
                                  {"pdf", QRpc::Types::AppPDF},
                                  {"txt", QRpc::Types::AppTXT},
                                  {"html", QRpc::Types::AppHTML},
                                  {"css", QRpc::Types::AppCSS},
                                  {"js", QRpc::Types::AppJS},
                                  {"svg", QRpc::Types::AppSVG},
                                  {"woff", QRpc::Types::AppWOFF},
                                  {"woff2", QRpc::Types::AppWOFF2},
                                  {"ttf", QRpc::Types::AppTTF},
                                  {"eot", QRpc::Types::AppEOT},
                                  {"otf", QRpc::Types::AppOTF},
                                  };
}

Q_COREAPP_STARTUP_FUNCTION(init)


Types::Types(QObject *parent)
    :QObject{parent}
{

}

Types::ContentType Types::contentType(const QVariant &v)
{
    if(!v.isValid() || v.isNull())
        return QRpc::Types::AppNone;

    QVariantList vList;
    switch (v.typeId()) {
    case QMetaType::QUrl:
    {
        QStm::MetaEnum<Types::ContentType> e;
        const auto &hash=*staticContentTypeExtension;
        auto url=v.toUrl();
        if(url.isLocalFile()){
            auto type=url.toLocalFile().split(QStringLiteral(".")).last().trimmed().toLower();
            QStm::MetaEnum<Types::ContentType> e=QVariant(
                hash.contains(type)
                    ?hash.value(type)
                    :int(Types::ContentType::AppNone)
                );
            return e.type();
        }
        break;
    }
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantPair:
        vList=v.toHash().values();
        break;
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        vList=v.toList();
        break;
    default:
        vList.append(v);
        break;
    }
    const auto &contentTypeHeader=*staticContentTypeHeader;
    for(auto &v:vList){
        QStm::MetaEnum<Types::ContentType> e=v;
        if(e.isValid())
            return e.type();
        else{
            auto t=contentTypeHeader.key(v.toString());
            if(t!=Types::ContentType::AppNone)
                return t;
        }
    }
    return Types::ContentType::AppNone;
}

QByteArray Types::contentTypeName(const QVariant &contentType)
{
    QStm::MetaEnum<AuthorizationScheme> e(Types::contentType(contentType));
    return e.name();
}

QString Types::contentTypeExtension(const QVariant &contentType)
{
    auto type=Types::contentType(contentType);
    const auto &hash=*staticContentTypeExtension;
    return hash.key(type);
}

QString Types::contentTypeHeader(const QVariant &contentType)
{
    auto type=Types::contentType(contentType);
    const auto &hash=*staticContentTypeHeader;
    return hash.value(type);
}

QVector<int> Types::contentTypeHeaders(const QVariant &contentType)
{
    if(contentType.isNull() || !contentType.isValid())
        return {};

    const auto &hash=*staticContentTypeHeader;
    QStringList vList;
    switch (contentType.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantPair:
    {
        Q_DECLARE_VU;
        auto vHash=contentType.toHash();
        QStringList vList;
        QHashIterator<QString,QVariant> i(vHash);
        while(i.hasNext()){
            i.next();
            auto sList=vu.toStringList(i.value());
            for(auto&v:sList)
                vList.append(v);
        }
        vList=vList.isEmpty()
                    ?QStringList{}
                    :vList;
        break;
    }
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    {
        for(auto&v:contentType.toList()){
            if(!v.isValid() || v.isNull())
                continue;
            vList.append(v.toString());
        }
        break;
    }
    default:
        vList.append(contentType.toString());
    }

    QVector<int> __return;
    for(auto &v:vList){
        auto keys=hash.keys(v);
        if(keys.isEmpty())
            continue;
        for(auto &key:keys)
            __return.append(key);
    }
    return __return;
}

QByteArray Types::authorizationSchemeName(const QVariant &authorizationScheme)
{
    QStm::MetaEnum<AuthorizationScheme> e=authorizationScheme;
    return e.name();
}

QByteArray Types::protocolName(const QVariant &protocol)
{
    QStm::MetaEnum<Protocol> e=protocol;
    return e.name();
}

QByteArray Types::methodName(const QVariant &method)
{
    QStm::MetaEnum<Method> e=method;
    return e.name();
}


//namespace Private {
//const QHash<ContentType, QString> &___ContentTypeHeaderTypeToHeader()
//{
//    static QHash<ContentType, QString> __return{{AppNone, "application/x-www-form-urlencoded"},
//                                                {AppXML, "application/xml"},
//                                                {AppJson, "application/json"},
//                                                {AppCBOR, "application/cbor"},
//                                                {AppOctetStream, "application/octet-stream"},
//                                                {AppXwwwForm, "application/x-www-form-urlencoded"},
//                                                {AppPNG, "image/png"},
//                                                {AppJPGE, "image/jpg"},
//                                                {AppGIF, "image/gif"},
//                                                {AppPDF, "application/pdf"},
//                                                {AppTXT, "text/plain; charset=utf-8"},
//                                                {AppHTML, "text/html; charset=utf-8"},
//                                                {AppCSS, "text/css"},
//                                                {AppJS, "text/javascript"},
//                                                {AppSVG, "image/svg+xml"},
//                                                {AppWOFF, "font/woff"},
//                                                {AppWOFF2, "font/woff2"},
//                                                {AppTTF, "application/x-font-ttf"},
//                                                {AppEOT, "application/vnd.ms-fontobject"},
//                                                {AppOTF, "application/font-otf"}};
//    return __return;
//}

//const QHash<QString, QRpc::Types::ContentType> &___ContentTypeHeaderToHeaderType()
//{
//    static QHash<QString, QRpc::Types::ContentType> __return{{"application/x-www-form-urlencoded", AppNone},
//                                                {"application/xml", AppXML},
//                                                {"application/json", AppJson},
//                                                {"application/cbor", AppCBOR},
//                                                {"application/octet-stream", AppOctetStream},
//                                                {"application/x-www-form-urlencoded", AppXwwwForm},
//                                                {"image/png", AppPNG},
//                                                {"image/jpg", AppJPGE},
//                                                {"image/gif", AppGIF},
//                                                {"application/pdf", AppPDF},
//                                                {"text/plain; charset=utf-8", AppTXT},
//                                                {"text/html; charset=utf-8", AppHTML},
//                                                {"text/css", AppCSS},
//                                                {"text/javascript", AppJS},
//                                                {"image/svg+xml", AppSVG},
//                                                {"font/woff", AppWOFF},
//                                                {"font/woff2", AppWOFF2},
//                                                {"application/x-font-ttf", AppTTF},
//                                                {"application/vnd.ms-fontobject", AppEOT},
//                                                {"application/font-otf", AppOTF}};
//    return __return;
//}

//const QHash<ContentType, QString> &___ContentTypeHeaderToExtension()
//{
//    static QHash<ContentType, QString> __return{{AppXML, "xml"},
//                                                {AppJson, "json"},
//                                                {AppCBOR, "cbor"},
//                                                {AppPNG, "png"},
//                                                {AppJPGE, "jpg"},
//                                                {AppGIF, "gif"},
//                                                {AppPDF, "pdf"},
//                                                {AppTXT, "txt"},
//                                                {AppHTML, "html"},
//                                                {AppCSS, "css"},
//                                                {AppJS, "js"},
//                                                {AppSVG, "svg"},
//                                                {AppWOFF, "woff"},
//                                                {AppWOFF2, "woff2"},
//                                                {AppTTF, "ttf"},
//                                                {AppEOT, "eot"},
//                                                {AppOTF, "otf"}};
//    return __return;
//}

//const QHash<QString, QRpc::Types::ContentType> &___ContentTypeExtensionToHeader()
//{
//    static QHash<QString, QRpc::Types::ContentType> __return{{"", AppOctetStream},
//                                                {"xml", AppXML},
//                                                {"json", AppJson},
//                                                {"cbor", AppCBOR},
//                                                {"png", AppPNG},
//                                                {"jpg", AppJPGE},
//                                                {"gif", AppGIF},
//                                                {"pdf", AppPDF},
//                                                {"txt", AppTXT},
//                                                {"html", AppHTML},
//                                                {"css", AppCSS},
//                                                {"js", AppJS},
//                                                {"svg", AppSVG},
//                                                {"woff", AppWOFF},
//                                                {"woff2", AppWOFF2},
//                                                {"ttf", AppTTF},
//                                                {"eot", AppEOT},
//                                                {"otf", AppOTF}};
//    return __return;
//}

//const QHash<int, QString> &___RequestMethodName()
//{
//    static QHash<int, QString> r{
//        {Head, QT_STRINGIFY2(Head)},
//        {Get, QT_STRINGIFY2(Get)},
//        {Post, QT_STRINGIFY2(Post)},
//        {Put, QT_STRINGIFY2(Put)},
//        {Delete, QT_STRINGIFY2(Delete)},
//        {Options, QT_STRINGIFY2(Options)},
//        {Patch, QT_STRINGIFY2(Patch)},
//        {Trace, QT_STRINGIFY2(Trace)},
//    };
//    return r;
//}

//const QHash<QString, RequestMethod> &___RequestMethodType()
//{
//    static QHash<QString, RequestMethod> r{{QT_STRINGIFY2(head), Head},
//                                           {QT_STRINGIFY2(get), Get},
//                                           {QT_STRINGIFY2(post), Post},
//                                           {QT_STRINGIFY2(put), Put},
//                                           {QT_STRINGIFY2(delete), Delete},
//                                           {QT_STRINGIFY2(options), Options},
//                                           {QT_STRINGIFY2(patch), Patch},
//                                           {QT_STRINGIFY2(trace), Trace},
//                                           {QT_STRINGIFY2(Head), Head},
//                                           {QT_STRINGIFY2(Get), Get},
//                                           {QT_STRINGIFY2(Post), Post},
//                                           {QT_STRINGIFY2(Put), Put},
//                                           {QT_STRINGIFY2(Delete), Delete},
//                                           {QT_STRINGIFY2(Options), Options},
//                                           {QT_STRINGIFY2(Patch), Patch},
//                                           {QT_STRINGIFY2(Trace), Trace}};
//    return r;
//}

//const QHash<QString, int> &___ProtocolType()
//{
//    static QHash<QString, int> r{{QT_STRINGIFY2(RPC), Protocol(0)},
//                                 {QT_STRINGIFY2(TcpSocket), TcpSocket},
//                                 {QT_STRINGIFY2(UdpSocket), UdpSocket},
//                                 {QT_STRINGIFY2(WebSocket), WebSocket},
//                                 {QT_STRINGIFY2(Mqtt), Mqtt},
//                                 {QT_STRINGIFY2(Amqp), Amqp},
//                                 {QT_STRINGIFY2(Http), Http},
//                                 {QT_STRINGIFY2(DataBase), DataBase},
//                                 {QT_STRINGIFY2(Kafka), Kafka},
//                                 {QT_STRINGIFY2(LocalSocket), LocalSocket},
//                                 {QT_STRINGIFY2(tcp), TcpSocket},
//                                 {QT_STRINGIFY2(udp), UdpSocket},
//                                 {QT_STRINGIFY2(wss), WebSocket},
//                                 {QT_STRINGIFY2(mqtt), Mqtt},
//                                 {QT_STRINGIFY2(amqp), Amqp},
//                                 {QT_STRINGIFY2(http), Http},
//                                 {QT_STRINGIFY2(database), DataBase},
//                                 {QT_STRINGIFY2(dataBase), DataBase},
//                                 {QT_STRINGIFY2(kafka), Kafka},
//                                 {QT_STRINGIFY2(LocalSocket), LocalSocket}};
//    return r;
//}

//const QHash<int, QString> &___ProtocolName()
//{
//    static QHash<int, QString> r{
//        {int(0), QT_STRINGIFY2(rpc)},
//        {TcpSocket, QT_STRINGIFY2(tcpsocket)},
//        {UdpSocket, QT_STRINGIFY2(udpsocket)},
//        {WebSocket, QT_STRINGIFY2(websocket)},
//        {Mqtt, QT_STRINGIFY2(mqtt)},
//        {Amqp, QT_STRINGIFY2(amqp)},
//        {Http, QT_STRINGIFY2(http)},
//        {DataBase, QT_STRINGIFY2(database)},
//        {LocalSocket, QT_STRINGIFY2(localsocket)},
//        {Kafka, QT_STRINGIFY2(kafka)},
//    };
//    return r;
//}

//const QHash<int, QString> &___ProtocolUrlName()
//{
//    static QHash<int, QString> r{{Protocol(0), QT_STRINGIFY2(rpc)},
//                                 {TcpSocket, QT_STRINGIFY2(tcp)},
//                                 {UdpSocket, QT_STRINGIFY2(udp)},
//                                 {WebSocket, QT_STRINGIFY2(wss)},
//                                 {Mqtt, QT_STRINGIFY2(mqtt)},
//                                 {Amqp, QT_STRINGIFY2(amqp)},
//                                 {Http, QT_STRINGIFY2(http)},
//                                 {DataBase, QT_STRINGIFY2(database)},
//                                 {LocalSocket, QT_STRINGIFY2(localsocket)},
//                                 {Kafka, QT_STRINGIFY2(dpkafka)}};
//    return r;
//}

//const QHash<int, QString> &___QSslProtocolToName()
//{
//    static QHash<int, QString> r
//    {
//#if QT_VERSION <= QT_VERSION_CHECK(5, 15, 0)
//        {QSsl::SslProtocol::SslV3, QT_STRINGIFY2(SslV3)},
//        {QSsl::SslProtocol::SslV2, QT_STRINGIFY2(SslV2)},
//        {QSsl::SslProtocol::TlsV1SslV3, QT_STRINGIFY2(TlsV1SslV3)},
//        {QSsl::SslProtocol::TlsV1_0, QT_STRINGIFY2(TlsV1_0)},
//        {QSsl::SslProtocol::TlsV1_1, QT_STRINGIFY2(TlsV1_1)},
//        {QSsl::SslProtocol::TlsV1_2, QT_STRINGIFY2(TlsV1_2)},
//        {QSsl::SslProtocol::TlsV1_3, QT_STRINGIFY2(TlsV1_3)},
//        {QSsl::SslProtocol::TlsV1_0OrLater, QT_STRINGIFY2(TlsV1_0OrLater)},
//        {QSsl::SslProtocol::TlsV1_1OrLater, QT_STRINGIFY2(TlsV1_1OrLater)},
//        {QSsl::SslProtocol::DtlsV1_0, QT_STRINGIFY2(DtlsV1_0)},
//        {QSsl::SslProtocol::DtlsV1_0OrLater, QT_STRINGIFY2(DtlsV1_0OrLater)},
//#endif
//            {QSsl::SslProtocol::TlsV1_2, QT_STRINGIFY2(TlsV1_2)},
//            {QSsl::SslProtocol::TlsV1_3, QT_STRINGIFY2(TlsV1_3)},
//            {QSsl::SslProtocol::AnyProtocol, QT_STRINGIFY2(AnyProtocol)},
//            {QSsl::SslProtocol::SecureProtocols, QT_STRINGIFY2(SecureProtocols)},
//            {QSsl::SslProtocol::TlsV1_2OrLater, QT_STRINGIFY2(TlsV1_2OrLater)},
//            {QSsl::SslProtocol::DtlsV1_2, QT_STRINGIFY2(DtlsV1_2)},
//            {QSsl::SslProtocol::DtlsV1_2OrLater, QT_STRINGIFY2(DtlsV1_2OrLater)},
//            {QSsl::SslProtocol::TlsV1_3OrLater, QT_STRINGIFY2(TlsV1_3OrLater)}
//    };
//    return r;
//}

//const QHash<QString, QSsl::SslProtocol> &___QSslProtocolNameToProtocol()
//{
//    static QHash<QString, QSsl::SslProtocol> r
//    {
//#if QT_VERSION <= QT_VERSION_CHECK(5, 15, 0)
//        {QString(QT_STRINGIFY2(SslV3)).toLower(), QSsl::SslProtocol::SslV3},
//            {QString(QT_STRINGIFY2(SslV2)).toLower(), QSsl::SslProtocol::SslV2},
//            {QString(QT_STRINGIFY2(TlsV1SslV3)).toLower(), QSsl::SslProtocol::TlsV1SslV3},
//            {QString(QT_STRINGIFY2(TlsV1_0)).toLower(), QSsl::SslProtocol::TlsV1_0},
//            {QString(QT_STRINGIFY2(TlsV1_1)).toLower(), QSsl::SslProtocol::TlsV1_1},
//            {QString(QT_STRINGIFY2(TlsV1_0OrLater)).toLower(), QSsl::SslProtocol::TlsV1_0OrLater},
//            {QString(QT_STRINGIFY2(TlsV1_1OrLater)).toLower(), QSsl::SslProtocol::TlsV1_1OrLater},
//            {QString(QT_STRINGIFY2(DtlsV1_0)).toLower(), QSsl::SslProtocol::DtlsV1_0},
//            {QString(QT_STRINGIFY2(DtlsV1_0OrLater)).toLower(), QSsl::SslProtocol::DtlsV1_0OrLater},
//#endif
//            {QString(QT_STRINGIFY2(TlsV1_2)).toLower(), QSsl::SslProtocol::TlsV1_2},
//            {QString(QT_STRINGIFY2(AnyProtocol)).toLower(), QSsl::SslProtocol::AnyProtocol},
//            {QString(QT_STRINGIFY2(SecureProtocols)).toLower(), QSsl::SslProtocol::SecureProtocols},
//            {QString(QT_STRINGIFY2(TlsV1_2OrLater)).toLower(), QSsl::SslProtocol::TlsV1_2OrLater},
//            {QString(QT_STRINGIFY2(DtlsV1_2)).toLower(), QSsl::SslProtocol::DtlsV1_2},
//            {QString(QT_STRINGIFY2(DtlsV1_2OrLater)).toLower(), QSsl::SslProtocol::DtlsV1_2OrLater},
//            {QString(QT_STRINGIFY2(TlsV1_3)).toLower(), QSsl::SslProtocol::TlsV1_3},
//        {
//            QString(QT_STRINGIFY2(TlsV1_3OrLater)).toLower(), QSsl::SslProtocol::TlsV1_3OrLater
//        }
//    };
//    return r;
//}
//} // namespace Private

} // namespace QRpc
