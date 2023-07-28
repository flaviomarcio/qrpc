#include "./p_qrpc_http_headers.h"
#include "./p_qrpc_util.h"
#include "../qrpc_macro.h"
#include "../../../qstm/src/qstm_util_variant.h"
#include "../../../qstm/src/qstm_meta_enum.h"
#include <QJsonDocument>

namespace QRpc {

static const auto __header_authorization="Authorization";
static const auto __header_proxy_authorization="Proxy-Authorization";
static const auto __header_www_authenticate="WWW-Authenticate";
static const auto __header_cookie="Cookie";
static const auto __equal="=";
static const auto __space=" ";
static const auto __null_str="";
static const auto staticDefaultContentType=QRpc::Types::ContentType::AppJson;

class HttpHeadersPvt:public QObject{
public:

    QObject *parent=nullptr;
    QVariantHash header;

    explicit HttpHeadersPvt(HttpHeaders *parent, const QVariant &headers):QObject{parent}, parent{parent}
    {
        if(!headers.isNull() && headers.isValid()){
            Q_DECLARE_VU;
            this->header=vu.toHash(headers);
        }
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

HttpHeaders::HttpHeaders(QObject *parent):QObject{parent},p{new HttpHeadersPvt{this,{}}}
{
}

HttpHeaders::HttpHeaders(const QVariant &headers, QObject *parent):QObject{parent}, p{new HttpHeadersPvt{this, headers}}
{

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


HttpHeaders &HttpHeaders::clear()
{
    p->header.clear();
    return *this;
}

HttpHeaders &HttpHeaders::print(const QString &output)
{
    for(auto &v:this->printOut(output))
        rWarning()<<v;
    return *this;
}

QStringList HttpHeaders::printOut(const QString &output)
{
    auto space=output.trimmed().isEmpty()?__null_str:QStringLiteral("    ");
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
            auto x_toUtf8 = x.replace(QT_STRINGIFY(Basic), __null_str).replace(QT_STRINGIFY(basic), __null_str).trimmed().toUtf8();
            auto x_from=QByteArray::fromBase64(x_toUtf8);
            if(x_from.toBase64()!=x_toUtf8)
                v=QT_STRINGIFY(Basic)+QByteArray(__space)+x_toUtf8.toBase64();
        }
    }

    p->header.insert(headerName, vList);
    return *this;
}

HttpHeaders &HttpHeaders::rawHeaderRemove(const QString &headerName)
{
    auto keys=p->header.keys();
    for(auto&key:keys){
        if(key.toLower().trimmed()==headerName.trimmed().toLower())
            p->header.remove(key);
    }
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

HttpHeaders &HttpHeaders::setContentType(const QVariant &v)
{
    auto value=Types::contentTypeHeader(v);
    if(!value.isEmpty())
        p->header.insert(ContentTypeName,value);
    return *this;
}

bool HttpHeaders::isContentType(const QVariant &contentType) const
{
    auto contentTypes=Types::contentTypeHeaders(p->header_v(ContentTypeName));
    if(contentTypes.isEmpty())
        return false;

    QStm::MetaEnum<Types::ContentType> e=contentType;
    if(!contentTypes.contains(e.type()))
        return false;
    return true;
}

QVariant HttpHeaders::contentType() const
{
    return p->header_v(ContentTypeName);
}

Types::ContentType HttpHeaders::defaultContentType()
{
    return staticDefaultContentType;
}

QVariant HttpHeaders::contentDisposition() const
{
    return p->header_v(ContentDispositionName);
}

HttpHeaders &HttpHeaders::setAuthorization(const QString &headerName, const QVariant &authorizationScheme, const QVariant &credentials)
{
    QString scredentials;
    QStm::MetaEnum<QRpc::Types::AuthorizationScheme> e=authorizationScheme;

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
        scredentials=params.join(__space);
        break;
    }
    default:
        scredentials=credentials.toString().trimmed();
    }

