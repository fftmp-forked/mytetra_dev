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
    virtual ~KnowTreeView() {}

  signals:
    void dropEventHandleCatch();

  protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    template <class X>
    bool isDragableData(X *event);
};
