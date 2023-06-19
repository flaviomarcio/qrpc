#include "./qrpc_controller_router.h"
#include "./qrpc_listen_request.h"

namespace QRpc {

class ConstsControllerRouter
{
public:
    QList<const QMetaObject *> defaultRouters;
    explicit ConstsControllerRouter() { defaultRouters << &ControllerRouter::staticMetaObject; }
    virtual ~ConstsControllerRouter() {}
};
Q_GLOBAL_STATIC_WITH_ARGS(ConstsControllerRouter,
                          constsControllerRouter,
                          (ConstsControllerRouter()))

class ControllerRouterPvt:public QObject
{
public:
    QRpc::ControllerRouter *ControllerRouter = nullptr;
    explicit ControllerRouterPvt(QRpc::ControllerRouter *parent):QObject{parent}
    {
        this->ControllerRouter = parent;
    }
};

ControllerRouter::ControllerRouter(QObject *parent) : QStm::Object{parent}
{
    this->p = new ControllerRouterPvt{this};
}

ResultValue &ControllerRouter::router(QRpc::ListenRequest *request, QMetaMethod &metaMethod)
{
    Q_UNUSED(request)
    Q_UNUSED(metaMethod)
    return this->lr();
}

bool ControllerRouter::installDefaultRouter(const QMetaObject *metaObject)
{
    constsControllerRouter->defaultRouters.clear();
    if (metaObject != nullptr) {
        constsControllerRouter->defaultRouters.append(metaObject);
        return true;
    }
    constsControllerRouter->defaultRouters.append(&ControllerRouter::staticMetaObject);
    return false;
}

ControllerRouter *ControllerRouter::newRouter(QObject *parent)
{
    ControllerRouter *router = nullptr;

    auto &defaultRouters = constsControllerRouter->defaultRouters;
    auto &defaultRouter = defaultRouters.first();

    if (defaultRouters.isEmpty())
        return new ControllerRouter(parent);

    auto object = defaultRouter->newInstance(Q_ARG(QObject *, parent));
    router = dynamic_cast<ControllerRouter *>(object);
    if (router != nullptr)
        return router;

    if (object != nullptr)
        delete object;

    return nullptr;
}

} // namespace QRpc
