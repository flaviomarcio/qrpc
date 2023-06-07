#include "./qrpc_service_setting.h"

namespace QRpc {

class ServiceSettingPvt:public QObject
{
public:
    ServiceSetting *parent=nullptr;
    QVariantHash connection;
    explicit ServiceSettingPvt(ServiceSetting *parent):QObject{parent}
    {
        this->parent=parent;
    }
};

ServiceSetting::ServiceSetting(QObject *parent):QStm::SettingBase{parent}
{
    this->p=new ServiceSettingPvt{this};
}

QVariantHash ServiceSetting::connection() const
{
    return p->connection;
}

void ServiceSetting::setConnection(const QVariantHash &newConnection)
{
    if (p->connection == newConnection)
        return;
    p->connection = newConnection;
    emit connectionChanged();
}

void ServiceSetting::resetConnection()
{
    return setConnection({});
}

}
