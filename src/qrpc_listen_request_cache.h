#pragma once

#include "./qrpc_global.h"
#include "./qrpc_listen_request.h"

namespace QRpc {

class Listen;
class ListenRequest;
class ListenRequestCachePvt;
//!
//! \brief The ListenRequestCache class
//!
class Q_RPC_EXPORT ListenRequestCache : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief ListenRequestCache
    //! \param parent
    //!
    explicit ListenRequestCache(Listen *parent = nullptr);

    //!
    //! \brief clear
    //!
    virtual void clear();

    //!
    //! \brief toRequest
    //! \param uuid
    //! \return
    //!
    ListenRequest &toRequest(const QUuid &uuid);

    //!
    //! \brief createRequest
    //! \return
    //!
    ListenRequest &createRequest();

    //!
    //! \brief createRequest
    //! \param vRequest
    //! \return
    //!
    ListenRequest &createRequest(const QVariant &vRequest);

private:
    ListenRequestCachePvt *p = nullptr;
};

} // namespace QRpc
