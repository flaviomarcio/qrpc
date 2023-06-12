#include "./qrpc_listen_request.h"
#include "./qrpc_request.h"
#include "./qrpc_startup.h"
#include "./qrpc_macro.h"
#include <QVariantHash>
#include "../../qstm/src/qstm_setting_base.h"
//#include <QStm>

//#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
//#define CBOR_INCLUDED
//#endif

#include <QtCborCommon>
#include <QCborStreamReader>
#include <QCborStreamWriter>
#include <QCborArray>
#include <QCborMap>
#include <QCborValue>
#include <QCborParserError>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDateTime>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QMetaProperty>
#include <QTimer>

namespace QRpc {

Q_GLOBAL_STATIC(QByteArray, baseUuid)

static void init()
{
    auto makeBaseUuid=[]()
    {
        QProcess process;
        auto lst = process.environment();
        auto bytes=lst.join(QStringLiteral(",")).toUtf8()+QDateTime::currentDateTime().toString().toUtf8();
        return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
    };

    *baseUuid=makeBaseUuid();
}

Q_RPC_STARTUP_FUNCTION(init);

class ListenRequestPvt:public QObject{
public:
    ListenRequest*parent=nullptr;
    QHash<QString, QFile *> uploadedFiles;
    QEventLoop eventLoop;
    QUuid listenUuid;
    ContentType requestContentType =QRpc::AppNone;
    QUuid requestUuid;
    QByteArray requestProtocol;
    int requestPort=-1;
    QByteArray requestPath;
    QByteArray requestMethod;
    QVariantHash requestHeader;
    QVariantHash requestParameter;
    QVariant requestBody;
    QVariantList requestParserProperty;
    QVariantHash responseHeader;
    QVariantHash responseCallback;
    QVariant responseBody;
    QByteArray responsePhrase;
    QVariantHash requestParamCache;
    ListenRequestCode listenCode;
    int responseCode=0;
    int requestTimeout=0;

    explicit ListenRequestPvt(ListenRequest*parent):QObject{parent}, listenCode(parent)
    {
        this->parent=parent;
        QObject::connect(parent, &ListenRequest::finish, this, &ListenRequestPvt::onRequestFinish);
    }

    virtual ~ListenRequestPvt()
    {
        this->freeFiles();
    }

    void mergeMap(const QVariant &v)
    {
        Q_DECLARE_VU;
        auto &r=*this->parent;
        r.mergeMap(vu.toMap(v));
        if(!r.isMethodGet() && !r.isMethodDelete())
            return;

        auto requestBody=r.requestBody().toByteArray().trimmed();
        if(!r.requestParameter().isEmpty() || requestBody.isEmpty())
            return;

        if(requestBody.isEmpty() || requestBody.at(0)!='{')
            return;

        QVariantHash requestParameter;
        auto body=QJsonDocument::fromJson(requestBody).toVariant().toHash();
        Q_V_HASH_ITERATOR(body){
            i.next();
            requestParameter[i.key()]=i.value();
        }
        r.setRequestParameter(requestParameter);
    }

    QVariantHash &requestParam()
    {
        auto requestBodyHash=this->parent->requestBodyHash();
        if(!requestBodyHash.isEmpty())
            this->requestParamCache=requestBodyHash;
        else
            this->requestParamCache=this->parent->requestParameter();
        return this->requestParamCache;
    }

    template<class T>
    T toVMap() const
    {
        T map;
        const QMetaObject* metaObject = parent->metaObject();
        for(int i = metaObject->propertyOffset() ; i < metaObject->propertyCount() ; i++){
            auto property = metaObject->property(i);
            auto key=property.name();
            auto value=property.read(parent);
            if(value.isValid()){
                if(value.typeId()==QMetaType::QUuid && (!value.toUuid().isNull()))
                    map.insert(key, value.toUuid());
                else
                    map.insert(key, value);
            }
        }
        return map;
    }

    void freeFiles()
    {
        for(auto &v:this->uploadedFiles){
            v->close();
            delete v;
        }
        this->uploadedFiles.clear();
    }

    void makeUuid()
    {
        if(this->requestUuid.isNull()){
            auto uuidNs = QUuid::createUuid();
            this->requestUuid=QUuid::createUuidV5(uuidNs, *baseUuid);
        }
    }

