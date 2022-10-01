#pragma once

#include <QWidget>
#include <QTreeView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>


class KnowTreeView : public QTreeView
{
 Q_OBJECT

public:
 explicit KnowTreeView(QWidget *parent = nullptr);
 virtual ~KnowTreeView();

signals:
 void tapAndHoldGestureFinished(const QPoint &);
 void dropEventHandleCatch();

protected:

 bool event(QEvent *event);

 void dragEnterEvent(QDragEnterEvent *event);
 void dragMoveEvent(QDragMoveEvent *event);
 void dropEvent(QDropEvent *event);

 template <class X> bool isDragableData(X *event);

};


