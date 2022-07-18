#pragma once

#include <QObject>
#include <QVariant>
#include <QString>
#include <QByteArray>
#include <QUuid>
#include <QUrl>

namespace QRpc {
namespace Util {

//!
//! \brief routeParser
//! \param vRouteBase
//! \return
//!
const QString routeParser(const QVariant &vRouteBase);

//!
//! \brief routeExtractMethod
//! \param routeBase
//! \return
//!
const QByteArray routeExtractMethod(const QString &routeBase);

//!
//! \brief routeExtract
//! \param routeBase
//! \return
//!
const QByteArray routeExtract(const QString &routeBase);

//!
//! \brief headerFormatName
//! \param name
//! \return
//!
const QString headerFormatName(const QString &name);

//!
//! \brief parseQueryItem
//! \param v
//! \return
//!
const QString parseQueryItem(const QVariant &v);

//!
//! \brief toMapResquest
//! \param method
//! \param request_url
//! \param request_body
//! \param request_parameters
//! \param response_body
//! \param request_header
//! \param request_start
//! \param request_finish
//! \return
//!
const QVariantHash toMapResquest(int method, const QVariant &request_url, const QString &request_body, const QVariantHash &request_parameters, const QString &response_body, const QVariantHash &request_header, const QDateTime&request_start, const QDateTime&request_finish);

//!
//! \brief routeExtractMethod
//! \param routeBase
//! \return
//!
const QByteArray routeExtractMethod(const QString &routeBase);

//!
//! \brief routeExtract
//! \param routeBase
//! \return
//!
const QByteArray routeExtract(const QString &routeBase);

//!
//! \brief headerFormatName
//! \param name
//! \return
//!
const QString headerFormatName(const QString &name);

//!
//! \brief parseQueryItem
//! \param v
//! \return
//!
const QString parseQueryItem(const QVariant &v);

//!
//! \brief toMapResquest
//! \param method
//! \param request_url
//! \param request_body
//! \param request_parameters
//! \param response_body
//! \param request_header
//! \param request_start
//! \param request_finish
//! \return
//!
const QVariantHash toMapResquest(int method, const QVariant &request_url, const QString &request_body, const QVariantHash &request_parameters, const QString &response_body, const QVariantHash &request_header, const QDateTime&request_start, const QDateTime&request_finish);

}
}
