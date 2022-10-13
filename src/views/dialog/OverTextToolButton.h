#pragma once

#include <QPaintEvent>
#include <QToolButton>

// Виджет, похожий на QToolButton, но позволяющий писать поверх иконки

class OverTextToolButton : public QToolButton {
    Q_OBJECT

  public:
    OverTextToolButton(QWidget *parent = nullptr);

    void setOverText(QString iText);

  protected:
    void paintEvent(QPaintEvent *event);

    QString overText;
};
