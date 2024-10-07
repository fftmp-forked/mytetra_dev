#pragma once

#include <QMenu>
#include <QObject>
#include <QTableView>
#include <QWidget>

class AttachTableController;

/// @brief Отображение таблицы с файлами (только таблица)
class AttachTableView : public QTableView {
    Q_OBJECT

  public:
    AttachTableView(QWidget *parent = nullptr);
    ~AttachTableView() { qInfo() << "AttachTableView DTOR"; }
    void init(void);

    void setController(AttachTableController *pController) { controller = pController; }

    int getFirstSelectionPos(void);

  protected slots:

    void onCustomContextMenuRequested(const QPoint &pos);

  protected:
    void setupSignals(void);
    void assemblyContextMenu(void);

    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    AttachTableController *controller;

    QMenu *contextMenu;
};
