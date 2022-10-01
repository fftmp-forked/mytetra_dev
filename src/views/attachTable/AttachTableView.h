#pragma once

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QMenu>

class AttachTableController;


/// @brief Отображение таблицы с файлами (только таблица)
class AttachTableView : public QTableView
{
  Q_OBJECT

public:
  AttachTableView(QWidget *parent=nullptr);
  virtual ~AttachTableView();

  void init(void);

  void setController(AttachTableController *pController) {controller=pController;}

  int getFirstSelectionPos(void);


signals:

  void tapAndHoldGestureFinished(const QPoint &);


protected slots:

  void onCustomContextMenuRequested(const QPoint &pos);


protected:

  void setupSignals(void);
  void assemblyContextMenu(void);

  bool event(QEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void paintEvent(QPaintEvent *event);

  AttachTableController *controller;

  QMenu *contextMenu;
};

