#include "./p_qrpc_util.h"
#include "../qrpc_types.h"
#include "../../qstm/src/qstm_macro.h"
#include "../../qstm/src/qstm_util_variant.h"

namespace QRpc {

namespace Util {

const QString routeParser(const QVariant &vRouteBase)
{
    auto routeBase=vRouteBase.toString();
    if(routeBase.contains(QStringLiteral("/"))){
        auto route=QStringLiteral("/")+routeBase.trimmed().toUtf8();
        while(route.contains("//"))
            route=route.replace(QStringLiteral("//"), QStringLiteral("/"));
        while(route.contains(QStringLiteral(" ")))
            route=route.replace(QStringLiteral(" "), "");
        return route.toLower();
    }
    return routeBase.toUtf8();
}

const QByteArray routeExtractMethod(const QString &routeBase)
{
    if(routeBase.contains(QStringLiteral("/")))
        return routeBase.split(QStringLiteral("/")).last().toUtf8().toLower();
    return {};
}

const QByteArray routeExtract(const QString &routeBase)
{
    if(routeBase.contains(QStringLiteral("/"))){
        auto lst=routeBase.split(QStringLiteral("/"));
        lst.takeLast();
        return lst.join(QStringLiteral("/")).toUtf8().toLower();
    }
    return {};
}

const QString headerFormatName(const QString &name)
{
    auto sname=name.trimmed();
    auto separator=QStringList{QStringLiteral("-")};
    QStringList newHeaderName;
    for(auto &v:separator){
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

const QVariantHash toMapResquest(int method, const QVariant &request_url, const QString &request_body, const QVariantHash &request_parameters, const QString &response_body, const QVariantHash &request_header, const QDateTime&request_start, const QDateTime&request_finish)
{
    Q_DECLARE_VU;
    auto request_method=RequestMethodName.value(method).toUpper();
    QVariantHash map{
        {QStringLiteral("url"), request_url},
        {QStringLiteral("method"), request_method},
        {QStringLiteral("header"), request_header},
        {QStringLiteral("start"), request_start},
        {QStringLiteral("finish"), request_finish}
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

    if(request_url.typeId()==QMetaType::QUrl){
        auto url=request_url.toUrl();
        cUrl=url.toString().split(QStringLiteral("/")).join(QStringLiteral("/"));
    }
    else{
        cUrl=request_url.toString();
    }
    if(method==QRpc::Post || method==QRpc::Put){
        map[QStringLiteral("body")]= request_parameters;
        auto body=request_body;
        body=response_body.trimmed().isEmpty()?"":QStringLiteral("-d '%1'").arg(body.replace('\n',' '));
        scUrl=QStringLiteral("curl --insecure -i -X %1 %2 %3 -G '%4'").arg(request_method, headers.join(' '), body, cUrl).trimmed();
    }
    else{
        QStringList params;
        map[QStringLiteral("parameter")]= request_parameters;
        Q_V_HASH_ITERATOR(request_parameters){
            i.next();
            auto k=i.key();
            params<<QStringLiteral("-d %1=%2").arg(k, Util::parseQueryItem(i.value()));
        }
        scUrl=QStringLiteral("curl --insecure -i -X %1 %2 -G '%3' %4").arg(request_method, headers.join(' '), cUrl, params.join(' ')).trimmed();
    }

    map[QStringLiteral("curl")]=scUrl;
    return map;
}

}
}
