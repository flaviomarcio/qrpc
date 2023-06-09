#include "./p_qrpc_http_headers.h"
#include "./p_qrpc_util.h"
#include "../qrpc_macro.h"
#include <QJsonDocument>
#include <QStm>

namespace QRpc {

static const auto staticDefaultContentType=QRpc::AppJson;

class HttpHeadersPvt:public QObject{
public:

    QObject *parent=nullptr;
    QVariantHash header;

    explicit HttpHeadersPvt(HttpHeaders*parent):QObject{parent}
    {
        this->parent=parent;
    }


    QVariant header_v(const QString &key)const
    {
        auto vkey=key.trimmed().toLower();
        QHashIterator<QString, QVariant> i(this->header);
        QStringList vList;
        while (i.hasNext()) {
            i.next();

            if(i.key().trimmed().toLower()!=vkey)
                continue;

            auto &v=i.value();
            switch (v.typeId()) {
            case QMetaType::QStringList:
            case QMetaType::QVariantList:
            {
                for(auto &v:v.toList())
                    vList.append(v.toString());
                break;
            }
            default:
                vList.append(v.toString());
            }
        }

        if(vList.isEmpty())
            return {};

        return vList;
    }
};

HttpHeaders::HttpHeaders(QObject *parent):QObject{parent}
{
    this->p = new HttpHeadersPvt{this};
    p->parent=parent;
}

HttpHeaders::HttpHeaders(const QVariant &v, QObject *parent):QObject{parent}
{
    this->p = new HttpHeadersPvt{this};

    p->parent=parent;
    Q_DECLARE_VU;
    p->header=vu.toHash(v);
}


HttpHeaders &HttpHeaders::clear()
{

    p->header.clear();
    return *this;
}

QVariantHash &HttpHeaders::rawHeader()const
{

    return p->header;
}

QVariant HttpHeaders::rawHeader(const QString &headername)const
{

    QStringList returnList;
    QHashIterator<QString, QVariant> i(p->header);
    while (i.hasNext()) {
        i.next();

        if(i.key().toLower()!=headername.toLower())
            continue;

        auto &v=i.value();
        QStringList vList;
        switch (v.typeId()) {
        case QMetaType::QStringList:
        case QMetaType::QVariantList:
            vList=v.toStringList();
            break;
        default:
            vList=v.toString().split(QStringLiteral(";"));
        }

        for(auto &header:vList)
            returnList.append(header);
    }
    return QVariant(returnList);
}

HttpHeaders &HttpHeaders::setRawHeader(const QVariantHash &rawHeader)
{

    auto &header=p->header;
    header.clear();
    QHashIterator<QString, QVariant> i(rawHeader);
    while (i.hasNext()) {
        i.next();
        const auto key=QRpc::Util::headerFormatName(i.key());
        const auto value=i.value();
        this->setRawHeader(key,value);
    }
    return *this;
}

HttpHeaders &HttpHeaders::setRawHeader(const QString &header, const QVariant &value)
{
    QVariantList list;
    switch (value.typeId()) {
    case QMetaType::QStringList:
    case QMetaType::QVariantList:
    {
        for(auto &v:value.toList())
            list.append(v);
        break;
    }
    default:
        list.append(value);
    }


    const auto headerName=QRpc::Util::headerFormatName(header);
    auto vList=p->header[headerName].toStringList();
    vList.clear();
    for(auto &v:list){
        auto vv=v.toByteArray().trimmed();
        if(vv.isEmpty())
            continue;

        if(!vList.contains(vv))
            vList.append(vv.trimmed());
    }

    for(auto &v:vList){
        auto x=v;
        if(x.startsWith(QT_STRINGIFY(Basic)) || x.startsWith(QT_STRINGIFY(basic))){
            auto x_toUtf8 = x.replace(QT_STRINGIFY(Basic), "").replace(QT_STRINGIFY(basic), "").trimmed().toUtf8();
            auto x_from=QByteArray::fromBase64(x_toUtf8);
            if(x_from.toBase64()!=x_toUtf8){
                v=QT_STRINGIFY(Basic)+QStringLiteral(" ")+x_toUtf8.toBase64();
            }
        }
    }

    p->header[headerName]=vList;
    return *this;
}

HttpHeaders &HttpHeaders::addRawHeader(const QVariantHash &rawHeader)
{
    Q_DECLARE_VU;
    QHashIterator<QString, QVariant> i(rawHeader);
    while (i.hasNext()) {
        i.next();
        this->addRawHeader(i.key(), i.value());
    }
    return *this;
}

HttpHeaders &HttpHeaders::addRawHeader(const QString &header, const QVariant &value)
{
    QVariantList list;
    switch (value.typeId()) {
    case QMetaType::QStringList:
    case QMetaType::QVariantList:
    {
        for(auto &v:value.toList())
            list.append(v);
        break;
    }
    default:
        list.append(value);
    }


    const auto headerName=QRpc::Util::headerFormatName(header);
    auto vList=p->header.value(headerName).toStringList();
    vList.clear();
    for(auto &v:list){
        auto vv=v.toByteArray().trimmed();

        if(vv.isEmpty())
            continue;

        if(vList.contains(vv))
            continue;

        vList.append(vv.trimmed());
    }
    this->setRawHeader(headerName, vList);
    return *this;
}

HttpHeaders &HttpHeaders::setContentType(const int contentType)
{
    auto content_type=ContentType(contentType);

    if(ContentTypeHeaderTypeToHeader.contains(content_type))
        return this->setContentType(ContentTypeHeaderTypeToHeader.value(content_type));

    return this->setContentType(ContentTypeHeaderTypeToHeader.value(QRpc::AppOctetStream));
}

HttpHeaders &HttpHeaders::setContentType(const QVariant &v)
{

    p->header.remove(ContentTypeName);
    p->header.remove(QString(ContentTypeName).toLower());
    QVariant value=v;
    if(v.typeId()==QMetaType::QUrl){
        value={};
        auto url=v.toUrl();
        if(url.isLocalFile()){
            auto ext=url.toLocalFile().split(QStringLiteral(".")).last().trimmed();
            if(!ContentTypeExtensionToHeader.contains(ext.toLower()))
                ext.clear();
            auto type=ContentTypeExtensionToHeader.value(ext.toLower());
            value=ContentTypeHeaderTypeToHeader.value(type);
        }
    }
    if(value.isValid())
        p->header.insert(ContentTypeName,value);
    return *this;
}

bool HttpHeaders::isContentType(int contentType) const
{

    const auto contenttype=p->header_v(ContentTypeName).toStringList();
    for(auto &ct:contenttype){
        int cType=-1;
        if(!ContentTypeHeaderToHeaderType.contains(ct))
            continue;
        cType=ContentTypeHeaderToHeaderType.value(ct);
        if(contentType!=cType)
            continue;
        return true;
    }
    return false;
}

QVariant HttpHeaders::contentType() const
{

    auto v=p->header_v(ContentTypeName);
    return v;
}

ContentType HttpHeaders::defaultContentType()
{
    return staticDefaultContentType;
}

QVariant HttpHeaders::contentDisposition() const
{

    auto v=p->header_v(ContentDispositionName);
    return v;
}

HttpHeaders &HttpHeaders::setAuthorization(const QString &authorization, const QString &type, const QVariant &credentials)
{
    QString scredentials;

    switch (credentials.typeId()) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantList:
    {
        QStringList params;
        QHashIterator<QString, QVariant> i(credentials.toHash());
        while (i.hasNext()) {
            i.next();
            params.append(QStringLiteral("%1=%2").arg(i.key(),i.value().toString()));
        }
        scredentials=params.join(QStringLiteral(" "));
        break;
    }
    default:
        scredentials=credentials.toString().trimmed();
    }

