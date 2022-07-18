#pragma once

#include <QMetaMethod>
#include <QMetaObject>
#include <QObject>
#include "./qrpc_global.h"
#include "../../qstm/src/qstm_object.h"

#define QRPC_CONTROLLER_ROUTER_REGISTER(className) \
    const auto __router_register##className = QRpc::ControllerRouter::installDefaultRouter( \
        &className::staticMetaObject);

namespace QRpc {
class ListenRequest;
class ControllerRouterPvt;
//!
//! \brief The ControllerRouter class
//!
class Q_RPC_EXPORT ControllerRouter : public QStm::Object
{
    Q_OBJECT
public:
    //!
    //! \brief ControllerRouter
    //! \param parent
    //!
    Q_INVOKABLE explicit ControllerRouter(QObject *parent = nullptr);

    //!
    //! \brief router
    //! \param request
    //! \param metaMethod
    //! \return
    //!
    virtual ResultValue &router(ListenRequest *request, QMetaMethod &metaMethod);

    //!
    //! \brief installDefaultRouter
    //! \param metaObject
    //! \return
    //!
    Q_INVOKABLE static bool installDefaultRouter(const QMetaObject *metaObject);

    //!
    //! \brief newRouter
    //! \param parent
    //! \return
    //!
    static ControllerRouter *newRouter(QObject *parent);

private:
    ControllerRouterPvt *p = nullptr;
};
} // namespace QRpc
