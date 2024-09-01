#pragma once

#include <QObject>
#include <QStatusBar>
#include "Singleton.h"


class FindScreen;

class GlobalParameters : public Singleton<GlobalParameters> {
    friend class Singleton<GlobalParameters>;
  public:
    void init(QString filename);

    QString getMainProgramFile(void) const { return mainProgramFile; }

    // Получение рабочей директории. Рабочая директория - это та, где лежит файл conf.ini
    QString getWorkDirectory(void) const { return workDirectory; }

    void setFindScreen(FindScreen *point) { pointFindScreen = point; }
    FindScreen *getFindScreen() const { return pointFindScreen; }

    void setStatusBar(QStatusBar *point) { pointStatusBar = point; }
    QStatusBar *getStatusBar() const { return pointStatusBar; }

    /// @brief Возможные режимы добавления записей в таблицу конечных записей
    enum AddNewRecordBehavior {
        ADD_TO_END = 0,
        ADD_BEFORE,
        ADD_AFTER
    };
    static void createStandartProgramFiles(void);

  private:
    GlobalParameters() {}

    void initWorkDirectory(void);
    bool findWorkDirectory(void);
    bool isMytetraIniConfig(QString fileName);
    static void createFirstProgramFiles(QString dirName);

    FindScreen *pointFindScreen = nullptr;
    QStatusBar *pointStatusBar = nullptr;

    QString mainProgramFile;
    QString workDirectory;
};
