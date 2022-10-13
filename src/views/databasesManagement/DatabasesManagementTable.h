#pragma once

#include <QMenu>
#include <QTableView>
#include <QWidget>

class DatabasesManagementController;

/// @brief Отображение таблицы известных баз данных (только таблица)
class DatabasesManagementTable : public QTableView {
    Q_OBJECT

  public:
    DatabasesManagementTable(QWidget *parent = nullptr) { Q_UNUSED(parent); };
    virtual ~DatabasesManagementTable(){};

    void init();
    void setController(DatabasesManagementController *pController) { controller = pController; };

  signals:

    void tapAndHoldGestureFinished(const QPoint &pos);

  protected slots:

    void onCustomContextMenuRequested(const QPoint &pos);

  protected:
    DatabasesManagementController *controller;

    QMenu contextMenu;

    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    void assemblyContextMenu(void);
    void setupSignals(void);
};
