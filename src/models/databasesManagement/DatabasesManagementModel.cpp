#include <QCommonStyle>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "../../libraries/FixedParameters.h"
#include "../../libraries/GlobalParameters.h"
#include "../appConfig/AppConfig.h"
#include "DatabasesManagementModel.h"

#define DBMANAGEMENT_COLUMNS 3
#define DBMANAGEMENT_COLUMN_SELECT 0   // Признак выбора базы
#define DBMANAGEMENT_COLUMN_DBPATH 1   // Путь к БД
#define DBMANAGEMENT_COLUMN_DESCRIPT 2 // Человекочитаемое описание базы

#define DBMANAGEMENT_LINE_SELECT_FLAG "1" // Значение, свидетельствующее о выборе базы

#define DBMANAGEMENT_DEFAULT_DESCRIPT "Custom database directory"

DatabasesManagementModel::DatabasesManagementModel(QObject *parent) : QAbstractTableModel(parent) {
    mKnownBasesConfig.init();
    this->initData();
}

void DatabasesManagementModel::initData() {
    mTableData.clear();

    this->scanDirectoriesDirect();
    this->scanDirectoriesFromConfig();
    this->scanDirectoriesFromKnownbasesConfig();

    this->selectDir(AppConfig::get().get_tetradir());
}

/// @brief Поиск возможных каталогов баз данных напрямую в известных местах
void DatabasesManagementModel::scanDirectoriesDirect() {
    QList<DatabasesDirsInfo> dbDirs;
    DatabasesDirsInfo dbDirsInfo;
    QString workingPath;
    QStringList cand = {QDir::homePath() + "/.config/" + FixedParameters::appTextId};
    for (const auto &p : cand) {
        dbDirsInfo.dbPath = p + "/data";
        dbDirsInfo.descript = tr("Knowledge base in %1").arg(workingPath);
        dbDirsInfo.isCurrentConfigPath = false;
        dbDirs << dbDirsInfo;
    }
    this->scanDirectories(dbDirs);
}

void DatabasesManagementModel::scanDirectoriesFromConfig() {
    // Получить директории БД из возможных файлов conf.ini

    QList<DatabasesDirsInfo> dbDirs;
    DatabasesDirsInfo dbDirsInfo;

    // Данные из текущего конфига
    dbDirsInfo.dbPath = AppConfig::get().get_tetradir();
    dbDirsInfo.descript = tr("Knowledge base from current config file %1").arg(AppConfig::get().getConfigFileName());
    dbDirsInfo.isCurrentConfigPath = true;
    dbDirs << dbDirsInfo;

    // Данные из возможного конфига в директории ~/.config/имяПрограммы
    auto configFileName = QDir::homePath() + "/.config/" + FixedParameters::appTextId + "/conf.ini";
    dbDirsInfo.dbPath = this->getDirectoriesFromConfigFile(configFileName);
    dbDirsInfo.descript = tr("Knowledge base from config file in user directory %1").arg(dbDirsInfo.dbPath);
    dbDirsInfo.isCurrentConfigPath = false;
    dbDirs << dbDirsInfo;

    this->scanDirectories(dbDirs);
}

QString DatabasesManagementModel::getDirectoriesFromConfigFile(const QString &path) {
    QFile confFile(path);

    if (confFile.exists()) {
        QSettings conf(path, QSettings::IniFormat);

        if (conf.contains("tetradir"))
            return conf.value("tetradir").toString();
    }
    return "";
}

void DatabasesManagementModel::scanDirectoriesFromKnownbasesConfig() {
    // Получить директориюБД из файла knownbases.ini в рабочей директории
    for (int i = 0; i < mKnownBasesConfig.getDbCount(); ++i) {
        QString dbPath = mKnownBasesConfig.getDbParameter(i, "dbPath");
        QString descript = tr(DBMANAGEMENT_DEFAULT_DESCRIPT);

        QStringList tableLine;
        tableLine << "" << dbPath << descript;

        mTableData << tableLine;
    }
}

void DatabasesManagementModel::scanDirectories(const QList<DatabasesDirsInfo> &dbDirs) {
    for (const auto &currentDbDirs : dbDirs) {
        QString dbPath = currentDbDirs.dbPath;
        QString descript = currentDbDirs.descript;
        bool isCurrentConfigPath = currentDbDirs.isCurrentConfigPath;

        if (dbPath == "")
            continue; // Если встречены пустые пути, такие данные добавлять и обрабатывать нельзя

        // Получение абсолютных путей
        QString absoluteDbPath = QDir(dbPath).absolutePath();

        // Если директории БД и корзины действительно являются таковыми директориями
        if (this->isDbDirectory(dbPath)) {
            // Проверка что таких директорий еще нет в списке возможных директорий
            bool isExists = false;
            for (auto &tableDataLine : mTableData) // Выводимый auto-тип в виде ссылки, чтобы элементы можно было изменять
            {
                // В списке возможных директорий могут быть как относительные, так и абсолютные пути
                // И для корректного сравнения нужны абсолютные
                QString lineAbsoluteDbPath = QDir(tableDataLine[DBMANAGEMENT_COLUMN_DBPATH]).absolutePath();

                if (lineAbsoluteDbPath == absoluteDbPath) {
                    isExists = true;

                    // Пути текущего проверяемого элемента исправляются на пути в явном виде,
                    // если они совпадают с прописанными в конфиге путями, так как форма записи путей
                    // в конфиге приоритетнее, чем просто абсолютные пути
                    if (isCurrentConfigPath)
                        tableDataLine[DBMANAGEMENT_COLUMN_DBPATH] = dbPath;
                }
            }

            if (!isExists) {
                QStringList tableLine;

                if (isCurrentConfigPath)
                    tableLine << "" << dbPath << descript; // Путь из текущего конфига добавляется как есть, так как он может быть относительным
                else
                    tableLine << "" << absoluteDbPath << descript;

                mTableData << tableLine; // Директории добавляются в список
            }
        }
    }
}

