#pragma once

#include "../qrpc_controller_setting.h"
#include "../qrpc_controller_options.h"
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace QRpc {

class ControllerOptionsPrv{
public:
    QVariant settingsFileName;
    ControllerSetting settingsDefault;
    QVariantHash settingBody;
    QVariantHash arguments;
    QHash<QString, ControllerSetting*> settings;
    ControllerOptions*parent=nullptr;
    QObject*parentParent=nullptr;

    explicit ControllerOptionsPrv(ControllerOptions *parent);

    virtual ~ControllerOptionsPrv();

    void init();

    bool isLoaded();

    bool isEmpty();

    void clear();

    QVariantHash toHash();

    ControllerSetting &settingGetCheck(const QString &settingName);

    ControllerOptions &insert(const QVariantHash &value);

    bool v_load(const QVariant &v);

    bool load(QObject *settingsObject);

    bool load(const QStringList &settingsFileName);

    bool load(const QString &fileName);

    bool load(const QVariantHash &settings);

};

}
