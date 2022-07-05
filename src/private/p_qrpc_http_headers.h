#pragma once

#include "../qrpc_global.h"

namespace QRpc {
class HttpHeadersPvt;
//!
//! \brief The HttpHeaders class
//!
class Q_RPC_EXPORT HttpHeaders: public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit HttpHeaders(QObject*parent=nullptr);

    explicit HttpHeaders(const QVariant &v,QObject*parent=nullptr);

    //!
    //! \brief clear
    //! \return
    //!
    virtual HttpHeaders &clear();

    //!
    //! \brief rawHeader
    //! \return
    //!
    virtual QVariantHash &rawHeader() const;
    virtual QVariant rawHeader(const QString &headername) const;
    virtual HttpHeaders &setRawHeader(const QVariantHash&rawHeader);
    virtual HttpHeaders &setRawHeader(const QString &header, const QVariant &value);

    //!
    //! \brief addRawHeader
    //! \param rawHeader
    //! \return
    //!
    virtual HttpHeaders &addRawHeader(const QVariantHash&rawHeader);
    virtual HttpHeaders &addRawHeader(const QString &header, const QVariant &value);

    //!
    //! \brief contentType
    //! \return
    //!
    virtual QVariant contentType() const;
    virtual HttpHeaders &setContentType(const int contentType);
    virtual HttpHeaders &setContentType(const QVariant &v);

    //!
    //! \brief isContentType
    //! \param contentType
    //! \return
    //!
    virtual bool isContentType(int contentType)const;

    //!
    //! \brief defaultContentType
    //! \return
    //!
    static QRpc::ContentType defaultContentType();

    //!
    //! \brief contentDisposition
    //! \return
    //!
    virtual QVariant contentDisposition() const;

    //!
    //! \brief setAuthorization
    //! \param authorization
    //! \param type
    //! \param credentials
    //! \return
    //!
    virtual HttpHeaders &setAuthorization(const QString &authorization, const QString &type, const QVariant &credentials);
    virtual HttpHeaders &setAuthorization(const QString &authorization, const AuthorizationType&type, const QVariant &credentials);
    virtual HttpHeaders &setAuthorization(const AuthorizationType&type, const QVariant &credentials);
    virtual HttpHeaders &setAuthorization(const AuthorizationType&type, const QVariant &userName, const QVariant &passWord);
    //virtual Header&setAuthorization(const QString &type, const QVariant &credentials);

    //!
    //! \brief setProxyAuthorization
    //! \param type
    //! \param credentials
    //! \return
    //!
    virtual HttpHeaders &setProxyAuthorization(const AuthorizationType&type, const QVariant &credentials);
    virtual HttpHeaders &setProxyAuthorization(const QString &type, const QVariant &credentials);

    //!
    //! \brief setWWWAuthenticate
    //! \param type
    //! \param credentials
    //! \return
    //!
    virtual HttpHeaders &setWWWAuthenticate(const AuthorizationType&type, const QVariant &credentials);
    virtual HttpHeaders &setWWWAuthenticate(const QString &type, const QVariant &credentials);

    //!
    //! \brief cookies
    //! \return
    //!
    virtual QVariant cookies() const;
    virtual HttpHeaders &setCookies(const QVariant &cookie);

    //!
    //! \brief authorization
    //! \param authorization
    //! \param type
    //! \return
    //!
    virtual QVariant authorization(const QString &authorization, const QString &type);
    virtual QVariant authorization(const QString &authorization, const AuthorizationType &type);
    virtual QVariant authorization(const AuthorizationType&type);
    virtual QVariant authorization(const QString &type);

    //!
    //! \brief proxyAuthorization
    //! \param type
    //! \return
    //!
    virtual QVariant proxyAuthorization(const AuthorizationType&type);
    virtual QVariant proxyAuthorization(const QString &type);

    //!
    //! \brief wwwAuthenticate
    //! \param type
    //! \return
    //!
    virtual QVariant wwwAuthenticate(const AuthorizationType&type);
    virtual QVariant wwwAuthenticate(const QString &type);

    //!
    //! \brief print
    //! \param output
    //! \return
    //!
    virtual HttpHeaders &print(const QString &output=QString());

    //!
    //! \brief printOut
    //! \param output
    //! \return
    //!
    virtual QStringList printOut(const QString &output=QString());

    //!
    //! \brief operator =
    //! \param v
    //! \return
    //!
    HttpHeaders &operator =(const QVariant &v);
    HttpHeaders &operator <<(const QVariant &v);
private:
    HttpHeadersPvt *p = nullptr;
};

}