    if(!type.isEmpty() && !scredentials.isEmpty())
        this->setRawHeader(authorization.toUtf8(), QStringLiteral("%1 %2").arg(type,scredentials).toUtf8());
    return *this;
}

HttpHeaders &HttpHeaders::setAuthorization(const QString &authorization, const AuthorizationType &type, const QVariant &credentials)
{
    QString stype;
    switch (type) {
    case Basic:
        stype=QStringLiteral("Basic");
        break;
    case Bearer:
        stype=QStringLiteral("Bearer");
        break;
    case Digest:
        stype=QStringLiteral("Digest");
        break;
    case HOBA:
        stype=QStringLiteral("HOBA");
        break;
    case Mutual:
        stype=QStringLiteral("Mutual");
        break;
    case AWS4_HMAC_SHA256:
        stype=QStringLiteral("AWS4_HMAC_SHA256");
        break;
    case Service:
        stype=QStringLiteral("Service");
        break;
    default:
        break;
    }

    if(type!=Basic)
        return this->setAuthorization(authorization, stype, credentials);

    auto v=QByteArray::fromBase64(credentials.toByteArray());
    if(!v.isEmpty())
        return this->setAuthorization(authorization, stype, credentials);

    v=credentials.toByteArray().toBase64();
    return this->setAuthorization(authorization, stype, v);
}


HttpHeaders &HttpHeaders::setAuthorization(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(QStringLiteral("Authorization"), type, credentials);
}

HttpHeaders &HttpHeaders::setAuthorization(const AuthorizationType &type, const QVariant &userName, const QVariant &passWord)
{
    auto credentials=QStringLiteral("%1:%2").arg(userName.toString(), passWord.toString()).toUtf8().toBase64();
    return this->setAuthorization(type, credentials);
}

HttpHeaders &HttpHeaders::setProxyAuthorization(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(QStringLiteral("Proxy-Authorization"), type, credentials);
}

HttpHeaders &HttpHeaders::setProxyAuthorization(const QString &type, const QVariant &credentials)
{
    return this->setAuthorization(QStringLiteral("Proxy-Authorization"), type, credentials);
}

