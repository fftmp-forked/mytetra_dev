#include <QDir>

#include "../FixedParameters.h"
#include "DebugHelper.h"
#include "DiskHelper.h"
#include "UniqueIdHelper.h"

/// @brief Создание временной директории
QString DiskHelper::createTempDirectory(void) {
    QDir dir;
    QString systemTempDirName = dir.tempPath();

    QString temp_dir_name = FixedParameters::appTextId + getUniqueId();

    // Создается директория
    dir.setPath(systemTempDirName);
    dir.mkdir(temp_dir_name);

    QString createTempDirName = systemTempDirName + "/" + temp_dir_name;

    qDebug() << "Create temporary directory " + createTempDirName;

    return createTempDirName;
}

// Копирование содержимого директории
// Копируются только файлы
bool DiskHelper::copyDirectory(const QString &fromName, const QString &toName) {
    QDir fromDir(fromName);
    QDir toDir(toName);

    if (fromDir.exists() && toDir.exists()) {
        Q_FOREACH (QFileInfo info, fromDir.entryInfoList(QDir::Files)) {
            QFile::copy(info.absoluteFilePath(), toName + "/" + info.fileName());
        }

        return true;
    }

    return false;
}

/// @brief Получение списка файлов с их содержимым в указанной директории
QMap<QString, QByteArray> DiskHelper::getFilesFromDirectory(QString dirName, QString fileMask) {
    QMap<QString, QByteArray> result;
    QDir directory(dirName);

    if (directory.exists()) {
        QStringList filter;
        filter << fileMask;

        foreach (QFileInfo info, directory.entryInfoList(filter, QDir::Files)) {
            QFile f(info.absoluteFilePath());
            if (!f.open(QIODevice::ReadOnly))
                criticalError("DiskHelper::getFilesFromDirectory() : File '" + info.absoluteFilePath() + "' open error");

            // Содержимое файла
            QByteArray b = f.readAll();

            // Содержимое файла сохраняется с ключем в виде имени файла
            result.insert(info.fileName(), b);
        }
    } else
        qDebug() << "DiskHelper::getFilesFromDirectory() : Can not find directory" << dirName;

    return result;
}

// Запись в директорию файлов, переданных в виде ассоциативного массива fileList
bool DiskHelper::saveFilesToDirectory(QString dirName, QMap<QString, QByteArray> fileList) {
    qDebug() << "DiskHelper::saveFilesToDirectory() : Directory name " << dirName;

    QDir directory(dirName);

    // Если директория существует
    if (directory.exists()) {
        foreach (QString filename, fileList.keys()) {
            qDebug() << "DiskHelper::saveFilesToDirectory() : Save file " << filename;

            QFile file(dirName + "/" + filename);

            // Файл открывается для записи
            if (!file.open(QIODevice::WriteOnly)) {
                qDebug() << "DiskHelper::saveFilesToDirectory() : Can not save file '" << filename << "' to directory '" << dirName << "'";
                return false;
            }

            // Данные сохраняются в файл
            file.write(fileList.value(filename));
        }

        return true;
    } else {
        qDebug() << "DiskHelper::saveFilesToDirectory() : Can not find directory" << dirName;
        return false;
    }
}
