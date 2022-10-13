#pragma once

#include <QByteArray>
#include <QMap>
#include <QString>

class DiskHelper {
  public:
    DiskHelper(){};

    static QString createTempDirectory(void);
    static bool copyDirectory(const QString &fromName, const QString &toName);
    static QMap<QString, QByteArray> getFilesFromDirectory(QString dirName, QString fileMask);
    static bool saveFilesToDirectory(QString dirName, QMap<QString, QByteArray> fileList);
};
