#include "./p_qrpc_util.h"
#include "../qrpc_types.h"
#include <QStm>

namespace QRpc {

namespace Util {

const QString routeParser(const QVariant &vRouteBase)
{
    auto routeBase=vRouteBase.toString();
    if(routeBase.contains(qsl("/"))){
        auto route=qsl("/")+routeBase.trimmed().toUtf8();
        while(route.contains("//"))
            route=route.replace(qbl("//"), qsl("/"));
        while(route.contains(qsl_space))
            route=route.replace(qsl_space, qbl_null);
        return route.toLower();
    }
    return routeBase.toUtf8();
}

const QByteArray routeExtractMethod(const QString &routeBase)
{
    if(routeBase.contains(qsl("/")))
        return routeBase.split(qsl("/")).last().toUtf8().toLower();
    return {};
}

const QByteArray routeExtract(const QString &routeBase)
{
    if(routeBase.contains(qsl("/"))){
        auto lst=routeBase.split(qsl("/"));
        lst.takeLast();
        return lst.join(qsl("/")).toUtf8().toLower();
    }
    return {};
}

const QString headerFormatName(const QString &name)
{
    auto sname=name.trimmed();
    auto separator=QStringList{qsl("-")};
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
    if(qTypeId(v)==QMetaType_QUuid)
        return v.toUuid().toString();
    return v.toString();
}

const QVariantHash toMapResquest(int method, const QVariant &request_url, const QString &request_body, const QVariantHash&request_parameters, const QString &response_body, const QVariantHash&request_header, const QDateTime&request_start, const QDateTime&request_finish)
{
    Q_DECLARE_VU;
    auto request_method=RequestMethodName.value(method).toUpper();
    QVariantHash map{
        {qsl("url"), request_url},
        {qsl("method"), request_method},
        {qsl("header"), request_header},
        {qsl("start"), request_start},
        {qsl("finish"), request_finish}
    };

    QStringList headers;
    QHashIterator<QString, QVariant> i(request_header);
    while (i.hasNext()) {
        i.next();
        const auto &k=i.key();
        auto v=i.value();
        auto typeId=qTypeId(v);
        v = QMetaTypeUtilVariantList.contains(typeId)?v.toStringList().join(qsl(";")):v;
        headers<<qsl("-H '%1: %2'").arg(k, v.toString());
    }

    QString scUrl, cUrl;

    if(qTypeId(request_url)==QMetaType_QUrl){
        auto url=request_url.toUrl();
        cUrl=url.toString().split(qsl("/")).join(qsl("/"));
    }
    else{
        cUrl=request_url.toString();
    }
    if(method==QRpc::Post || method==QRpc::Put){
        map[qsl("body")]= request_parameters;
        auto body=request_body;
        body=response_body.trimmed().isEmpty()?qsl_null:qsl("-d '%1'").arg(body.replace('\n',' '));
        scUrl=qsl("curl --insecure -i -X %1 %2 %3 -G '%4'").arg(request_method, headers.join(' '), body, cUrl).trimmed();
    }
    else{
        QStringList params;
        map[qsl("parameter")]= request_parameters;
        Q_V_HASH_ITERATOR(request_parameters){
            i.next();
            auto k=i.key();
            params<<qsl("-d %1=%2").arg(k, Util::parseQueryItem(i.value()));
        }
        scUrl=qsl("curl --insecure -i -X %1 %2 -G '%3' %4").arg(request_method, headers.join(' '), cUrl, params.join(' ')).trimmed();
    }

    map[qsl("curl")]=scUrl;
    return map;
}

}
}