void DatabasesManagementModel::clearSelection() {
    for (auto &tableDataLine : mTableData)
        tableDataLine[DBMANAGEMENT_COLUMN_SELECT] = "";
}

/// @brief Выставление пометки что директория базы выбрана в качестве рабочей
void DatabasesManagementModel::selectDir(const QString &dbPath) {
    this->clearSelection();

    for (auto &tableDataLine : mTableData) {
        if (tableDataLine[DBMANAGEMENT_COLUMN_DBPATH] == dbPath) {
            tableDataLine[DBMANAGEMENT_COLUMN_SELECT] = DBMANAGEMENT_LINE_SELECT_FLAG;
            return;
        }
    }
}

/// @brief Проверка, что каталог является каталогом с базой данных
bool DatabasesManagementModel::isDbDirectory(const QString &path) const {
    // Формальными признаками каталога с БД являются:
    // - Существование в нем файла mytetra.xml
    // - Существование в нем подкаталога /base

    if (!QFileInfo(path + "/mytetra.xml").isFile() || !QFileInfo(path + "/base").isDir())
        return false;

    return true;
}

// Число столбцов
int DatabasesManagementModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return DBMANAGEMENT_COLUMNS;
}

// Число строк
int DatabasesManagementModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return mTableData.size();
}

// Получение данных
QVariant DatabasesManagementModel::data(const QModelIndex &index, int role) const {
    if (index.isValid() && (role == Qt::DisplayRole || role == Qt::DecorationRole))
        return getCell(index.row(), index.column(), role);

    return QVariant();
}

// Получение значения ячейки, защищенный метод
QVariant DatabasesManagementModel::getCell(int row, int column, int role) const {
    Q_UNUSED(row)

    switch (column) {
    case DBMANAGEMENT_COLUMN_SELECT:

        if (role == Qt::DisplayRole) {
            if (mTableData[row][DBMANAGEMENT_COLUMN_SELECT] == DBMANAGEMENT_LINE_SELECT_FLAG) {
                return QVariant(tr("Selected"));
            } else {
                return QVariant(QString());
            }
        }

        // Вывод иконок
        if (role == Qt::DecorationRole) {
            if (mTableData[row][DBMANAGEMENT_COLUMN_SELECT] == DBMANAGEMENT_LINE_SELECT_FLAG) {
                return QCommonStyle().standardIcon(QStyle::SP_DialogApplyButton);
            }
        }

        break;

    case DBMANAGEMENT_COLUMN_DBPATH:

        if (role == Qt::DisplayRole) {
            return QVariant(mTableData[row][DBMANAGEMENT_COLUMN_DBPATH]);
        }

        break;

    case DBMANAGEMENT_COLUMN_DESCRIPT:

        if (role == Qt::DisplayRole) {
            return QVariant(mTableData[row][DBMANAGEMENT_COLUMN_DESCRIPT]);
        }

        break;
    }

    return QVariant();
}

QVariant DatabasesManagementModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical)
        return QVariant(section + 1);
    else
        switch (section) {
        case DBMANAGEMENT_COLUMN_SELECT:
            return QVariant(tr("Select"));

        case DBMANAGEMENT_COLUMN_DBPATH:
            return QVariant(tr("Detabase path"));

        case DBMANAGEMENT_COLUMN_DESCRIPT:
            return QVariant(tr("Description"));

        default:
            return QVariant();
        }
}

bool DatabasesManagementModel::isDbPathExists(const QString &path) {
    for (auto &tableDataLine : mTableData) {
        if (QDir(tableDataLine[DBMANAGEMENT_COLUMN_DBPATH]).absolutePath() == QDir(path).absolutePath()) {
            return true;
        }
    }

    return false;
}

void DatabasesManagementModel::addDatabaseByUser(const QString &dbPath) {
    QStringList line;
    line << "" << dbPath << tr(DBMANAGEMENT_DEFAULT_DESCRIPT);

    this->beginResetModel();
    mTableData << line;
    this->endResetModel();

    if (!mKnownBasesConfig.isDbParameterExists("dbPath", dbPath)) {
        int n = mKnownBasesConfig.getDbCount();
        mKnownBasesConfig.setDbParameter(n, "dbPath", dbPath);
    }
}

void DatabasesManagementModel::selectDatabase(const int &row) {
    this->clearSelection();

    QStringList line = mTableData[row];
    line[DBMANAGEMENT_COLUMN_SELECT] = DBMANAGEMENT_LINE_SELECT_FLAG;

    this->beginResetModel();
    mTableData[row] = line;
    this->endResetModel();
}
