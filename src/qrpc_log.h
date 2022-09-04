#pragma once

#include <QObject>

namespace QRpc {

//!
//! \brief logDir
//! \return
//!
QString &logDir();

//!
//! \brief logRegister
//! \return
//!
bool logRegister();

//!
//! \brief logFile
//! \param extension
//! \param path
//! \return
//!
QString logFile(const QString &extension, const QString &path={});

} // namespace QOrm

