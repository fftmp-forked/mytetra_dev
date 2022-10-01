#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QAction>
#include <QToolBar>

class DatabasesManagementTable;
class DatabasesManagementController;
class QDialogButtonBox;


/// @brief Виджет отображения настроек каталогов известных баз данных
class DatabasesManagementScreen : public QDialog
{
  Q_OBJECT

public:
  DatabasesManagementScreen(QWidget *parent = nullptr);
  virtual ~DatabasesManagementScreen() {}

  // Действия, используемые как на тулбаре, так и в контекстном меню
  QAction *actionSelect;
  QAction *actionCreate;
  QAction *actionAdd;
  QAction *actionCopy;

protected:

  DatabasesManagementController *databasesManagementController;
  DatabasesManagementTable *databasesManagementTable;

  // Экранные элементы
  QToolBar *toolBar;
  QDialogButtonBox *buttonBox;
  QVBoxLayout *screenLayout;

  void setupUI(void);
  void setupSignals(void);
  void setupActions(void);
  void assembly(void);

};

