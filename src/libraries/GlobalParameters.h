#pragma once

#include <QObject>
#include <QStatusBar>

class FindScreen;

/// @brief singleton class
class GlobalParameters {
  public:
    GlobalParameters() = delete;
    GlobalParameters(const GlobalParameters &) = delete;
    GlobalParameters &operator=(const GlobalParameters &) = delete;
    GlobalParameters(GlobalParameters &&) = delete;
    GlobalParameters &operator=(GlobalParameters &&) = delete;
    ~GlobalParameters(){};

    static void init(QString filename) { _self = new GlobalParameters(filename); }
    static GlobalParameters &get() { return *_self; }

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

  private:
    explicit GlobalParameters(QString filename);
    static GlobalParameters *_self;

    void initWorkDirectory(void);
    bool findWorkDirectory(void);
    bool isMytetraIniConfig(QString fileName);
    void createStandartProgramFiles(void);
    void createPortableProgramFiles(void);
    void createFirstProgramFiles(QString dirName);

    FindScreen *pointFindScreen = nullptr;
    QStatusBar *pointStatusBar = nullptr;

    QString mainProgramFile;
    QString workDirectory;
};
