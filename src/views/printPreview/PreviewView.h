#pragma once

#include <QAbstractScrollArea>
#include <QTextDocument>


class PreviewView : public QAbstractScrollArea
{
    Q_OBJECT

public:
    PreviewView(QTextDocument *document);

    inline void updateLayout() { resizeEvent(nullptr); viewport()->update(); }

public slots:
    void zoomIn();
    void zoomOut();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    void paintPage(QPainter *painter, int page);
    QTextDocument *doc;
    qreal scale;
    int interPageSpacing;
    QPoint mousePressPos;
    QPoint scrollBarValuesOnMousePress;
};

