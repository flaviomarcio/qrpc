#pragma once

#include "./qrpc_global.h"
#include "./qrpc_controller_setting.h"
#include <QSettings>

namespace QRpc {
class ControllerOptionsPrv;
//!
//! \brief The ControllerOptions class
//!
class Q_RPC_EXPORT ControllerOptions : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief ControllerOptions
    //! \param parent
    //!
    Q_INVOKABLE explicit ControllerOptions(QObject *parent = nullptr);
    explicit ControllerOptions(const QStringList&settingFileName, QObject *parent = nullptr);
    explicit ControllerOptions(const QString &settingFileName, QObject *parent = nullptr);
    ~ControllerOptions();

    //!
    //! \brief clear
    //!
    virtual void clear();

    //!
    //! \brief insert
    //! \param value
    //! \return
    //!
    virtual ControllerOptions &insert(ControllerSetting &value);
    virtual ControllerOptions &insert(const QVariantHash &value);

    //!
    //! \brief setting
    //! \return
    //!
    virtual ControllerSetting &setting();
    virtual ControllerSetting &setting(const QString &value);

    //!
    //! \brief load
    //! \param settings
    //! \return
    //!
    virtual bool load(const QVariant &settings);
    virtual bool load(const ControllerOptions &manager) const;
    virtual bool load(QObject *settingsObject) const;

    //!
    //! \brief settingsFileName
    //! \return
    //!
    virtual QVariant settingsFileName() const;

    //!
    //! \brief operator <<
    //! \param value
    //! \return
    //!
    ControllerOptions &operator<<(ControllerSetting&value);

    //!
    //! \brief settingBody
    //! \return
    //!
    //! full settings source
    virtual QVariantHash settingBody() const;

    //!
    //! \brief arguments
    //! \return
    //!
    //! arguments with settings
    virtual QVariantHash arguments() const;
    virtual void setArguments(const QVariantHash &value);

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantMap toMap() const;

    //!
    //! \brief toHash
    //! \return
    //!
    virtual QVariantHash toHash() const;

private:
    ControllerOptionsPrv *p=nullptr;
};

}
