#include "./qrpc_log.h"
#include <QCoreApplication>
#include <QThread>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

namespace QRpc {

Q_GLOBAL_STATIC(QString, staticLogDir);
static bool staticLogRegister=false;

static void staticLogDirClear(const QString &ormLogDir)
{
    QStringList dir_found;
    QStringList dir_rm_file;
    dir_found.append(ormLogDir);
    while(!dir_found.isEmpty()){
        auto scanDir = dir_found.takeFirst();
        dir_rm_file.append(scanDir);
        QDir dir(scanDir);
        if(!dir.exists(scanDir))
            continue;

        dir.setFilter(QDir::AllDirs);
        for(auto &scanInDir:dir.entryList()){
            if(scanInDir==QStringLiteral(".") || scanInDir==QStringLiteral(".."))
                continue;

            auto dir=QStringLiteral("%1/%2").arg(scanDir, scanInDir);
            dir_rm_file.append(dir);
            dir_found.append(dir);
        }
    }

    auto ext=QStringList{QByteArrayLiteral("*.*")};
    for(auto &sdir:dir_rm_file){
        QDir scanDir(sdir);
        if(!scanDir.exists())
            continue;
        scanDir.setFilter(QDir::Drives | QDir::Files);
        scanDir.setNameFilters(ext);
        for(auto &dirFile : scanDir.entryList()){
            auto fileName=sdir+QByteArrayLiteral("/")+dirFile;
            QFile::remove(fileName);
        }
    }
}

static void staticLogInitDir()
{
#ifdef QT_DEBUG
    staticLogRegister = true;
#else
    staticLogRegister = QVariant{QString{getenv(QByteArrayLiteral("Q_LOG_ENABLED"))}.trimmed()}.toBool();
#endif

    const auto log_local_name=QString{__PRETTY_FUNCTION__}.split(QStringLiteral("::")).first().replace(QStringLiteral("void "), "").split(QStringLiteral(" ")).last();
    *staticLogDir=(new QString("%1/%2/%3"))->arg(QDir::homePath(), log_local_name, qApp->applicationName());

    QDir dir(*staticLogDir);
    if(!dir.exists(*staticLogDir))
        dir.mkpath(*staticLogDir);

    if(dir.exists(*staticLogDir))
        staticLogDirClear(*staticLogDir);
}

Q_COREAPP_STARTUP_FUNCTION(staticLogInitDir);

QString &logDir()
{
    return *staticLogDir;
}

bool logRegister()
{
    return staticLogRegister;
}

QString logFile(const QString &extension, const QString &path)
{
    auto vExt=extension.trimmed();
    if(vExt.trimmed().isEmpty())
        vExt="log";

    auto currentName=QThread::currentThread()->objectName().trimmed();
    if(currentName.isEmpty())
        currentName=QString::number(qlonglong(QThread::currentThreadId()),16);

    if(logRegister()){
        static const auto __formatPath=logDir()+QStringLiteral("/%1");
        static const auto __formatFile=QStringLiteral("/%1/%2.%3");

        auto finalPath=__formatPath.arg(path);
        QDir dir(finalPath);
        if(!dir.exists())
            dir.mkpath(finalPath);

        return __formatFile.arg(finalPath, currentName, extension);
    }
    return {};
}

}
