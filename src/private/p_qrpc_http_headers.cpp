#include "./p_qrpc_http_headers.h"
#include "./p_qrpc_util.h"
#include "../qrpc_macro.h"
#include <QJsonDocument>
#include <QStm>

namespace QRpc {

static const auto staticDefaultContentType=QRpc::AppJson;

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCHttpHeadersPvt*>(this->p)

class QRPCHttpHeadersPvt{
public:

    QObject*parent=nullptr;
    QVariantHash header;

    explicit QRPCHttpHeadersPvt(QRPCHttpHeaders*parent)
    {
        this->parent=parent;
    }

    virtual ~QRPCHttpHeadersPvt()
    {
    }

    QVariant header_v(const QString&key)const
    {
        auto vkey=key.trimmed().toLower();
        QHashIterator<QString, QVariant> i(this->header);
        QStringList vList;
        while (i.hasNext()) {
            i.next();

            if(i.key().trimmed().toLower()!=vkey)
                continue;

            auto&v=i.value();
            switch (qTypeId(v)) {
            case QMetaType_QStringList:
                vList=v.toStringList();
                break;
            case QMetaType_QVariantList:
            {
                for(auto&v:v.toList())
                    vList<<v.toString();
                break;
            }
            default:
                vList<<v.toString();
            }
        }

        if(vList.isEmpty())
            return {};

        return vList;
    }
};

QRPCHttpHeaders::QRPCHttpHeaders(QObject *parent):QObject(parent)
{
    this->p = new QRPCHttpHeadersPvt(this);
    dPvt();
    p.parent=parent;
}

QRPCHttpHeaders::QRPCHttpHeaders(const QVariant &v, QObject *parent):QObject(parent)
{
    this->p = new QRPCHttpHeadersPvt(this);
    dPvt();
    p.parent=parent;
    Q_DECLARE_VU;
    p.header=vu.toHash(v);
}

QRPCHttpHeaders::~QRPCHttpHeaders()
{
    dPvt();
    delete&p;
}

QRPCHttpHeaders &QRPCHttpHeaders::clear()
{
    dPvt();
    p.header.clear();
    return*this;
}

QVariantHash&QRPCHttpHeaders::rawHeader()const
{
    dPvt();
    return p.header;
}

QVariant QRPCHttpHeaders::rawHeader(const QString &headername)const
{
    dPvt();
    QStringList returnList;
    QHashIterator<QString, QVariant> i(p.header);
    while (i.hasNext()) {
        i.next();

        if(i.key().toLower()!=headername.toLower())
            continue;

        auto&v=i.value();
        QStringList vList;
        switch (qTypeId(v)) {
        case QMetaType_QStringList:
        case QMetaType_QVariantList:
            vList=v.toStringList();
            break;
        default:
            vList=v.toString().split(qsl(";"));
        }

        for(auto&header:vList)
            returnList<<header;
    }
    return QVariant(returnList);
}

QRPCHttpHeaders &QRPCHttpHeaders::setRawHeader(const QVariantHash &rawHeader)
{
    dPvt();
    auto&header=p.header;
    header.clear();
    QHashIterator<QString, QVariant> i(rawHeader);
    while (i.hasNext()) {
        i.next();
        const auto key=QRpc::Util::headerFormatName(i.key());
        const auto value=i.value();
        this->setRawHeader(key,value);
    }
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::setRawHeader(const QString &header, const QVariant &value)
{
    QVariantList list;
    switch (qTypeId(value)) {
    case QMetaType_QStringList:
    case QMetaType_QVariantList:
    {
        for(auto&v:value.toList())
            list<<v;
        break;
    }
    default:
        list<<value;
    }

    dPvt();
    const auto headerName=QRpc::Util::headerFormatName(header);
    auto vList=p.header[headerName].toStringList();
    vList.clear();
    for(auto&v:list){
        auto vv=v.toByteArray().trimmed();
        if(vv.isEmpty())
            continue;

        if(!vList.contains(vv))
            vList<<vv.trimmed();
    }

    for(auto&v:vList){
        auto x=v;
        if(x.startsWith(qbl_fy(Basic)) || x.startsWith(qbl_fy(basic))){
            auto x_toUtf8 = x.replace(qbl_fy(Basic), qsl_null).replace(qbl_fy(basic), qsl_null).trimmed().toUtf8();
            auto x_from=QByteArray::fromBase64(x_toUtf8);
            if(x_from.toBase64()!=x_toUtf8){
                v=qbl_fy(Basic)+qsl_space+x_toUtf8.toBase64();
            }
        }
    }

    p.header[headerName]=vList;
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::addRawHeader(const QVariantHash &rawHeader)
{
    Q_DECLARE_VU;
    QHashIterator<QString, QVariant> i(rawHeader);
    while (i.hasNext()) {
        i.next();
        this->addRawHeader(i.key(), i.value());
    }
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::addRawHeader(const QString &header, const QVariant &value)
{
    QVariantList list;
    switch (qTypeId(value)) {
    case QMetaType_QStringList:
    case QMetaType_QVariantList:
    {
        for(auto&v:value.toList())
            list<<v;
        break;
    }
    default:
        list<<value;
    }

    dPvt();
    const auto headerName=QRpc::Util::headerFormatName(header);
    auto vList=p.header.value(headerName).toStringList();
    vList.clear();
    for(auto&v:list){
        auto vv=v.toByteArray().trimmed();

        if(vv.isEmpty())
            continue;

        if(vList.contains(vv))
            continue;

        vList<<vv.trimmed();
    }
    this->setRawHeader(headerName, vList);
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::setContentType(const int contentType)
{
    QString header;
    auto content_type=ContentType(contentType);

    if(QRPCContentTypeHeaderTypeToHeader.contains(content_type))
        return this->setContentType(QRPCContentTypeHeaderTypeToHeader.value(content_type));

    return this->setContentType(QRPCContentTypeHeaderTypeToHeader.value(QRpc::AppOctetStream));
}

QRPCHttpHeaders &QRPCHttpHeaders::setContentType(const QVariant &v)
{
    dPvt();
    p.header.remove(ContentTypeName);
    p.header.remove(QString(ContentTypeName).toLower());
    QVariant value=v;
    if(qTypeId(v)==QMetaType_QUrl){
        value=QVariant();
        auto url=v.toUrl();
        if(url.isLocalFile()){
            auto ext=url.toLocalFile().split(qsl(".")).last().trimmed();
            if(!QRPCContentTypeExtensionToHeader.contains(ext.toLower()))
                ext.clear();
            auto type=QRPCContentTypeExtensionToHeader.value(ext.toLower());
            value=QRPCContentTypeHeaderTypeToHeader.value(type);
        }
    }
    if(value.isValid())
        p.header.insert(ContentTypeName,value);
    return*this;
}

bool QRPCHttpHeaders::isContentType(int contentType) const
{
    dPvt();
    const auto contenttype=p.header_v(ContentTypeName).toStringList();
    for(auto&ct:contenttype){
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

QVariant QRPCHttpHeaders::contentType() const
{
    dPvt();
    auto v=p.header_v(ContentTypeName);
    return v;
}

ContentType QRPCHttpHeaders::defaultContentType()
{
    return staticDefaultContentType;
}

QVariant QRPCHttpHeaders::contentDisposition() const
{
    dPvt();
    auto v=p.header_v(ContentDispositionName);
    return v;
}

QRPCHttpHeaders &QRPCHttpHeaders::setAuthorization(const QString &authorization, const QString &type, const QVariant &credentials)
{
    QString scredentials;

    switch (qTypeId(scredentials)) {
    case QMetaType_QVariantMap:
    case QMetaType_QVariantList:
    {
        QStringList params;
        QHashIterator<QString, QVariant> i(credentials.toHash());
        while (i.hasNext()) {
            i.next();
            params<<qsl("%1=%2").arg(i.key(),i.value().toString());
        }
        scredentials=params.join(qsl_space);
        break;
    }
    default:
        scredentials=credentials.toString().trimmed();
    }

    if(!type.isEmpty() && !scredentials.isEmpty())
        this->setRawHeader(authorization.toUtf8(), qsl("%1 %2").arg(type,scredentials).toUtf8());
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::setAuthorization(const QString &authorization, const AuthorizationType &type, const QVariant &credentials)
{
    QString stype;
    switch (type) {
    case Basic:
        stype=qsl("Basic");
        break;
    case Bearer:
        stype=qsl("Bearer");
        break;
    case Digest:
        stype=qsl("Digest");
        break;
    case HOBA:
        stype=qsl("HOBA");
        break;
    case Mutual:
        stype=qsl("Mutual");
        break;
    case AWS4_HMAC_SHA256:
        stype=qsl("AWS4_HMAC_SHA256");
        break;
    case Service:
        stype=qsl("Service");
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


QRPCHttpHeaders &QRPCHttpHeaders::setAuthorization(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("Authorization"), type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setAuthorization(const AuthorizationType &type, const QVariant &userName, const QVariant &passWord)
{
    auto credentials=qsl("%1:%2").arg(userName.toString(), passWord.toString()).toUtf8().toBase64();
    return this->setAuthorization(type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setProxyAuthorization(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("Proxy-Authorization"), type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setProxyAuthorization(const QString &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("Proxy-Authorization"), type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setWWWAuthenticate(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("WWW-Authenticate"), type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setWWWAuthenticate(const QString &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("WWW-Authenticate"), type, credentials);
}

QVariant QRPCHttpHeaders::cookies()const
{
    return this->rawHeader(qsl("Cookie"));
}

QRPCHttpHeaders &QRPCHttpHeaders::setCookies(const QVariant &cookie)
{
    return this->setRawHeader(qsl("Cookie"), cookie);
}

QVariant QRPCHttpHeaders::authorization(const QString &authorization, const QString &type)
{
    dPvt();
    QVariantList returnList;
    QHashIterator<QString, QVariant> i(p.header);
    while (i.hasNext()) {
        i.next();
        if(i.key().toLower()!=authorization.toLower())
            continue;
        auto list=i.value().toString().split(qsl_space);
        if(list.size()<=1)
            continue;


        if(type.toLower()!=list.first())
            continue;

        list.takeFirst();
        for(auto&v:list){
            if(v.contains(qsl("="))){
                auto sp=v.split(qsl("="));
                QVariantHash map;
                map.insert(sp.at(0),sp.at(1));
                returnList<<map;
                continue;
            }

            if(!returnList.contains(v)){
                returnList<<v;
                continue;
            }
        }
    }
    return returnList.size()==1?returnList.first():returnList;
}

QVariant QRPCHttpHeaders::authorization(const QString &authorization, const AuthorizationType &type)
{
    QString stype;
    switch (type) {
    case Basic:
        stype=qsl("Basic");
        break;
    case Bearer:
        stype=qsl("Bearer");
        break;
    case Digest:
        stype=qsl("Digest");
        break;
    case HOBA:
        stype=qsl("HOBA");
        break;
    case Mutual:
        stype=qsl("Mutual");
        break;
    case AWS4_HMAC_SHA256:
        stype=qsl("AWS4_HMAC_SHA256");
        break;
    default:
        break;
    }
    return this->authorization(authorization, stype);
}

QVariant QRPCHttpHeaders::authorization(const AuthorizationType &type)
{
    return this->authorization(qsl("Authorization"), type);
}

QVariant QRPCHttpHeaders::authorization(const QString &type)
{
    return this->authorization(qsl("Authorization"), type);
}

QVariant QRPCHttpHeaders::proxyAuthorization(const AuthorizationType &type)
{
    return this->authorization(qsl("Proxy-Authorization"), type);
}

QVariant QRPCHttpHeaders::proxyAuthorization(const QString &type)
{
    return this->authorization(qsl("Proxy-Authorization"), type);
}

QVariant QRPCHttpHeaders::wwwAuthenticate(const AuthorizationType &type)
{
    return this->authorization(qsl("WWW-Authorization"), type);
}

QVariant QRPCHttpHeaders::wwwAuthenticate(const QString &type)
{
    return this->authorization(qsl("WWW-Authorization"), type);
}

QRPCHttpHeaders &QRPCHttpHeaders::print(const QString &output)
{
    for(auto&v:this->printOut(output))
        sInfo()<<v;
    return*this;
}

QStringList QRPCHttpHeaders::printOut(const QString &output)
{
    auto space=output.trimmed().isEmpty()?qsl_null:qsl("    ");
    QStringList __return;
    Q_DECLARE_VU;
    auto vHash=this->rawHeader();
    if(!vHash.isEmpty()){
        __return<<qsl("%1%2 headers").arg(space, output).trimmed();
        QHashIterator<QString, QVariant> i(vHash);
        while (i.hasNext()){
            i.next();
            __return<<qsl("%1     %2:%3").arg(space, i.key(), vu.toStr(i.value()));
        }
    }
    return __return;
}

QRPCHttpHeaders &QRPCHttpHeaders::operator=(const QVariant &v)
{
    dPvt();
    p.header.clear();
    QVariantHash vHash;
    switch (qTypeId(v)) {
    case QMetaType_QString:
    case QMetaType_QByteArray:
        vHash=QJsonDocument::fromJson(v.toByteArray()).toVariant().toHash();
        break;
    default:
        vHash=v.toHash();
    }
    return this->setRawHeader(vHash);
}

QRPCHttpHeaders &QRPCHttpHeaders::operator<<(const QVariant &v)
{
    QVariantHash vHash;
    switch (qTypeId(v)) {
    case QMetaType_QString:
    case QMetaType_QByteArray:
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
    return*this;
}

}
