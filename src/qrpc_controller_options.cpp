#include "./qrpc_controller_options.h"
#include "./private/p_qrpc_controller_options.h"

namespace QRpc {

ControllerOptions::ControllerOptions(QObject *parent) : QObject{nullptr}
{
    this->p = new ControllerOptionsPrv(this);
    if(parent && parent->thread()==this->thread())
        this->setParent(parent);

    p->parentParent = parent;
    p->load(p->parentParent);
}

ControllerOptions::ControllerOptions(const QStringList &settingFileName, QObject *parent)
{
    this->p = new ControllerOptionsPrv(this);
    if(parent!=nullptr){
        if(parent->thread()==this->thread())
            this->setParent(parent);
    }

    p->load(settingFileName);
}

ControllerOptions::ControllerOptions(const QString &settingFileName, QObject *parent):QObject{parent}
{
    this->p = new ControllerOptionsPrv(this);
    if(parent!=nullptr){
        if(parent->thread()==this->thread())
            this->setParent(parent);
    }

    p->load(settingFileName);
}

ControllerOptions::~ControllerOptions()
{
    delete p;
}

void ControllerOptions::clear()
{

    p->clear();
}

ControllerOptions &ControllerOptions::insert(ControllerSetting &value)
{

    return p->insert(value.toHash());
}

ControllerOptions &ControllerOptions::insert(const QVariantHash &value)
{

    return p->insert(value);
}

ControllerSetting&ControllerOptions::setting()
{
    return this->setting({});
}

ControllerSetting&ControllerOptions::setting(const QString &value)
{

    return p->settingGetCheck(value);
}

bool ControllerOptions::load(const QVariant &settings)
{

    return p->v_load(settings);
}

bool ControllerOptions::load(const QVariantHash &settings)const
{

    return p->v_load(settings);
}

bool ControllerOptions::load(const ControllerOptions&manager)const
{

    return p->load(manager.toHash());
}

bool ControllerOptions::load(QObject *settingsObject)const
{

    return p->load(settingsObject);
}

QVariant ControllerOptions::settingsFileName()const
{

    return p->settingsFileName;
}

ControllerOptions &ControllerOptions::operator<<(ControllerSetting &value)
{
    return this->insert(value);
}

QVariantHash ControllerOptions::settingBody() const
{

    return p->settingBody;
}

QVariantHash ControllerOptions::arguments() const
{

    return p->arguments;
}

void ControllerOptions::setArguments(const QVariantHash &value)
{

    p->arguments = value;
}

QVariantMap ControllerOptions::toMap() const
{

    return QVariant(p->toHash()).toMap();
}

QVariantHash ControllerOptions::toHash() const
{

    return p->toHash();
}

}
