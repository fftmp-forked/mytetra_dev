#pragma once

#include <QObject>
#include <QStatusBar>
#include "Singleton.h"


class FindScreen;

class GlobalParameters : public Singleton<GlobalParameters> {
    friend class Singleton<GlobalParameters>;
  public:
    ~GlobalParameters() { qInfo() << " GP DTOR"; };
    bool init(QString config_dir);

    QString get_cfg_dir() const { return cfg_dir; }

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
    GlobalParameters() {}

    FindScreen *pointFindScreen = nullptr;
    QStatusBar *pointStatusBar = nullptr;

    QString cfg_dir; // dir with used conf.ini
};
