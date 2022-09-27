#pragma once

#include <QStyledItemDelegate>

// Этот класс не используется, скорее всего будет удален

class MtStyledItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT
public:
 explicit MtStyledItemDelegate(QObject *parent = nullptr);

 QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

};

