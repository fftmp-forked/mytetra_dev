#pragma once

#include <QAction>
#include <QDialog>


/// @brief Виджет отображения настроек каталогов известных баз данных
class DatabasesManagementScreen : public QDialog {
    Q_OBJECT

  public:
    DatabasesManagementScreen(QWidget *parent = nullptr);

    QAction *actionCopy;
};
