#include "./p_qrpc_util.h"
#include "../qrpc_types.h"
#include "../../qstm/src/qstm_macro.h"
#include "../../qstm/src/qstm_meta_enum.h"

namespace QRpc {

namespace Util {

static const auto __pathDelimer="/";
static const auto __pathDelimer2="//";
static const auto __space=" ";

static const auto __url="url";
static const auto __method="method";
static const auto __header="header";
static const auto __start="start";
static const auto __finish="finish";
static const auto __body="body";

const QString routeParser(const QVariant &vRouteBase)
{
    auto routeBase=vRouteBase.toString();
    if(routeBase.contains(__pathDelimer)){
        auto route=__pathDelimer+routeBase.trimmed().toUtf8();
        while(route.contains(__pathDelimer2))
            route=route.replace(__pathDelimer2, __pathDelimer);
        while(route.contains(__space))
            route=route.replace(__space, "");
        return route.toLower();
    }
    return routeBase.toUtf8();
}

const QByteArray routeExtractMethod(const QString &routeBase)
{
    if(routeBase.contains(__pathDelimer))
        return routeBase.split(__pathDelimer).last().toUtf8().toLower();
    return {};
}

const QByteArray routeExtract(const QString &routeBase)
{
    if(routeBase.contains(__pathDelimer)){
        auto lst=routeBase.split(__pathDelimer);
        lst.takeLast();
        return lst.join(__pathDelimer).toUtf8().toLower();
    }
    return {};
}

const QString headerFormatName(const QString &name)
{
    auto sname=name.trimmed();
    QStringList newHeaderName;
    static const auto __separator=QStringList{QStringLiteral("-")};
    for(auto &v:__separator){
        auto nameList=sname.split(v);
        for(auto &name:nameList){
            if(name.trimmed().isEmpty())
                continue;
            name=name.toLower();
            name[0]=name[0].toUpper();
            newHeaderName<<name;
        }
        sname=newHeaderName.join(v);
    }
    return sname;
}

const QString parseQueryItem(const QVariant &v)
{
    if(v.typeId()==QMetaType::QUuid)
        return v.toUuid().toString();
    return v.toString();
}

const QVariantHash toHashResquest(int method, const QVariant &request_url, const QString &request_body, const QVariantHash &request_parameters, const QString &response_body, const QVariantHash &request_header, const QDateTime &request_start, const QDateTime &request_finish)
{
    QStm::MetaEnum<QRpc::Types::Method> eMethod(method);

    auto request_method=eMethod.name();
    QVariantHash vHash{
        {__url, request_url},
        {__method, request_method},
        {__header, request_header},
        {__start, request_start},
        {__finish, request_finish}
    };

    QStringList headers;
    QHashIterator<QString, QVariant> i(request_header);
    while (i.hasNext()) {
        i.next();
        const auto &k=i.key();
        auto v=i.value();
        switch (v.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            v=v.toStringList().join(QStringLiteral(";"));
            break;
        default:
            break;
        }
        headers.append(QStringLiteral("-H '%1: %2'").arg(k, v.toString()));
    }

    QString scUrl, cUrl;
    switch (request_url.typeId()) {
    case QMetaType::QUrl:
    {
        auto url=request_url.toUrl();
        cUrl=url.toString().split(__pathDelimer).join(__pathDelimer);
        break;
    }
    default:
        cUrl=request_url.toString();
        break;
    }

    switch (eMethod.type()) {
    case QRpc::Types::Post:
    case QRpc::Types::Put:
    {
        vHash.insert(__body, request_parameters);
        auto body=request_body;
        body=response_body.trimmed().isEmpty()?"":QStringLiteral("-d '%1'").arg(body.replace('\n',' '));
        scUrl=QStringLiteral("curl --insecure -i -X %1 %2 %3 -G '%4'").arg(request_method, headers.join(' '), body, cUrl).trimmed();
        break;
    }
    default:
        QStringList params;
        vHash[QStringLiteral("parameter")]= request_parameters;
        Q_V_HASH_ITERATOR(request_parameters){
            i.next();
            auto k=i.key();
            params.append(QStringLiteral("-d %1=%2").arg(k, Util::parseQueryItem(i.value())));
        }
        scUrl=QStringLiteral("curl --insecure -i -X %1 %2 -G '%3' %4").arg(request_method, headers.join(' '), cUrl, params.join(' ')).trimmed();
    }

    vHash.insert(QStringLiteral("curl"), scUrl);
    return vHash;
}
}
}
