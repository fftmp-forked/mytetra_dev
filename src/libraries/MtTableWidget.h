#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QStyledItemDelegate>

// Этот класс не используется, скорее всего будет удален

// Класс, исправляющий QTableWidget, чтобы правильно применялись QSS-стили

class MtTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    MtTableWidget(QWidget *parent = nullptr);
    virtual ~MtTableWidget();

private:
    QStyledItemDelegate* itemDelegate;

};


