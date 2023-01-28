#pragma once

#include "./qrpc_controller.h"
#include "./qrpc_global.h"
#include "./qrpc_listen_request.h"
#include <QMetaMethod>
#include <QMetaObject>

namespace QRpc {
class ListenRequestParserPvt;
//!
//! \brief The ListenRequestParser class
//!
class Q_RPC_EXPORT ListenRequestParser : public QObject, public QRpcPrivate::AnotationsExtended
{
    Q_OBJECT
    friend class ListenQRPCSlotPvt;

public:
    //!
    //! \brief ListenRequestParser
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenRequestParser(QObject *parent = nullptr);



    virtual QStringList &basePath() const;

    //!
    //! \brief controller
    //! \return
    //!
    virtual Controller &controller();

protected:
    //!
    //! \brief setController
    //! \param value
    //!
    void setController(Controller *value);

public:
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
    //! \brief canRoute
    //! \param metaObject
    //! \param route
    //! \return
    //!test route is valid
    static bool canRoute(const QMetaObject &metaObject, const QString &route);

    //!
    //! \brief routeToMethod
    //! \param metaObject
    //! \param route
    //! \param outMethod
    //! \return
    //!return method to invoke
    static bool routeToMethod(const QMetaObject &metaObject,
                              const QString &route,
                              QMetaMethod &outMethod);

    //!
    //! \brief initializeInstalleds
    //! \param metaObject
    //!
    static void initializeInstalleds(const QMetaObject &metaObject);

protected:
    //!
    //! \brief parse
    //! \param metaMethod
    //! \return
    //!default method to parser request
    virtual bool parse(const QMetaMethod &metaMethod);

private:
    ListenRequestParserPvt *p = nullptr;
};

} // namespace QRpc
