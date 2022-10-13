#pragma once

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QTreeView>
#include <QWidget>

class KnowTreeView : public QTreeView {
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

    template <class X>
    bool isDragableData(X *event);
};