HttpHeaders &HttpHeaders::setWWWAuthenticate(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(QStringLiteral("WWW-Authenticate"), type, credentials);
}

HttpHeaders &HttpHeaders::setWWWAuthenticate(const QString &type, const QVariant &credentials)
{
    return this->setAuthorization(QStringLiteral("WWW-Authenticate"), type, credentials);
}

QVariant HttpHeaders::cookies()const
{
    return this->rawHeader(QStringLiteral("Cookie"));
}

HttpHeaders &HttpHeaders::setCookies(const QVariant &cookie)
{
    return this->setRawHeader(QStringLiteral("Cookie"), cookie);
}

QVariant HttpHeaders::authorization(const QString &authorization, const QString &type)
{

    QVariantList returnList;
    QHashIterator<QString, QVariant> i(p->header);
    while (i.hasNext()) {
        i.next();
        if(i.key().toLower()!=authorization.toLower())
            continue;
        auto list=i.value().toString().split(QStringLiteral(" "));
        if(list.size()<=1)
            continue;


        if(type.toLower()!=list.first())
            continue;

        list.takeFirst();
        for(auto &v:list){
            if(v.contains(QStringLiteral("="))){
                auto sp=v.split(QStringLiteral("="));
                QVariantHash map;
                map.insert(sp.at(0),sp.at(1));
                returnList.append(map);
                continue;
            }

            if(!returnList.contains(v)){
                returnList.append(v);
                continue;
            }
        }
    }
    return returnList.size()==1?returnList.first():returnList;
}

QVariant HttpHeaders::authorization(const QString &authorization, const AuthorizationType &type)
{
    QString stype;
    switch (type) {
    case Basic:
        stype=QStringLiteral("Basic");
        break;
    case Bearer:
        stype=QStringLiteral("Bearer");
        break;
    case Digest:
        stype=QStringLiteral("Digest");
        break;
    case HOBA:
        stype=QStringLiteral("HOBA");
        break;
    case Mutual:
        stype=QStringLiteral("Mutual");
        break;
    case AWS4_HMAC_SHA256:
        stype=QStringLiteral("AWS4_HMAC_SHA256");
        break;
    default:
        break;
    }
    return this->authorization(authorization, stype);
}

QVariant HttpHeaders::authorization(const AuthorizationType &type)
{
    return this->authorization(QStringLiteral("Authorization"), type);
}

QVariant HttpHeaders::authorization(const QString &type)
{
    return this->authorization(QStringLiteral("Authorization"), type);
}

QVariant HttpHeaders::proxyAuthorization(const AuthorizationType &type)
{
    return this->authorization(QStringLiteral("Proxy-Authorization"), type);
}

QVariant HttpHeaders::proxyAuthorization(const QString &type)
{
    return this->authorization(QStringLiteral("Proxy-Authorization"), type);
}

QVariant HttpHeaders::wwwAuthenticate(const AuthorizationType &type)
{
    return this->authorization(QStringLiteral("WWW-Authorization"), type);
}

QVariant HttpHeaders::wwwAuthenticate(const QString &type)
{
    return this->authorization(QStringLiteral("WWW-Authorization"), type);
}

HttpHeaders &HttpHeaders::print(const QString &output)
{
    for(auto &v:this->printOut(output))
        rInfo()<<v;
    return *this;
}

QStringList HttpHeaders::printOut(const QString &output)
{
    auto space=output.trimmed().isEmpty()?"":QStringLiteral("    ");
    QStringList __return;
    Q_DECLARE_VU;
    auto vHash=this->rawHeader();
    if(!vHash.isEmpty()){
        __return.append(QStringLiteral("%1%2 headers").arg(space, output).trimmed());
        QHashIterator<QString, QVariant> i(vHash);
        while (i.hasNext()){
            i.next();
            __return.append(QStringLiteral("%1     %2:%3").arg(space, i.key(), vu.toStr(i.value())));
        }
    }
    return __return;
}

HttpHeaders &HttpHeaders::operator=(const QVariant &v)
{

    p->header.clear();
    QVariantHash vHash;
    switch (v.typeId()) {
    case QMetaType::QString:
    case QMetaType::QByteArray:
        vHash=QJsonDocument::fromJson(v.toByteArray()).toVariant().toHash();
        break;
    default:
        vHash=v.toHash();
    }
    return this->setRawHeader(vHash);
}

HttpHeaders &HttpHeaders::operator<<(const QVariant &v)
{
    QVariantHash vHash;
    switch (v.typeId()) {
    case QMetaType::QString:
    case QMetaType::QByteArray:
        vHash=QJsonDocument::fromJson(v.toByteArray()).toVariant().toHash();
        break;
    default:
        vHash=v.toHash();
    }
    QHashIterator<QString, QVariant> i(vHash);
    while (i.hasNext()) {
        i.next();
        this->addRawHeader(i.key(), i.value());
    }
    return *this;
}

}
