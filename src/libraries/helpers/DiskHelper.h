#pragma once

#include <QByteArray>
#include <QFile>
#include <QFileDevice>
#include <QMap>
#include <QString>


namespace DiskHelper {
    QString createTempDirectory(void);
    bool copyDirectory(const QString &from, const QString &to);
    QMap<QString, QByteArray> getFilesFromDirectory(QString dirName, QString fileMask);
    bool saveFilesToDirectory(QString dirName, QMap<QString, QByteArray> fileList);
    bool install(const QString & src, const QString & dst, QFileDevice::Permissions perm = QFile::ReadUser | QFile::WriteUser);
}
