#pragma once

#include "./p_qrpc_http_headers.h"
#include <QNetworkReply>

namespace QRpc {
class HttpResponsePvt;
//!
//! \brief The HttpResponse class
//!
class Q_RPC_EXPORT HttpResponse: public QObject{
    Q_OBJECT
public:
    //!
    //! \brief HttpResponse
    //! \param parent
    //!
    Q_INVOKABLE explicit HttpResponse(QObject *parent=nullptr);

    //!
    //! \brief header
    //! \return
    //!
    QRpc::HttpHeaders &header() const;

    //!
    //! \brief setBody
    //! \param vBody
    //!
    void setBody(const QVariant &vBody);

    //!
    //! \brief body
    //! \return
    //!
    QByteArray &body() const;

    //!
    //! \brief bodyVariant
    //! \return
    //!
    QVariant bodyVariant() const;

    //!
    //! \brief bodyMap
    //! \return
    //!
    QVariantMap bodyMap() const;

    //!
    //! \brief bodyHash
    //! \return
    //!
    QVariantHash bodyHash() const;

    //!
    //! \brief bodyObject
    //! \return
    //!
    QVariantHash bodyObject() const;

    //!
    //! \brief bodyList
    //! \return
    //!
    QVariantList bodyList() const;

    //!
    //! \brief bodyArray
    //! \return
    //!
    QVariantList bodyArray() const;

    //!
    //! \brief bodyToList
    //! \return
    //!
    QVariantList bodyToList() const;

    //!
    //! \brief statusCode
    //! \return
    //!
    int&statusCode() const;

    //!
    //! \brief reasonPhrase
    //! \return
    //!
    QString &reasonPhrase() const;

    //!
    //! \brief qtStatusCode
    //! \return
    //!
    QNetworkReply::NetworkError &qtStatusCode() const;

    //!
    //! \brief toHash
    //! \return
    //!
    QVariantHash toHash() const;

    //!
    //! \brief toResponse
    //! \return
    //!
    QVariantHash toResponse() const;

    //!
    //! \brief isOk
    //! \return
    //!
    bool isOk() const;

    //!
    //! \brief isCreated
    //! \return
    //!
    bool isCreated() const;

    //!
    //! \brief isNotFound
    //! \return
    //!
    bool isNotFound() const;

    //!
    //! \brief isUnAuthorized
    //! \return
    //!
    bool isUnAuthorized() const;

    //!
    //! \brief setResponse
    //! \param objectResponse
    //! \return
    //!
    HttpResponse &setResponse(QObject *objectResponse);

    //!
    //! \brief toString
    //! \return
    //!
    QString toString() const;

    //!
    //! \brief operator bool
    //!
    operator bool() const;

    //!
    //! \brief print
    //! \param output
    //! \return
    //!
    HttpResponse &print(const QString &output={});

    //!
    //! \brief printOut
    //! \param output
    //! \return
    //!
    QStringList printOut(const QString &output={});

private:
    HttpResponsePvt *p = nullptr;
};


}
