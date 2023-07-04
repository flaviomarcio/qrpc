#pragma once

#include "../qrpc_global.h"
#include "../qrpc_types.h"
#include <QVariant>
#include <QVariantHash>

namespace QRpc {
class HttpHeadersPvt;
//!
//! \brief The HttpHeaders class
//!
class Q_RPC_EXPORT HttpHeaders: public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit HttpHeaders(QObject *parent=nullptr);

    explicit HttpHeaders(const QVariant &headers, QObject *parent=nullptr);

    //!
    //! \brief operator =
    //! \param v
    //! \return
    //!
    HttpHeaders &operator = (const QVariant &v);
    HttpHeaders &operator << (const QVariant &v);

    //!
    //! \brief clear
    //! \return
    //!
    virtual HttpHeaders &clear();


    //!
    //! \brief print
    //! \param output
    //! \return
    //!
    virtual HttpHeaders &print(const QString &output={});

    //!
    //! \brief printOut
    //! \param output
    //! \return
    //!
    virtual QStringList printOut(const QString &output={});

    //!
    //! \brief rawHeader
    //! \return
    //!
    virtual QVariantHash &rawHeader() const;
    virtual QVariant rawHeader(const QString &headername) const;
    virtual HttpHeaders &setRawHeader(const QVariantHash &rawHeader);
    virtual HttpHeaders &setRawHeader(const QString &header, const QVariant &value);

    //!
    //! \brief addRawHeader
    //! \param rawHeader
    //! \return
    //!
    virtual HttpHeaders &addRawHeader(const QVariantHash &rawHeader);
    virtual HttpHeaders &addRawHeader(const QString &header, const QVariant &value);

    //!
    //! \brief contentType
    //! \return
    //!
    virtual QVariant contentType() const;
    virtual HttpHeaders &setContentType(const QVariant &v);

    //!
    //! \brief isContentType
    //! \param contentType
    //! \return
    //!
    virtual bool isContentType(const QVariant &contentType)const;

    //!
    //! \brief defaultContentType
    //! \return
    //!
    static Types::ContentType defaultContentType();

    //!
    //! \brief contentDisposition
    //! \return
    //!
    virtual QVariant contentDisposition() const;

    //!
    //! \brief setAuthorization
    //! \param authorization
    //! \param authorizationScheme
    //! \param credentials
    //! \return
    //!
    virtual HttpHeaders &setAuthorization(const QString &headerName, const QVariant &authorizationScheme, const QVariant &credentials);
    virtual HttpHeaders &setAuthorization(const QVariant &authorizationScheme, const QVariant &credentials);
    virtual HttpHeaders &setAuthorizationBearer(const QVariant &token);
    virtual HttpHeaders &setAuthorizationService(const QVariant &token);
    virtual HttpHeaders &setAuthorizationBasic(const QVariant &base64Encode);
    virtual HttpHeaders &setAuthorizationBasic(const QVariant &userName, const QVariant &passWord);
    virtual QVariant authorization(const QString &headerName, const QVariant &authorizationScheme);
    virtual QVariant authorization(const QVariant &authorizationScheme);


    //!
    //! \brief setProxyAuthorization
    //! \param authorizationScheme
    //! \param credentials
    //! \return
    //!
    virtual HttpHeaders &setProxyAuthorization(const QVariant &authorizationScheme, const QVariant &credentials);
    virtual QVariant proxyAuthorization(const QVariant &authorizationScheme);

    //!
    //! \brief setWWWAuthenticate
    //! \param authorizationScheme
    //! \param credentials
    //! \return
    //!
    virtual HttpHeaders &setWWWAuthenticate(const QVariant &authorizationScheme, const QVariant &credentials);
    virtual QVariant wwwAuthenticate(const QVariant &authorizationScheme);

    //!
    //! \brief cookies
    //! \return
    //!
    virtual QVariant cookies() const;
    virtual HttpHeaders &cookies(const QVariant &cookie);

private:
    HttpHeadersPvt *p = nullptr;
};

}
