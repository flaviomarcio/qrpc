#include "./p_qrpc_listen_qrpc.h"
#include "../qrpc_controller.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_parser.h"
#include "../qrpc_server.h"
#include "./p_qrpc_listen_qrpc_slot.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QMetaMethod>
#include <QMutex>

namespace QRpc {

#define dPvt() auto &p = *reinterpret_cast<ListenQRPCPvt *>(this->p)

class ListenQRPCPvt : public QObject
{
public:
    QMutex mutexLockerRunning;
    QMutex mutexLockerHash;
    QHash<QByteArray, const QMetaObject *> controller;
    QHash<QByteArray, const QMetaObject *> controllerParsers;
    ListenSlotCache listenSlotCache;
    ListenQRPC *listenQRPC = nullptr;
    QMap<QUuid, Listen *> listens;

    MultStringList controllerRedirect;
    ControllerMethodCollection controllerMethods;

    explicit ListenQRPCPvt(ListenQRPC *parent) : QObject{parent}
    {
        this->listenQRPC = dynamic_cast<ListenQRPC *>(this->parent());
    }

    virtual ~ListenQRPCPvt()
    {
        auto list = listenSlotCache.values();
        listenSlotCache.clear();
        qDeleteAll(list);
    }

//    auto basePathToStringList(const QVariant &vBasePath)
//    {
//        QStringList vList;
//        switch (qTypeId(vBasePath)) {
//        case QMetaType_QStringList:
//        case QMetaType_QVariantList: {
//            for (auto &v : vBasePath.toList()) {
//                auto path = v.toString().trimmed().trimmed().toLower();
//                if (path.isEmpty())
//                    continue;
//                vList << path;
//            }
//            break;
//        }
//        default:
//            vList = QStringList{vBasePath.toString().toLower()};
//        }
//        return vList;
//    }

    auto basePathParser(const QString &path, const QString &methodName)
    {
        auto __return = path.trimmed().simplified();
        __return = __return.replace(qsl("*"), qsl_null).toLower().toUtf8();
        __return = qbl("/") + __return + qbl("/") + methodName;
        while (__return.contains(qbl("\"")) || __return.contains(qbl("//"))) {
            __return = QString(__return)
                           .replace(qsl("\""), qsl_null)
                           .replace(qsl("//"), qsl("/"))
                           .toUtf8();
        }
        return __return.toUtf8();
    };

    void listenRegister(Listen *listen)
    {
        auto listenQRPC = this->listenQRPC;
        if (listen == listenQRPC)
            return;
        if (this->listens.contains(listen->uuid()))
            return;
        this->listens.insert(listen->uuid(), listen);
        listen->registerListenPool(listenQRPC);
        QObject::connect(listen, &Listen::rpcRequest, this, &ListenQRPCPvt::onRpcRequest);
    }

    void listenerFree()
    {
        auto listenSlotCacheAux = this->listenSlotCache;
        this->listenSlotCache.clear();
        for (auto &slot : listenSlotCacheAux) {
            for (auto &t : *slot) {
                t->quit();
                t->wait();
                t->start();
            }
        }
    }

    void apiMakeBasePath(QObject *makeObject, const QMetaObject *metaObject)
    {
        auto className = QByteArray(metaObject->className()).toLower().trimmed();

        if (controllerMethods.contains(className))
            return;

        auto methodList = controllerMethods.value(className);
        if (!methodList.isEmpty())
            return;

        auto controller = dynamic_cast<Controller *>(makeObject);
        if (controller == nullptr)
            return;

        auto controllerIsRedirect=controller->notation().contains(controller->apiRedirect);
        if (controllerIsRedirect)
            controllerRedirect.insert(className, controller->basePath());



#if Q_RPC_LOG_SUPER_VERBOSE
        sWarning() << "registered class : " << makeObject->metaObject()->className();
#endif

        static auto nottionExcludeMethod=QVariantList{controller->rqRedirect, controller->rqExcludePath};
        static ByteArrayVector methodBlackList=QRPC_METHOD_BACK_LIST;
        const auto &vBasePathList = controller->basePath();
        if (vBasePathList.isEmpty())
            return;

        for (auto i = 0; i < metaObject->methodCount(); ++i) {
            auto method = metaObject->method(i);

            if (method.methodType() != method.Method)
                continue;

            if (method.parameterCount() > 0) {
#if Q_RPC_LOG_SUPER_VERBOSE
                sWarning() << qsl("Method(%1) ignored").arg(mMth.name().constData());
#endif
                continue;
            }

            auto methodName = method.name().toLower();

            if (methodBlackList.contains(methodName)) {
#if Q_RPC_LOG_SUPER_VERBOSE
                sWarning() << qsl("Method(%1) in blacklist").arg(mMth.name().constData());
#endif
                continue;
            }

            if (methodName.startsWith(qbl("_"))) //ignore methods with [_] in start name
                continue;

            const auto &notations=controller->notation(method);

            if(notations.contains(nottionExcludeMethod))
                continue;

            for (auto &v : vBasePathList) {
                auto path = basePathParser(v, methodName);
                if (methodList.contains(path))
                    continue;
                methodList.insert(path, method);
            }
        }
        this->controllerMethods.insert(className, methodList);
    }