    void clear()
    {
        this->eventLoop.quit();
        this->listenUuid={};
        this->requestParamCache.clear();
        this->clearRequest();
        this->clearResponse();
    }

    void clearRequest()
    {
        this->requestUuid={};
        this->requestHeader={};
        this->requestParameter={};
        this->requestBody={};
    }

    void clearResponse()
    {
        this->responseHeader={};
        this->responseBody={};
        this->responseCode=0;
        this->responsePhrase={};
    }

    void setRequestHeader(const QVariantHash &value)
    {
        this->requestHeader=value;
        this->requestContentType=QRpc::AppXwwwForm;
        static auto headerName=QString(ContentTypeName).toLower();
        QHashIterator<QString, QVariant> i(value);
        while (i.hasNext()) {
            i.next();
            auto key=i.key().toLower();
            auto value=i.value().toString().toLower().trimmed();
            if(key.toLower()!=headerName)
                continue;

            QHashIterator<QString, ContentType> i(ContentTypeHeaderToHeaderType);
            while (i.hasNext()) {
                i.next();
                if(!value.contains(i.key()))
                    continue;
                this->requestContentType=i.value();
                break;
            }
        }
    }

    QVariantHash authorizationHeaders() const
    {
        static QStm::Network n;
        QHashIterator<QString, QVariant> i(this->requestHeader);
        while (i.hasNext()) {
            i.next();
            if(i.key().trimmed().toLower()==n.HEADER_AUTHORIZATION.toLower())
                return {{n.HEADER_AUTHORIZATION, i.value()}};
        }
        return {};
    }

