#include "./p_qrpc_controller_options.h"

namespace QRpc {

ControllerOptionsPrv::ControllerOptionsPrv(ControllerOptions *parent):settingsDefault(parent)
{
    this->parent=parent;
    this->init();
}

ControllerOptionsPrv::~ControllerOptionsPrv()
{
    this->clear();
}

void ControllerOptionsPrv::init()
{
}

bool ControllerOptionsPrv::isLoaded()
{
    QHashIterator<QString, ControllerSetting*> i(this->settings);
    while (i.hasNext()) {
        i.next();
        if(i.key().trimmed().isEmpty())
            continue;

        auto &v=i.value();
        if(v->isValid())
            return true;
    }
    return false;
}

bool ControllerOptionsPrv::isEmpty()
{
    QHashIterator<QString, ControllerSetting*> i(this->settings);
    while (i.hasNext()) {
        i.next();
        auto &v=i.value();
        if(v->isValid())
            return false;
    }
    return true;
}

void ControllerOptionsPrv::clear()
{
    auto _detail=this->settings.values();
    this->settings.clear();
    qDeleteAll(_detail);
}

QVariantHash ControllerOptionsPrv::toHash()
{
    QVariantHash vSettings;
    auto vList=QList<ControllerSetting*>()<<&this->settingsDefault;
    vList=vList+this->settings.values();
    for(auto &v:vList)
        vSettings.insert(v->name(), v->toHash());
    return vSettings;
}

ControllerSetting &ControllerOptionsPrv::settingGetCheck(const QString &settingName)
{
    const auto p1=settingName.toLower().trimmed();
    for(auto &setting:this->settings){
        if(setting->macth(p1)){
            return*setting;
        }
    }
    static ControllerSetting defaultControllerSetting;
    defaultControllerSetting.setEnabled(false);
    return defaultControllerSetting;
    return defaultControllerSetting;
}

ControllerOptions &ControllerOptionsPrv::insert(const QVariantHash &value)
{
    auto &p=*this;
    auto vValue=value;
    if(vValue.isEmpty())
        return*this->parent;

    auto name=vValue.value(qsl("name")).toByteArray().trimmed();
    if(name.isEmpty())
        return*this->parent;
    auto setting=p.settings.value(name);
    if(setting!=nullptr)
        setting->deleteLater();

    auto l=QStringList{QT_STRINGIFY2(activityLimit),QT_STRINGIFY2(activityInterval)};
    for(auto &property:l){
        auto v=vValue.value(property);
        if(!v.isValid())
            continue;

        if(v.toLongLong()>0)
            continue;

        v=ControllerSetting::parseInterval(v);
        vValue[property]=v;
    }
    setting=new ControllerSetting();
    setting->fromHash(vValue);
    setting->setName(name);
    p.settings.insert(setting->name(), setting);
    return*this->parent;
}

bool ControllerOptionsPrv::v_load(const QVariant &v)
{
    switch (qTypeId(v)) {
    case QMetaType_QVariantList:
    case QMetaType_QStringList:
        return this->load(v.toStringList());
    case QMetaType_QVariantHash:
    case QMetaType_QVariantMap:
        return this->load(v.toHash());
    default:
        return this->load(v.toString());
    }
}

bool ControllerOptionsPrv::load(QObject *settingsObject)
{
    auto &p=*this;
    if(settingsObject==nullptr)
        return false;

    auto metaObject=settingsObject->metaObject();
    for(int methodIndex = 0; methodIndex < metaObject->methodCount(); ++methodIndex) {
        auto metaMethod = metaObject->method(methodIndex);
        if(metaMethod.parameterCount()>0)
            continue;

        auto methodName=QString(metaMethod.name()).toLower().trimmed();
        auto staticNames=QStringList{qsl("resourcesettings")};
        if(!staticNames.contains(methodName))
            continue;

        QVariant invokeReturn;
        auto argReturn=Q_RETURN_ARG(QVariant, invokeReturn);
        if(!metaMethod.invoke(settingsObject, argReturn))
            continue;

        return p.v_load(invokeReturn);
    }
    return false;
}

bool ControllerOptionsPrv::load(const QStringList &settingsFileName)
{
    QVariantList vList;
    auto &p=*this;
    for(auto &fileName:settingsFileName){
        QFile file(fileName);
        if(fileName.isEmpty()){
            continue;
        }

        if(!file.exists()){
#if Q_RPC_LOG
            sWarning()<<qsl("file not exists %1").arg(file.fileName());
#endif
            continue;
        }

        if(!file.open(QFile::ReadOnly)){
#if Q_RPC_LOG
            sWarning()<<qsl("%1, %2").arg(file.fileName(), file.errorString());
#endif
            continue;
        }

        auto bytes=file.readAll();
        file.close();
        QJsonParseError*error=nullptr;
        auto doc=QJsonDocument::fromJson(bytes, error);
        if(error!=nullptr){
#if Q_RPC_LOG
            sWarning()<<qsl("%1, %2").arg(file.fileName(), error->errorString());
#endif
            continue;
        }

        if(doc.object().isEmpty()){
#if Q_RPC_LOG
            sWarning()<<qsl("object is empty, %1").arg(file.fileName());
#endif
            continue;
        }
        auto map=doc.object().toVariantHash();
        if(map.isEmpty())
            continue;

        vList<<map;
    }
    Q_DECLARE_VU;
    auto vHash=vu.vMerge(vList).toHash();
    if(p.load(vHash))
        this->settingsFileName=settingsFileName;
    else
        this->settingsFileName.clear();
    return p.isLoaded();
}

bool ControllerOptionsPrv::load(const QString &fileName)
{
    auto load=[this, fileName](){
        auto &p=*this;
        QFile file(fileName);
        if(fileName.trimmed().isEmpty()){
#if Q_RPC_LOG
            sWarning()<<qsl("not file settings");
#endif
            return false;
        }

        if(!file.exists()){
#if Q_RPC_LOG
            sWarning()<<qsl("file not exists %1").arg(file.fileName());
#endif
            return false;
        }

        if(!file.open(QFile::ReadOnly)){
#if Q_RPC_LOG
            sWarning()<<qsl("%1, %2").arg(file.fileName(), fileName);
#endif
            return false;
        }

        auto bytes=file.readAll();
        file.close();

        QJsonParseError*error=nullptr;
        auto doc=QJsonDocument::fromJson(bytes, error);
        if(error!=nullptr){
#if Q_RPC_LOG
            sWarning()<<qsl("%1, %2").arg(file.fileName(), error->errorString());
#endif
            return false;
        }

        if(doc.object().isEmpty()){
#if Q_RPC_LOG
            sWarning()<<qsl("object is empty, %1").arg(file.fileName());
#endif
            return false;
        }

        auto vHash=doc.object().toVariantHash();
        if(!vHash.contains(qsl("services"))){
#if Q_RPC_LOG
            sWarning()<<qsl("tag services not exists, %1").arg(file.fileName());
#endif
        }
        return p.load(vHash);
    };

    if(!load()){
        this->settingsFileName.clear();
        return false;
    }
    return true;
}

bool ControllerOptionsPrv::load(const QVariantHash &settings)
{
    auto &p=*this;
    p.settingBody=settings;

    auto arguments=settings.value(qsl("arguments"));

    switch (qTypeId(arguments)) {
    case QMetaType_QVariantList:
    case QMetaType_QStringList:
    {
        for(auto &v:arguments.toList()){
            auto l=v.toString().split(qsl("="));
            if(l.isEmpty()){
                continue;
            }

            if(l.size()==1){
                auto key=l.first();
                auto value=l.last();
                p.arguments[key]=value;
            }
            else{
                auto key=l.first().toLower();
                auto value=l.last();
                p.arguments[key]=value;
            }
        }
        break;
    }
    case QMetaType_QVariantHash:
    case QMetaType_QVariantMap:
    {
        Q_V_HASH_ITERATOR (arguments.toHash()){
            i.next();
            p.arguments[i.key().toLower()]=i.value();
        }
        break;
    }
    default:
        break;
    }


    QVariantHash defaultVariables{{qsl("hostName") , qsl("SERVICE_HOST")}};
    QVariantHash defaultValues;
    if(!defaultVariables.isEmpty()){
        Q_V_HASH_ITERATOR (defaultVariables){
            i.next();
            auto env = i.value().toByteArray();
            auto v = QByteArray(getenv(env)).trimmed();
            if(v.isEmpty())
                v = QByteArray(getenv(env.toLower())).trimmed();
            if(!v.isEmpty())
                defaultValues.insert(i.key(),v);
        }
    }

    auto defaultSetting=settings.value(qsl("default")).toHash();

    p.settingsDefault=defaultSetting;

    if(settings.contains(qsl("hostName")) && settings.contains(qsl("port"))){
        this->insert(settings);
        return this->isLoaded();
    }

    Q_V_HASH_ITERATOR (settings){
        i.next();
        auto value=i.value().toHash();
        value.insert(qsl("name"), i.key().trimmed());

        {
            Q_V_HASH_ITERATOR (defaultValues){
                i.next();
                if(!value.contains(i.key()))
                    value.insert(i.key(), i.value());
            }
        }

        {
            Q_V_HASH_ITERATOR (defaultSetting){
                i.next();
                if(!value.contains(i.key()))
                    value.insert(i.key(), i.value());
            }
        }
        this->insert(value);
    }

    return this->isLoaded();

}

}
