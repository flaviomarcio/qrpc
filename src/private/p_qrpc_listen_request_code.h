#pragma once

#include "../qrpc_global.h"
#include <QNetworkReply>

namespace QRpc {

//!
//! \brief The ListenRequestCode class
//!
class Q_RPC_EXPORT ListenRequestCode : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief The Code enum
    //!
    enum Code{//https://developer.mozilla.org/pt-BR/docs/Web/HTTP/Status
        //Respostas de informação (100-199),
         ssContinue                     = 100
        ,ssSwitchingProtocols           = 101

        //Respostas de sucesso (200-299),
        ,ssOK                           = 200
        ,ssCreated                      = 201
        ,ssAccepted                     = 202
        ,ssNonAuthoritativeInformation  = 203
        ,ssNoContent                    = 204
        ,ssResetContent                 = 205
        ,ssPartialContent               = 206

        //Redirecionamentos (300-399)
        ,ssMultipleChoices              = 300
        ,ssMovedPermanently             = 301
        ,ssFound                        = 302
        ,ssSeeOther                     = 303
        ,ssNotModified                  = 304
        ,ssUseProxy                     = 305
        ,ssTemporaryRedirect            = 307

        //Erros do cliente (400-499)
        ,ssBadRequest                   = 400
        ,ssUnauthorized                 = 401
        ,ssPaymentRequired              = 402
        ,ssForbidden                    = 403
        ,ssNotFound                     = 404
        ,ssMethodNotAllowed             = 405
        ,ssNotAcceptable                = 406
        ,ssProxyAuthenticationRequired  = 407
        ,ssRequestTimeout               = 408
        ,ssConflict                     = 409
        ,ssGone                         = 410
        ,ssLengthRequired               = 411
        ,ssPreconditionFailed           = 412
        ,ssPayloadTooLarge              = 413
        ,ssURITooLong                   = 414
        ,ssUnsupportedMediaType         = 415
        ,ssRangeNotSatisfiable          = 416
        ,ssExpectationFailed            = 417
        ,ssUpgradeRequired              = 426

        //Erros do servidor (500-599).
        ,ssInternalServerError          = 500
        ,ssNotImplemented               = 501
        ,ssBadGateway                   = 502
        ,ssServiceUnavailable           = 503
        ,ssGatewayTimeout               = 504
        ,ssHTTPVersionNotSupported      = 505
    };

    //!
    //! \brief ListenRequestCode
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenRequestCode(QObject *parent = nullptr);

    ~ListenRequestCode();

    //!
    //! \brief operator =
    //! \param v
    //! \return
    //!
    ListenRequestCode&operator=(const QVariant &v);

    //!
    //! \brief emptyData
    //! \return
    //!
    virtual QVariant emptyData();

    //!
    //! \brief isOK
    //! \return
    //!
    bool isOK();

    //!
    //! \brief code
    //! \return
    //!
    int code();
    int code(const QVariant &code);
    ListenRequestCode&setCode();

    //!
    //! \brief reasonPhrase
    //! \return
    //!
    QString reasonPhrase();

    //!
    //! \brief reasonPhrase
    //! \param stateCode
    //! \return
    //!
    static const QString reasonPhrase(const int stateCode);

    //!
    //! \brief qt_network_error_phrase
    //! \param networkError
    //! \return
    //!
    static const QString qt_network_error_phrase(const QNetworkReply::NetworkError networkError);

    //!
    //! \brief setCode
    //! \param code
    //! \param phrase
    //! \return
    //!
    ListenRequestCode&setCode(QVariant code, const QString &phrase={});


    ListenRequestCode&setContinue(const QString &phrase={});
    ListenRequestCode&setSwitchingProtocols(const QString &phrase={});
    ListenRequestCode&setOK(const QString &phrase={});
    ListenRequestCode&setCreated(const QString &phrase={});
    ListenRequestCode&setAccepted(const QString &phrase={});
    ListenRequestCode&setNonAuthoritativeInformation(const QString &phrase={});
    ListenRequestCode&setNoContent(const QString &phrase={});
    ListenRequestCode&setResetContent(const QString &phrase={});
    ListenRequestCode&setPartialContent(const QString &phrase={});
    ListenRequestCode&setMultipleChoices(const QString &phrase={});
    ListenRequestCode&setMovedPermanently(const QString &phrase={});
    ListenRequestCode&setFound(const QString &phrase={});
    ListenRequestCode&setSeeOther(const QString &phrase={});
    ListenRequestCode&setNotModified(const QString &phrase={});
    ListenRequestCode&setUseProxy(const QString &phrase={});
    ListenRequestCode&setTemporaryRedirect(const QString &phrase={});
    ListenRequestCode&setBadRequest(const QString &phrase={});
    ListenRequestCode&setUnauthorized(const QString &phrase={});
    ListenRequestCode&setPaymentRequired(const QString &phrase={});
    ListenRequestCode&setForbidden(const QString &phrase={});
    ListenRequestCode&setNotFound(const QString &phrase={});
    ListenRequestCode&setMethodNotAllowed(const QString &phrase={});
    ListenRequestCode&setNotAcceptable(const QString &phrase={});
    ListenRequestCode&setProxyAuthenticationRequired(const QString &phrase={});
    ListenRequestCode&setRequestTimeout(const QString &phrase={});
    ListenRequestCode&setConflict(const QString &phrase={});
    ListenRequestCode&setGone(const QString &phrase={});
    ListenRequestCode&setLengthRequired(const QString &phrase={});
    ListenRequestCode&setPreconditionFailed(const QString &phrase={});
    ListenRequestCode&setPayloadTooLarge(const QString &phrase={});
    ListenRequestCode&setURITooLong(const QString &phrase={});
    ListenRequestCode&setUnsupportedMediaType(const QString &phrase={});
    ListenRequestCode&setRangeNotSatisfiable(const QString &phrase={});
    ListenRequestCode&setExpectationFailed(const QString &phrase={});
    ListenRequestCode&setUpgradeRequired(const QString &phrase={});
    ListenRequestCode&setInternalServerError(const QString &phrase={});
    ListenRequestCode&setNotImplemented(const QString &phrase={});
    ListenRequestCode&setBadGateway(const QString &phrase={});
    ListenRequestCode&setServiceUnavailable(const QString &phrase={});
    ListenRequestCode&setGatewayTimeout(const QString &phrase={});
    ListenRequestCode &setHTTPVersionNotSupported(const QString &phrase={});

};

}
