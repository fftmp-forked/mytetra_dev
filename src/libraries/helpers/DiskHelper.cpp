#include <QDir>
#include <QFileInfo>

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

/// @brief copy files from @param <from> dir to @param <to> dir. Skip directories, no deep copy.
bool DiskHelper::copyDirectory(const QString &from, const QString &to) {
    QDir fromDir(from);
    QDir toDir(to);

    if (!fromDir.exists() || !toDir.exists())
        return false;

    const auto dir_elems = fromDir.entryInfoList(QDir::Files);
    for(auto & info : dir_elems) {
        QFile::copy(info.absoluteFilePath(), to + "/" + info.fileName());
    }
    return true;
}

/// @brief Получение списка файлов с их содержимым в указанной директории
QMap<QString, QByteArray> DiskHelper::getFilesFromDirectory(QString dirName, QString fileMask) {
    QMap<QString, QByteArray> result;
    QDir directory(dirName);

    if (directory.exists()) {
        QStringList filter;
        filter << fileMask;

        const auto dir_elems = directory.entryInfoList(filter, QDir::Files);
        for(auto & info : dir_elems) {
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

    if (!directory.exists()) {
        qDebug() << "DiskHelper::saveFilesToDirectory() : Can not find directory" << dirName;
        return false;
    }

    for (auto it = fileList.keyBegin(); it != fileList.keyEnd(); ++it) {
        const auto & filename = *it;
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
}

/// @brief copy src file to dst file and set dst permissions to @param perm
static bool install_file(const QString & src_file, const QString & dst_file, QFileDevice::Permissions perm) {
    qDebug() << "copy " << src_file << " to " << dst_file;
    if (!QFile::copy(src_file, dst_file))
        return false;
    return QFile::setPermissions(dst_file, perm);
}


/// @brief copy content of @param src to @param dst recursively
/// @param src - file, directory or Qt resource.
/// @param dst - file or directory.
/// @param perm - permissions for files. Directory will take permissions from umask.
bool DiskHelper::install(const QString & src, const QString & dst, QFileDevice::Permissions perm) {
    QFileInfo src_info(src);
    if (src_info.isFile()) {
        QFileInfo dst_info(dst);
        return install_file(src, dst_info.isDir() ? dst + '/' + src_info.fileName() : dst, perm);
    } else {
        QDir src_dir(src);
        const auto dir_elems = src_dir.entryInfoList();
        for (auto & it : dir_elems) {
            bool rv;
            if(it.isDir()) {
                QDir().mkpath(dst + '/' + it.fileName());
                rv = install(it.filePath(), dst + '/' + it.fileName(), perm);
            } else
                rv = install_file(it.filePath(), dst + '/' + it.fileName(), perm);
            if(!rv)
                return false;
        }
        return true;
    }
}