    void apiMakeBasePath()
    {
        controllerMethods.clear();
        auto server = this->listenQRPC->server();
        if (server == nullptr) {
            qWarning() << qsl("Invalid server");
            return;
        }

        this->controller.clear();
        for (auto &mObj : server->controllers()) {
            auto name = QString::fromUtf8(mObj->className()).toLower().toUtf8().toLower();

            QScopedPointer<QObject> sObj(mObj->newInstance(Q_ARG(QObject*, nullptr )));

            auto object=sObj.data();

            if (object == nullptr)
                continue;

            auto controller = dynamic_cast<Controller *>(object);
            if (controller == nullptr)
                continue;

            apiMakeBasePath(controller, mObj);
            this->controller.insert(name, mObj);
        }
    }

    void apiMakeBasePathParser()
    {
        auto server = this->listenQRPC->server();
        if (server == nullptr) {
            qWarning() << qsl("Invalid server");
            return;
        }
        this->controllerParsers.clear();
        for (auto &mObj : server->controllerParsers()) {
            auto name = QString::fromUtf8(mObj->className()).toLower().toUtf8().toLower();
            auto object = mObj->newInstance(Q_ARG(QObject*, nullptr));
            if (object == nullptr)
                continue;

            auto controller = dynamic_cast<ListenRequestParser *>(object);
            if (controller != nullptr)
                this->controllerParsers.insert(name, mObj);
            delete object;
            object = nullptr;
        }
    }

public slots:
    void onRpcRequest(QVariantHash vRequest, const QVariant &uploadedFiles)
    {
        ListenRequest request;
        if (!request.fromHash(vRequest)) {
            if (request.co().isOK())
                request.co().setBadRequest();
            return;
        }

        auto server = this->listenQRPC->server();

        if ((server != nullptr) && (!server->authorizationRequest(request))) {
            if (request.co().isOK())
                request.co().setUnauthorized();
            return;
        }

        auto vHash = request.toHash();
        ListenSlotList *listenSlotList = nullptr;
        { //locker
            QMutexLOCKER locker(&mutexLockerHash);
            auto md5 = request.hash();
            listenSlotList = this->listenSlotCache.value(md5);
            if (listenSlotList == nullptr) {
                listenSlotList = new ListenSlotList{};
                this->listenSlotCache[md5] = listenSlotList;
            }
        }

        auto requestInvoke = [&listenSlotList, &vHash, &uploadedFiles](ListenQRPCSlot *&thread) {
            thread = nullptr;
            for (auto &v : *listenSlotList) {
                if (!v->canRequestInvoke(vHash, uploadedFiles))
                    continue;
                thread = v;
                return true;
            }
            return false;
        };

        ListenQRPCSlot *thread = nullptr;
        while (!requestInvoke(thread)) {
            QMutexLOCKER locker(&mutexLockerHash);
            auto thread = new ListenQRPCSlot(this->listenQRPC);
            thread->start();
            listenSlotList->append(thread);
        }
    }
};

ListenQRPC::ListenQRPC(QObject *parent) : Listen(nullptr)
{
    Q_UNUSED(parent)
    this->p = new ListenQRPCPvt{this};
}

ListenQRPC::~ListenQRPC()
{
    p->mutexLockerRunning.tryLock(5000);//wait finish threads
    p->mutexLockerRunning.unlock();
}

void ListenQRPC::run()
{

    p->mutexLockerRunning.lock();

    p->listenerFree();

    Listen::run();

    p->listenerFree();

    p->mutexLockerRunning.unlock();
}

bool ListenQRPC::start()
{

    p->apiMakeBasePathParser();
    p->apiMakeBasePath();
    return Listen::start();
}

QHash<QByteArray, const QMetaObject *> &ListenQRPC::controllers()
{

    return p->controller;
}

QHash<QByteArray, const QMetaObject *> &ListenQRPC::controllerParsers()
{

    return p->controllerParsers;
}

ControllerMethodCollection &ListenQRPC::controllerMethods()
{

    return p->controllerMethods;
}

MultStringList &ListenQRPC::controllerRedirect()
{

    return p->controllerRedirect;
}

void ListenQRPC::registerListen(Listen *listen)
{

    p->listenRegister(listen);
}

Listen *ListenQRPC::childrenListen(QUuid uuid)
{

    auto listen = p->listens.value(uuid);
    return listen;
}

} // namespace QRpc
