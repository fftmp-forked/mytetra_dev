#pragma once

#include <QAbstractTableModel>
#include <QObject>
#include <QVariant>

#include "KnownBasesConfig.h"

// Если в конфиге прописаны относительные пути, то они высчитываются относительно директории, где лежит conf.ini.

/// @brief Модель для работы с возможными расположениями каталогов баз данных
class DatabasesManagementModel : public QAbstractTableModel {
    Q_OBJECT

    struct DatabasesDirsInfo {
        QString dbPath;
        QString descript;
        bool isCurrentConfigPath = false;
    };

  public:
    DatabasesManagementModel(QObject *parent);
    virtual ~DatabasesManagementModel(){};

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool isDbPathExists(const QString &path);
    void addDatabaseByUser(const QString &dbPath);
    void selectDatabase(const int &row);

  private:
    QVariant getCell(int row, int column, int role) const;

    void initData();

    void scanDirectoriesDirect();
    void scanDirectoriesFromConfig();
    void scanDirectoriesFromKnownbasesConfig();

    void scanDirectories(const QList<DatabasesDirsInfo> &dbDirs);

    void clearSelection();
    void selectDir(const QString &dbPath);

    bool isDbDirectory(const QString &path) const;

    QString getDirectoriesFromConfigFile(const QString &path);

    // На каждой строке хранится строковый набор данных
    // Столбцы содержат информацию согласно определениям DBMANAGEMENT_COLUMN_*
    QList<QStringList> mTableData;

    KnownBasesConfig mKnownBasesConfig;
};
