#include <QSortFilterProxyModel>
#include <QDebug>

#include "RecordTableProxyModel.h"


bool RecordTableProxyModel::removeRow(int row, const QModelIndex &parent) {
    qDebug() << Q_FUNC_INFO << " row: " << row;

    return QSortFilterProxyModel::removeRow(row, parent);
}