    QByteArray authorizationParser(const QByteArray &type) const
    {
        const auto stype=type.toLower().trimmed();
        QHashIterator<QString, QVariant> i(this->requestHeader);
        while (i.hasNext()) {
            i.next();
            auto k=i.key().trimmed().toLower();
            auto v=i.value().toString()
                         .replace(QStringLiteral("Basic"),QStringLiteral("basic"))
                         .replace(QStringLiteral("Bearer"),QStringLiteral("bearer"))
                         .replace(QStringLiteral("Service"),QStringLiteral("service"));
            if(k!=QStringLiteral("authorization"))
                continue;

            if(v.startsWith(stype)){
                auto authList=v.split(stype);
                auto value=authList.last().trimmed();
                return value.toUtf8();
            }
        }
        return {};
    }

public slots:
    void onRequestFinish()
    {
        this->eventLoop.quit();
    }
};

ListenRequest::ListenRequest(QObject *parent):QObject{parent}
{
    this->p = new ListenRequestPvt{this};
}

ListenRequest::ListenRequest(const QVariant &requestBody, QObject *parent):QObject{parent},p{new ListenRequestPvt{this}}
{
    p->mergeMap(requestBody);
}

ListenRequest::ListenRequest(const QVariant &requestBody, const QVariantHash &setting, QObject *parent):QObject{parent},p{new ListenRequestPvt{this}}
{
    this->p = new ListenRequestPvt{this};
    this->setControllerSetting(setting);
    p->mergeMap(requestBody);
}

QVariantHash ListenRequest::authorizationHeaders() const
{
    return p->authorizationHeaders();
}

QString QRpc::ListenRequest::authorizationBasic() const
{
    return p->authorizationParser(QByteArrayLiteral("Basic"));
}

QString ListenRequest::authorizationBearer() const
{
    return p->authorizationParser(QByteArrayLiteral("Bearer"));
}

QString ListenRequest::authorizationService() const
{
    return p->authorizationParser(QByteArrayLiteral("Service"));
}

bool ListenRequest::isMethodHead() const
{
    return (p->requestMethod==QByteArrayLiteral("head"));
}

bool ListenRequest::isMethodGet()const
{
    return (p->requestMethod==QByteArrayLiteral("get"));
}

bool ListenRequest::isMethodPost()const
{
    return (p->requestMethod==QByteArrayLiteral("post"));
}

bool ListenRequest::isMethodPut() const
{
    return (p->requestMethod==QByteArrayLiteral("put"));
}

bool ListenRequest::isMethodDelete() const
{
    return (p->requestMethod==QByteArrayLiteral("delete"));
}

bool ListenRequest::isMethodUpload() const
{
    return !p->uploadedFiles.isEmpty();
}

bool ListenRequest::isMethodOptions() const
{
    return (p->requestMethod==QByteArrayLiteral("options"));
}

bool ListenRequest::canMethodHead()
{
    if(!this->isMethodHead())
        return this->co().setBadGateway().isOK();
    return true;
}

bool ListenRequest::canMethodGet()
{
    if(!this->isMethodGet())
        return this->co().setBadGateway().isOK();
    return true;
}

bool ListenRequest::canMethodPost()
{
    if(!this->isMethodPost())
        return this->co().setBadGateway().isOK();
    return true;
}

bool ListenRequest::canMethodPut()
{
    if(!this->isMethodPut())
        return this->co().setBadGateway().isOK();
    return true;
}

bool ListenRequest::canMethodDelete()
{
    if(!this->isMethodDelete())
        return this->co().setBadGateway().isOK();
    return true;
}

bool ListenRequest::canMethodUpload()
{
    if(!this->isMethodUpload())
        return this->co().setBadGateway().isOK();
    return true;
}

bool ListenRequest::canMethodOptions()
{
    if(!this->isMethodOptions())
        return this->co().setBadGateway().isOK();
    return true;
}

void ListenRequest::start()
{
    auto onTimeOut=[this]{
        this->co().setRequestTimeout();
        p->eventLoop.quit();
    };
    if(this->requestTimeout()>0){
        QTimer::singleShot(this->requestTimeout(), this, onTimeOut);
    }
    p->eventLoop.exec();
}

ListenRequestCode &ListenRequest::codeOption()
{
    return p->listenCode;
}

ListenRequestCode &ListenRequest::co()
{
    return p->listenCode;
}

ListenRequestCode &ListenRequest::co(const QVariant &v)
{
    p->listenCode=v;
    return p->listenCode;
}

QByteArray ListenRequest::hash() const
{
    return QCryptographicHash::hash(p->requestPath, QCryptographicHash::Md5).toHex();
}

bool ListenRequest::isValid() const
{
    return !p->requestUuid.isNull();
}

bool ListenRequest::isEmpty() const
{
    if(p->requestPath.trimmed().isEmpty())
        return false;

    if(p->requestMethod.trimmed().isEmpty())
        return false;

    return true;
}

void ListenRequest::clear()
{
    p->clear();
}

bool ListenRequest::fromRequest(const ListenRequest &request)
{
    return this->fromHash(request.toHash());
}

bool ListenRequest::fromMap(const QVariantMap &vRequest)
{
    p->clear();
    return this->mergeMap(vRequest);
}

bool ListenRequest::mergeMap(const QVariantMap &vRequest)
{
    auto &metaObject =*this->metaObject();
    for(auto i = metaObject.propertyOffset() ; i < metaObject.propertyCount() ; i++){
        auto property = metaObject.property(i);
        auto key = property.name();
        auto vVal= property.read(this);
        auto vNew=vRequest.value(key);
        if(!vNew.isValid() || vNew.isNull()){
            continue;
        }
        auto type=property.typeId();
        switch (type) {
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        {
            auto vHash=vVal.toHash();
            if(!vHash.isEmpty()){
                auto mNew=vNew.toHash();
                QHashIterator<QString, QVariant> it(mNew);
                while (it.hasNext()) {
                    it.next();
                    vHash.insert(it.key(), it.value());
                }
                vNew=vHash;
            }
            if(!property.write(this, vNew)){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto vLst=vVal.toList();
            if(!vLst.isEmpty()){
                auto mNew=vNew.toList();
                for(auto &v:mNew){
                    if(vLst.contains(v))
                        continue;
                    vLst<<v;
                }
                vNew=vLst;
            }
            if(!property.write(this, vNew)){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::QUuid:
        {
            if(!property.write(this, vNew.toUuid())){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::QUrl:
        {
            if(!property.write(this, vNew.toUrl())){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::UInt:
        case QMetaType::Int:
        {
            if(!property.write(this, vNew.toInt())){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::ULongLong:
        case QMetaType::LongLong:
        {
            if(!property.write(this, vNew.toLongLong())){
                this->clear();
                return this->isValid();
            }
            break;
        }

        case QMetaType::Double:
        {
            if(!property.write(this, vNew.toDouble())){
                this->clear();
                return this->isValid();
            }
            break;
        }
        default:
            if(!property.write(this, vNew)){
                if(type!=QMetaType::User){
                    this->clear();
                    return this->isValid();
                }

                if(!property.write(this, vNew.toInt())){
                    this->clear();
                    return this->isValid();
                }
                break;
            }
        }
    }
    return this->isValid();
}

bool ListenRequest::fromHash(const QVariantHash &vRequest)
{
    p->clear();
    return this->mergeHash(vRequest);
}

bool ListenRequest::mergeHash(const QVariantHash &vRequest)
{
    auto &metaObject =*this->metaObject();
    for(auto i = metaObject.propertyOffset() ; i < metaObject.propertyCount() ; i++){
        auto property = metaObject.property(i);
        auto key = property.name();
        auto vVal= property.read(this);
        auto vNew=vRequest.value(key);
        if(!vNew.isValid() || vNew.isNull()){
            continue;
        }
        auto type=property.typeId();
        switch (type) {
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        {
            auto vHash=vVal.toHash();
            if(!vHash.isEmpty()){
                auto mNew=vNew.toHash();
                QHashIterator<QString, QVariant> it(mNew);
                while (it.hasNext()) {
                    it.next();
                    vHash.insert(it.key(), it.value());
                }
                vNew=vHash;
            }
            if(!property.write(this, vNew)){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto vLst=vVal.toList();
            if(!vLst.isEmpty()){
                auto mNew=vNew.toList();
                for(auto &v:mNew){
                    if(vLst.contains(v))
                        continue;
                    vLst.append(v);
                }
                vNew=vLst;
            }
            if(!property.write(this, vNew)){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::QUuid:
        {
            if(!property.write(this, vNew.toUuid())){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::QUrl:
        {
            if(!property.write(this, vNew.toUrl())){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::UInt:
        case QMetaType::Int:
        {
            if(!property.write(this, vNew.toInt())){
                this->clear();
                return this->isValid();
            }
            break;
        }
        case QMetaType::ULongLong:
        case QMetaType::LongLong:
        {
            if(!property.write(this, vNew.toLongLong())){
                this->clear();
                return this->isValid();
            }
            break;
        }

        case QMetaType::Double:
        {
            if(!property.write(this, vNew.toDouble())){
                this->clear();
                return this->isValid();
            }
            break;
        }
        default:
            if(!property.write(this, vNew)){
                if(type!=QMetaType::User){
                    this->clear();
                    return this->isValid();
                }

                if(!property.write(this, vNew.toInt())){
                    this->clear();
                    return this->isValid();
                }
                break;
            }
        }
    }
    return this->isValid();
}

bool ListenRequest::fromResponseMap(const QVariantHash &vRequest)
{
    p->clearResponse();
    const QMetaObject* metaObject = this->metaObject();
    for(int i = metaObject->propertyOffset() ; i < metaObject->propertyCount() ; i++){
        auto property = metaObject->property(i);
        auto key = QByteArray{property.name()};
        if(!key.startsWith(QByteArrayLiteral("requestUuid")) && !key.startsWith(QByteArrayLiteral("response")))
            continue;

        auto value=vRequest.value(key);
        if(property.write(this, value))
            continue;

        auto typeId=property.typeId();
        switch (typeId) {
        case QMetaType::QByteArray:
            value=value.toByteArray();
            break;
        case QMetaType::QString:
            value=value.toString();
            break;
        case QMetaType::QChar:
            value=value.toChar();
            break;
        case QMetaType::QBitArray:
            value=value.toByteArray();
            break;
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            value=value.toLongLong();
            break;
        case QMetaType::Int:
        case QMetaType::UInt:
            value=value.toInt();
            break;
        case QMetaType::Double:
            value=value.toDouble();
            break;
        case QMetaType::Bool:
            value=value.toBool();
            break;
        case QMetaType::QDateTime:
            value=value.toDateTime();
            break;
        case QMetaType::QDate:
            value=value.toDate();
            break;
        case QMetaType::QTime:
            value=value.toTime();
            break;
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
            value=value.toHash();
            break;
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            value=value.toList();
            break;
        case QMetaType::QUuid:
            value=value.toUuid();
            break;
        case QMetaType::QUrl:
            value=value.toUrl();
            break;
        default:
            break;
        }
    }
    return this->isValid();
}

bool ListenRequest::fromResponseHash(const QVariantHash &vRequest)
{
    p->clearResponse();
    const QMetaObject* metaObject = this->metaObject();
    for(int i = metaObject->propertyOffset() ; i < metaObject->propertyCount() ; i++){
        auto property = metaObject->property(i);
        auto key = QByteArray{property.name()};
        if(!key.startsWith(QByteArrayLiteral("requestUuid")) && !key.startsWith(QByteArrayLiteral("response")))
            continue;

        auto value=vRequest.value(key);
        if(property.write(this, value))
            continue;

        auto typeId=property.typeId();
        switch (typeId) {
        case QMetaType::QByteArray:
            value=value.toByteArray();
            break;
        case QMetaType::QString:
            value=value.toString();
            break;
        case QMetaType::QChar:
            value=value.toChar();
            break;
        case QMetaType::QBitArray:
            value=value.toByteArray();
            break;
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            value=value.toLongLong();
            break;
        case QMetaType::Int:
        case QMetaType::UInt:
            value=value.toInt();
            break;
        case QMetaType::Double:
            value=value.toDouble();
            break;
        case QMetaType::Bool:
            value=value.toBool();
            break;
        case QMetaType::QDateTime:
            value=value.toDateTime();
            break;
        case QMetaType::QDate:
            value=value.toDate();
            break;
        case QMetaType::QTime:
            value=value.toTime();
            break;
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
            value=value.toHash();
            break;
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            value=value.toList();
            break;
        case QMetaType::QUuid:
            value=value.toUuid();
            break;
        case QMetaType::QUrl:
            value=value.toUrl();
            break;
        default:
            break;
        }
    }
    return this->isValid();
}

bool ListenRequest::fromJson(const QByteArray &bytes)
{
    QJsonParseError*parser=nullptr;
    auto v=QJsonDocument::fromJson(bytes, parser).object().toVariantHash();
    return this->fromHash(v);
}

QByteArray ListenRequest::toJson() const
{
    auto vMap=this->toHash();
    auto bytes=QJsonDocument::fromVariant(vMap).toJson(QJsonDocument::Compact);
    return bytes;
}

QVariantMap ListenRequest::toMap() const
{
    return p->toVMap<QVariantMap>();
}

QVariantHash ListenRequest::toHash() const
{
    return p->toVMap<QVariantHash>();
}

void ListenRequest::requestStart()
{
    QTimer::singleShot(p->requestTimeout, &p->eventLoop, &QEventLoop::quit);
    p->eventLoop.exec();
}

QUuid &ListenRequest::listenUuid() const
{
    return p->listenUuid;
}

void ListenRequest::setListenUuid(const QUuid &value)
{
    p->listenUuid=value;
}

QUuid &ListenRequest::requestUuid() const
{
    return p->requestUuid;
}

void ListenRequest::setRequestUuid(const QUuid &value)
{
    p->requestUuid = value;
}

QByteArray &ListenRequest::requestProtocol() const
{
    return p->requestProtocol;
}

void ListenRequest::setRequestProtocol(const QVariant &value)
{
    auto ivalue=value.toLongLong();
    if(ivalue>0){
        auto name=ProtocolUrlName.value(Protocol(ivalue)).toUtf8();
        if(name.isEmpty())
            p->requestProtocol = value.toString().toUtf8();
        else
            p->requestProtocol = name;
    }
}

int ListenRequest::requestPort() const
{
    return p->requestPort;
}

void ListenRequest::setRequestPort(int value)
{
    p->requestPort=value;
    emit requestPortChanged();
}

QByteArray &ListenRequest::requestPath() const
{
    return p->requestPath;
}

void QRpc::ListenRequest::setRequestPath(const QVariant &value)
{
    p->requestPath=value.toByteArray();
    static const auto __path_separator="/";
    if(!p->requestPath.startsWith(__path_separator))
        p->requestPath=__path_separator+p->requestPath;
    if(p->requestPath.endsWith(__path_separator))
        p->requestPath=p->requestPath.mid(0,p->requestPath.length()-1);
}

QByteArray &ListenRequest::requestMethod() const
{
    return p->requestMethod;
}

void ListenRequest::setRequestMethod(const QVariant &value)
{
    auto ivalue=value.toLongLong();
    if(ivalue>0)
        p->requestMethod=RequestMethodName.value(ivalue).toUtf8();
    else
        p->requestMethod=value.toByteArray().toLower();
}

QVariantHash &ListenRequest::requestHeader() const
{
    return p->requestHeader;
}

void ListenRequest::setRequestHeader(const QVariantHash &value)
{
    p->setRequestHeader(value);
}

QVariantHash &ListenRequest::requestParameter() const
{
    return p->requestParameter;
}

void ListenRequest::setRequestParameter(const QVariantHash &value)
{
    p->requestParamCache.clear();
    p->requestParameter=value;
}

QVariant &ListenRequest::requestBody() const
{
    return p->requestBody;
}

QVariantMap ListenRequest::requestBodyMap() const
{
    switch (p->requestBody.typeId())
    {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
        return p->requestBody.toMap();
        break;
    default:
        if(this->isMethodGet() || this->isMethodDelete())
            return QVariant{p->requestParameter}.toMap();
        return {};
    }
}

QVariantHash ListenRequest::requestBodyHash() const
{
    switch (p->requestBody.typeId())
    {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
        return p->requestBody.toHash();
        break;
    default:
        if(this->isMethodGet() || this->isMethodDelete())
            return QVariant{p->requestParameter}.toHash();
    }
    return {};
}

bool ListenRequest::requestParserBodyMap()
{
    QVariantMap body;
    if(!this->requestParserBodyMap(p->requestParserProperty, body))
        return this->co().setBadRequest().isOK();

    return true;
}

bool ListenRequest::requestParserBodyMap(const QVariant &property)
{
    QVariantMap body;
    return this->requestParserBodyMap(property, body);
}

bool ListenRequest::requestParserBodyMap(const QVariant &property, QVariantMap &body) const
{

    bool __return=true;
    if(property.isValid()){
        p->requestParserProperty.clear();
        switch (property.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            for(auto &v:property.toList()){
                auto s=v.toString().toLower().trimmed();
                if(!p->requestParserProperty.contains(s))
                    p->requestParserProperty<<s;
            }
            break;
        }
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        {
            auto vKeys=property.toHash().keys();
            for(auto &v:vKeys){
                auto s=v.toLower().trimmed();
                if(!p->requestParserProperty.contains(s))
                    p->requestParserProperty<<s;
            }
            break;
        }
        default:
            break;
        }
        if(!p->requestParserProperty.isEmpty()){
            auto vBody=this->requestParamMap();
            QVariantList vPropBody;
            auto vKeys=vBody.keys();
            for(auto &v:vKeys){
                vPropBody<<v.toLower().trimmed();
            }
            for(auto &prop:p->requestParserProperty){
                if(!vPropBody.contains(prop)){
                    rInfo()<<QStringLiteral("parameter not found: ")+prop.toString();
                    __return=false;
                }
            }
            body=this->requestBodyMap();
        }
    }

    return __return;
}

bool ListenRequest::requestParserBodyHash()
{
    QVariantHash body;
    if(!this->requestParserBodyHash(p->requestParserProperty, body))
        return this->co().setBadRequest().isOK();
    return true;
}

bool ListenRequest::requestParserBodyHash(const QVariant &property)
{
    QVariantHash body;
    return this->requestParserBodyHash(property, body);
}

bool ListenRequest::requestParserBodyHash(const QVariant &property, QVariantHash &body) const
{
    bool __return=true;
    if(property.isValid()){
        p->requestParserProperty.clear();
        switch (property.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            for(auto &v:property.toList()){
                auto s=v.toString().toLower().trimmed();
                if(!p->requestParserProperty.contains(s))
                    p->requestParserProperty<<s;
            }
            break;
        }
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        {
            auto vKeys=property.toHash().keys();
            for(auto &v:vKeys){
                auto s=v.toLower().trimmed();
                if(!p->requestParserProperty.contains(s))
                    p->requestParserProperty<<s;
            }
            break;
        }
        default:
            break;
        }
        if(!p->requestParserProperty.isEmpty()){
            auto vBody=this->requestParamMap();
            QVariantList vPropBody;
            auto vKeys=vBody.keys();
            for(auto &v:vKeys){
                vPropBody<<v.toLower().trimmed();
            }
            for(auto &prop:p->requestParserProperty){
                if(!vPropBody.contains(prop)){
                    rInfo()<<QStringLiteral("parameter not found: ")+prop.toString();
                    __return=false;
                }
            }
            body=this->requestBodyHash();
        }
    }

    return __return;
}

QVariantList &ListenRequest::requestParserProperty()
{
    return p->requestParserProperty;
}

QVariant ListenRequest::requestParamMap(const QByteArray &key) const
{
    return this->requestParamHash(key);
}

QVariantMap ListenRequest::requestParamMap()const
{
    if(this->isMethodPost() || this->isMethodPut())
        return this->requestBodyMap();
    return QVariant{this->requestParameter()}.toMap();
}

QVariant ListenRequest::requestParamHash(const QByteArray &key) const
{
    const auto &map=p->requestParam();
    auto akey=key.trimmed().toLower();
    QHashIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();

        auto bKey=i.key().trimmed().toLower().toUtf8();
        if(akey!=bKey)
            continue;

        auto &v=i.value();
        switch (p->requestBody.typeId()) {
        case QMetaType::QString:
        case QMetaType::QByteArray:
        {
            auto s=v.toByteArray();
            return s.isEmpty()?QVariant{}:s;
        }
        default:
            return v;
        }
    }

    return {};
}

QVariantHash ListenRequest::requestParamHash() const
{
    return p->requestParam();
}

QVariantList ListenRequest::requestBodyList() const
{
    switch (p->requestBody.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return p->requestBody.toList();
    default:
        return QVariantList{p->requestParameter};
    }
}

void ListenRequest::setRequestBody(const QVariant &value)
{
    QVariant _body;
    switch (value.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
        _body=value;
        break;
    default:
        switch (p->requestContentType) {
        case AppJson:
            _body=QJsonDocument::fromJson(value.toByteArray()).toVariant();
            break;
        case AppCBOR:
        case AppOctetStream:
            _body=QCborValue::fromVariant(value).toVariant();
            break;
        default:
            _body={};
            if(!_body.isValid())
                _body=QJsonDocument::fromJson(value.toByteArray()).toVariant();
            if(!_body.isValid())
                _body=QCborValue::fromVariant(value).toVariant();
            if(!_body.isValid())
                _body=value;
        }
    }
    if(this->isMethodGet() || this->isMethodDelete()){
        switch (_body.typeId()) {
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
            p->requestParameter=_body.toHash();
            break;
        default:
            break;
        }
    }
    p->requestParamCache.clear();
    p->requestBody=_body;
}

int ListenRequest::requestTimeout() const
{
    return p->requestTimeout;
}

void ListenRequest::setRequestTimeout(int value)
{
    p->requestTimeout = value;
}

QString ListenRequest::uploadedFileName() const
{
    if(p->uploadedFiles.isEmpty())
        return {};
    return p->uploadedFiles.begin().key();
}

QFile *ListenRequest::uploadedFile() const
{
    if(p->uploadedFiles.isEmpty())
        return nullptr;

    return p->uploadedFiles.begin().value();
}

QHash<QString, QFile *> &ListenRequest::uploadedFiles() const
{
    return p->uploadedFiles;
}

void ListenRequest::setUploadedFiles(const QVariant &vFiles)
{
    auto files=vFiles.toStringList();
    p->freeFiles();
    for(const auto &v:files){
        auto file=new QFile(v);
        if(!file->open(QFile::ReadOnly)){
            delete file;
            continue;
        }

        auto filename=file->fileName().split(QByteArrayLiteral("/")).last();
        p->uploadedFiles.insert(filename, file);
    }
}

QFile *ListenRequest::temporaryFile(const QString &fileName)
{
    static QTemporaryFile f;

    return p->uploadedFiles.value(fileName);
}

QVariantHash &ListenRequest::responseHeader() const
{
    return p->responseHeader;
}

void ListenRequest::setResponseHeader(const QVariantHash &value)
{
    p->responseHeader.clear();
    Q_V_HASH_ITERATOR(value){
        i.next();
        auto &v=i.value();
        auto s=(v.typeId()==QMetaType::QStringList || v.typeId()==QMetaType::QVariantList)
                     ?v.toStringList().join(' ')
                     :v.toString();
        p->responseHeader.insert(i.key(), s);
    }
}

void ListenRequest::setResponseHeader(const HttpHeaders &value)
{
    this->setResponseHeader(value.rawHeader());
}

QVariantHash &ListenRequest::responseCallback() const
{
    return p->responseCallback;
}

void ListenRequest::setResponseCallback(const QVariantHash &value)
{
    p->responseCallback=value;
}

QVariant &ListenRequest::responseBody() const
{
    return p->responseBody;
}

QByteArray ListenRequest::responseBodyBytes() const
{
    QVariant v;
    const auto &response = p->responseBody;
    auto typeId=response.typeId();
    switch (typeId) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    {
        //TODO UTILIZAR switch () {}
        if(p->requestContentType==AppJson)
            v=QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        else if(p->requestContentType==AppCBOR)
            v=QCborValue::fromVariant(response).toCbor();
        else if(p->requestContentType==AppXML)
            v=QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        else if(p->requestContentType==AppOctetStream)
            v=QCborValue::fromVariant(response).toCbor();
        else
            v=QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        break;
    }
    default:
        v=QVariant::fromValue(response);
    }

    if(v.isValid()){
        QString body;
        switch (v.typeId()) {
        case QMetaType::QDate:
            body=v.toDate().toString(Qt::ISODate);
            break;
        case QMetaType::QTime:
            body=v.toTime().toString(Qt::ISODateWithMs);
            break;
        case QMetaType::QDateTime:
            body=v.toDateTime().toString(Qt::ISODateWithMs);
            break;
        case QMetaType::QUuid:
            body=v.toUuid().toString();
            break;
        case QMetaType::QUrl:
            body=v.toUrl().toString();
            break;
        default:
            body=v.toString();
        }
        return body.toUtf8();
    }

    return {};
}

void ListenRequest::setResponseBody(const QVariant &value)
{
    switch (value.typeId()) {
    case QMetaType::QByteArray:
    case QMetaType::QString:
    case QMetaType::QChar:
    {
        if(value.toString().trimmed().isEmpty()){
            p->responseBody={};
            return;
        }
        p->responseBody=value;
        break;
    }
    default:
        p->responseBody=value;
    }
}

int ListenRequest::responseCode(int code) const
{
    return (p->responseCode>0)?p->responseCode:code;
}

void ListenRequest::setResponseCode(const int &value)
{
    p->responseCode = value;
}

QByteArray ListenRequest::responsePhrase() const
{
    if(p->responsePhrase.isEmpty())
        return ListenRequestCode::reasonPhrase(this->responseCode()).toUtf8();
    return p->responsePhrase;
}

QByteArray ListenRequest::responsePhrase(const int code)const
{
    auto c=(code>0)?code:this->responseCode();
    return ListenRequestCode::reasonPhrase(c).toUtf8();
}

void ListenRequest::setResponsePhrase(const QByteArray &value)
{
    p->responsePhrase = value;
}

const QUuid ListenRequest::makeUuid()
{
    p->makeUuid();
    return p->requestUuid;
}

int ListenRequest::requestContentType() const
{
    return p->requestContentType;
}

void ListenRequest::setRequestContentType(const QVariant &value)
{
    auto content_type=ContentType(value.toInt());
    if(!ContentTypeHeaderTypeToHeader.contains(content_type))
        p->requestContentType = QRpc::AppNone;
    else
        p->requestContentType = content_type;
}

void ListenRequest::setRequestResponse(QObject *request)
{
    auto writeResponse=[this](HttpResponse*response){
        if(response!=nullptr){
            this->setResponseHeader(response->header());
            this->setResponseBody(response->body());
            this->co().setCode(response->statusCode());
        }
    };

    HttpResponse*response=nullptr;
    if(request->metaObject()->className()==Request::staticMetaObject.className())
        response=&dynamic_cast<Request*>(request)->response();
    else
        response=dynamic_cast<HttpResponse*>(request);
    writeResponse(response);
}

void ListenRequest::setControllerSetting(const QVariantHash &vSetting)
{
    QStm::SettingBase setting;
    setting.fromHash(vSetting);

    if(setting.isValid() && !setting.enabled())
        return;

    auto &set=setting;
    auto &rq=*this;
    Q_DECLARE_VU;
    {
        auto vDes=rq.requestHeader();
        auto vSrc=set.headers();
        auto vHash=vu.vUnion(vSrc, vDes).toHash();
        rq.setRequestHeader(vHash);
    }
    {
        auto vDes=rq.requestParameter();
        auto vSrc=set.parameters();
        auto vHash=vu.vUnion(vSrc, vDes).toHash();
        rq.setRequestParameter(vHash);
    }
}

}

