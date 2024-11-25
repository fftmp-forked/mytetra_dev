#include <stdio.h>

#include "EditorAbsTableCell.h"

#include "../helpers/DebugHelper.h"


void EditorAbsTableCell::clear() {
    htmlProperty.clear();
    value = QString();
    cellType = IS_NULL_CELL;
    refSuperCellX = 0;
    refSuperCellY = 0;
    superCellSizeIsModify = false;
}


/// @brief Вывод содержимого ячейки для отладки
void EditorAbsTableCell::print_cell() {
    char type = "ONSU"[cellType];
    auto val = value.simplified().trimmed().leftJustified(5, '.', true);

    printf("%c[%d,%d](%d,%d)%s ", type,
           htmlProperty["colspan"].toInt(),
           htmlProperty["rowspan"].toInt(),
           refSuperCellX,
           refSuperCellY,
           val.toUtf8().data()); // Ранее было toAscii, но в Qt 5.2 toAscii уже не поддерживается
}

void EditorAbsTableCell::set_supercell_size_is_modify(bool i) {
    if (cellType != IS_SUPER_CELL) {
        criticalError("Try set flag size_is_modify to not supercell");
        return;
    } else
        superCellSizeIsModify = i;
}

bool EditorAbsTableCell::get_supercell_size_is_modify() {
    if (cellType != IS_SUPER_CELL) {
        criticalError("Try get flag size_is_modify from not supercell");
        return false;
    } else
        return superCellSizeIsModify;
}
