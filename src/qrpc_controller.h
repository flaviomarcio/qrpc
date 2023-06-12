#pragma once

#include <QList>
#include <QMetaMethod>
#include <QMetaObject>
#include "./qrpc_global.h"
#include "./qrpc_controller_annotation.h"
#include "./qrpc_listen_request.h"

namespace QRpc {

class ListenRequest;
class ListenRequestParser;
class Server;
class ControllerPvt;
//!
//! \brief The Controller class
//!
class Q_RPC_EXPORT Controller : public QObject, public QRpcPrivate::AnotationsExtended
{
    Q_OBJECT
    friend class Server;
    friend class Request;
    friend class ListenQRPC;
    friend class ListenQRPCSlotPvt;

    Q_PROPERTY(QStringList basePath READ basePath NOTIFY basePathChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
public:
    struct MethodInfo{
    public:
        int order=-1;
        QMetaMethod method;
        bool excluded=false;
        QByteArray name;
        QByteArray description;
        QByteArray basePath;
        QByteArray group;
        QByteArray path;
        QByteArray fullPath;
        QStringList methods;
        QStringList rules;
        QAnnotation::Collection annotations;
    };

    typedef QVector<MethodInfo> MethodInfoCollection;


public:
    //!
    //! \brief Controller
    //! \param parent
    //!
    Q_INVOKABLE explicit Controller(QObject *parent = nullptr);

    //!
    //! \brief invokableMethod
    //! \return
    //!
    Controller::MethodInfoCollection invokableMethod()const;

    //!
    //! \brief basePath
    //! \return
    //!
    virtual QStringList &basePath() const;

    //!
    //! \brief module
    //! \return
    //!
    virtual QString module() const;

    //!
    //! \brief description
    //! \return
    //!
    virtual QString description() const;

    //!
    //! \brief enabled
    //! \return
    //!
    virtual bool enabled() const;

    //!
    //! \brief setEnabled
    //! \param enabled
    //!
    virtual Controller &setEnabled(bool enabled);

    //!
    //! \brief request
    //! \return
    //!
    virtual ListenRequest &request();

    //!
    //! \brief rq
    //! \return
    //!
    virtual ListenRequest &rq();

    //!
    //! \brief requestSettings
    //! \return
    //!
    bool requestSettings();

    //!
    //! \brief canOperation
    //! \param method
    //! \return
    //!
    virtual bool canOperation(const QMetaMethod &method);

    //!
    //! \brief canAuthorization
    //! \return
    //!
    virtual bool canAuthorization();

    //!
    //! \brief canAuthorization
    //! \param method
    //! \return
    //!
    virtual bool canAuthorization(const QMetaMethod &method);

    //!
    //! \brief beforeAuthorization
    //! \return
    //!
    virtual bool beforeAuthorization();

    //!
    //! \brief authorization
    //! \return
    //!
    virtual bool authorization();

    //!
    //! \brief authorization
    //! \param method
    //! \return
    //!
    virtual bool authorization(const QMetaMethod &method);

    //!
    //! \brief afterAuthorization
    //! \return
    //!
    virtual bool afterAuthorization();

    //!
    //! \brief requestBeforeInvoke
    //! \return
    //!
    virtual bool requestBeforeInvoke();

    //!
    //! \brief requestAfterInvoke
    //! \return
    //!
    virtual bool requestAfterInvoke();

    //!
    //! \brief server
    //! \return
    //!
    virtual Server *server();

    //!
    //! \brief install
    //! \param metaObject
    //! \return
    //!
    static const QMetaObject &install(const QMetaObject &metaObject);

    //!
    //! \brief installParser
    //! \param metaObject
    //! \return
    //!
    static const QMetaObject &installParser(const QMetaObject &metaObject);

    //!
    //! \brief staticApiList
    //! \return
    //!
    static QVector<const QMetaObject *> &staticApiList();

    //!
    //! \brief staticApiParserList
    //! \return
    //!
    static QVector<const QMetaObject *> &staticApiParserList();

protected:
    //!
    //! \brief setServer
    //! \param server
    //!
    virtual QRpc::Controller &setServer(Server *server);

    //!
    //! \brief setRequest
    //! \param request
    //!
    virtual Controller &setRequest(ListenRequest &request);

signals:

    //!
    //! \brief basePathChanged
    //!
    void basePathChanged();

    //!
    //! \brief enabledChanged
    //!
    void enabledChanged();

private:
    ControllerPvt *p = nullptr;
    Protocol acceptedProtocols;
};

typedef Controller QRPCController;

} // namespace QRpc
