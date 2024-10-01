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
#include "helpers/DiskHelper.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#endif


/// @brief Проверка ini-файла
static bool isMytetraIniConfig(QString fileName) {
    qDebug() << "Check config file " << fileName;

    if (!QFile::exists(fileName))
        return false;

    QSettings conf(fileName, QSettings::IniFormat);
    return conf.contains("version") && conf.value("programm").toString() == FixedParameters::appTextId;
}


bool GlobalParameters::init(QString config_dir) {
    cfg_dir = config_dir;
    QDir d(cfg_dir);
    if (!d.exists() || d.isEmpty()) {
        qDebug() << "Create first program files in directory " << cfg_dir;

        // Создаются файлы конфигурации
        DiskHelper::install(":/resource/standardconfig/", cfg_dir);
    }
    return isMytetraIniConfig(cfg_dir + "/conf.ini");
}
