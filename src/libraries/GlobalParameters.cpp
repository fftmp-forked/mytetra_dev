#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QStringList>

#include "GlobalParameters.h"

#include "FixedParameters.h"
#include "helpers/CssHelper.h"
#include "helpers/DebugHelper.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#endif


void GlobalParameters::init(QString filename) {
    mainProgramFile = filename;
    pointFindScreen = nullptr;
    pointStatusBar = nullptr;

    // После установки кодеков можно показать имя бинарника, и оно должно отобразиться правильно
    // даже если путь содержит каталог с национальными символами
    qDebug() << "Set main program file to " << mainProgramFile;

    initWorkDirectory(); // Инициализация рабочей директории
}

/// @brief Инициализация рабочей директории.
/// Если рабочая директория уже существует, она будет установлена как рабочая.
/// Иначе будет создана новая рабочая директория с начальными файлами и она будет установлена как рабочая
void GlobalParameters::initWorkDirectory(void) {
    // Если рабочая директория найдена автоматически
    if (findWorkDirectory())
        return;
    /// @todo: return portable mode
}

void GlobalParameters::createStandartProgramFiles(void) {
    qDebug() << "Create standard program files";

    QDir userDir = QDir::home();
    QString dataDirName = ".config/" + FixedParameters::appTextId;
    if (userDir.mkpath(dataDirName)) {
        qDebug() << "Successfull create subdirectory " << dataDirName << " in directory " << userDir.absolutePath();

        QString createFilePath = userDir.absolutePath() + "/" + dataDirName; // Ранее использовался QDir::homePath()

        createFirstProgramFiles(createFilePath);
    } else {
        criticalError("Can not create directory \"" + dataDirName + "\" in user directory \"" + QDir::homePath() + "\"");
    }
}

/// @brief Создание первоначального набора файлов в указанной директории
void GlobalParameters::createFirstProgramFiles(QString dirName) {
    qDebug() << "Create first program files in directory " << dirName;

    QDir dir(dirName);

    // Создается дерево директорий в указанной директории
    dir.mkpath("data/base/1300000000aaaaaaaaa2");

    // Создаются файлы конфигурации

    QFile::copy(":/standardconfig/conf.ini", dirName + "/conf.ini");
    QFile::setPermissions(dirName + "/conf.ini", QFile::ReadUser | QFile::WriteUser);

    QFile::copy(":/standardconfig/editorconf.ini", dirName + "/editorconf.ini");
    QFile::setPermissions(dirName + "/editorconf.ini", QFile::ReadUser | QFile::WriteUser);

    CssHelper::createStyleSheetFile(dirName);

    // Создается файл базы данных
    QFile::copy(":/resource/standartdata/mytetra.xml", dirName + "/data/mytetra.xml");
    QFile::setPermissions(dirName + "/data/mytetra.xml", QFile::ReadUser | QFile::WriteUser);

    // Создается файл первой записи
    QFile::copy(":/resource/standartdata/base/1300000000aaaaaaaaa2/text.html", dirName + "/data/base/1300000000aaaaaaaaa2/text.html");
    QFile::setPermissions(dirName + "/data/base/1300000000aaaaaaaaa2/text.html", QFile::ReadUser | QFile::WriteUser);

// Синхронизация файловой системы, почему-то после создания файлы
// не всегда доступны на Linux. Под windows такой утилиты нет в стандартной поставке
#ifdef Q_OS_LINUX
    sync();
#endif
}

/// @brief Автоопределение рабочей директории
bool GlobalParameters::findWorkDirectory(void) {
    // Поиск файла conf.ini в той же директории, где находится бинарник

    // Нужно учесть, что программу могут запускать из другой директории
    // QDir::currentPath() - выдает директорию, где была выполнена команда запуска
    // mainProgramFile - содержит путь к бинарнику относительно директории запуска

    // Директория, где была выполнена команда запуска
    auto fullCurrentPath = QFileInfo(mainProgramFile).absolutePath();

    qDebug() << "Check full current path " << fullCurrentPath;

    if (isMytetraIniConfig(fullCurrentPath + "/conf.ini")) {
        qDebug() << "Work directory set to path " << fullCurrentPath;

        workDirectory = fullCurrentPath;
    } else {
        // Если в текущей директории запуска нет conf.ini

        // поиск conf.ini в "~/.config/имя_программы"
        auto dir = QDir::homePath() + "/.config/" + FixedParameters::appTextId;

        if (isMytetraIniConfig(dir + "/conf.ini")) {
            qDebug() << "Config init file success find in " << dir;
            workDirectory = dir;
        } else
            qDebug() << "Сan't' find conf.ini in " << dir;
    }

    if (workDirectory.length() == 0) {
        qDebug() << "Can't find work directory with mytetra data";
        return false;
    } else {
        qDebug() << "Set work directory to " << workDirectory;

        // Устанавливается эта директория как рабочая
        if (QDir::setCurrent(workDirectory))
            return true;
        else
            criticalError("Can not set work directory as '" + workDirectory + "'. System problem.");
    }
}

/// @brief Проверка ini-файла
bool GlobalParameters::isMytetraIniConfig(QString fileName) {
    qDebug() << "Check config file " << fileName;

    QFileInfo info(fileName);

    if (!info.exists())
        return false;

    // Выясняется имя файла без пути к директории
    auto shortFileName = info.fileName();
    qDebug() << "Short config file name " << shortFileName;

    // Выясняется имя директории из имени файла
    auto dirName = info.dir().absolutePath();
    qDebug() << "Config directory name " << dirName;

    // Открывается хранилище настроек
    QScopedPointer<QSettings> conf(new QSettings(fileName, QSettings::IniFormat));

    if (!conf->contains("version") || !conf->contains("programm"))
        return false;
    return conf->value("programm").toString() == FixedParameters::appTextId;
}