    return this->setRawHeader(headerName.toUtf8(), QStringLiteral("%1 %2").arg(e.name(),scredentials).toUtf8());
}

//HttpHeaders &HttpHeaders::setAuthorization(const QString &authorization, const QVariant &authType, const QVariant &credentials)
//{
//    QStm::MetaEnum<QRpc::Types::AuthorizationScheme> e=authType;
//    if(!e.e(Types::Basic))
//        return this->setAuthorization(authorization, stype, credentials);
//    auto v=QByteArray::fromBase64(credentials.toByteArray());
//    if(!v.isEmpty())
//        return this->setAuthorization(authorization, stype, credentials);
//    v=credentials.toByteArray().toBase64();
//    return this->setAuthorization(authorization, stype, v);
//}

HttpHeaders &HttpHeaders::setAuthorization(const QVariant &authorizationScheme, const QVariant &credentials)
{
    return this->setAuthorization(__header_authorization, authorizationScheme, credentials);
}

HttpHeaders &HttpHeaders::setAuthorizationBearer(const QVariant &token)
{
    return this->setAuthorization(QRpc::Types::Bearer, token);
}

HttpHeaders &HttpHeaders::setAuthorizationService(const QVariant &token)
{
    return this->setAuthorization(QRpc::Types::Service, token);
}

HttpHeaders &HttpHeaders::setAuthorizationBasic(const QVariant &base64Encode)
{
    return this->setAuthorization(QRpc::Types::Service, base64Encode);
}

HttpHeaders &HttpHeaders::setAuthorizationBasic(const QVariant &userName, const QVariant &passWord)
{
    static const auto __format=QStringLiteral("%1:%2");
    auto credentials=__format.arg(userName.toString(), passWord.toString()).toUtf8().toBase64();
    return this->setAuthorization(QRpc::Types::Basic, credentials);
}

HttpHeaders &HttpHeaders::setProxyAuthorization(const QVariant &authorizationScheme, const QVariant &credentials)
{
    return this->setAuthorization(__header_proxy_authorization, authorizationScheme, credentials);
}

HttpHeaders &HttpHeaders::setWWWAuthenticate(const QVariant &authorizationScheme, const QVariant &credentials)
{
    return this->setAuthorization(__header_www_authenticate, authorizationScheme, credentials);
}

QVariant HttpHeaders::authorization(const QString &headerName, const QVariant &authorizationScheme)
{
    QStm::MetaEnum<QRpc::Types::AuthorizationScheme> e=authorizationScheme;
    QVariantList returnList;
    QHashIterator<QString, QVariant> i(p->header);
    while (i.hasNext()) {
        i.next();

        if(i.key().toLower()!=headerName.toLower())
            continue;

        auto list=i.value().toString().split(__space);
        if(list.size()<=1)
            continue;


        if(e.name().toLower()!=list.first())
            continue;

        list.takeFirst();
        for(auto &v:list){
            if(v.contains(__equal)){
                auto sp=v.split(__equal);
                returnList.append(QVariantHash{{sp.first(),sp.last()}});
                continue;
            }

            if(!returnList.contains(v)){
                returnList.append(v);
                continue;
            }
        }
    }
    return returnList.size()==1
               ?returnList.first()
               :returnList;
}

QVariant HttpHeaders::authorization(const QVariant &authorizationScheme)
{
    return this->authorization(__header_authorization, authorizationScheme);
}

HttpHeaders &HttpHeaders::authorizationRemove()
{
    return this->rawHeaderRemove(__header_authorization);
}

QVariant HttpHeaders::proxyAuthorization(const QVariant &authorizationScheme)
{
    return this->authorization(__header_proxy_authorization, authorizationScheme);
}

QVariant HttpHeaders::wwwAuthenticate(const QVariant &authorizationScheme)
{
    return this->authorization(__header_www_authenticate, authorizationScheme);
}

QVariant HttpHeaders::cookies()const
{
    return this->rawHeader(__header_cookie);
}

HttpHeaders &HttpHeaders::cookies(const QVariant &cookie)
{
    return this->setRawHeader(__header_cookie, cookie);
}

}
