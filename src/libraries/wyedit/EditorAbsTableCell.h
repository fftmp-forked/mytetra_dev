#pragma once

#include <QMap>

// Этот класс нельзя унаследовать от QObject, так как в этом случае
// при вставке ячейки в вектор потребуется конструктор копирования

class EditorAbsTableCell {

  public:
    EditorAbsTableCell() {  clear(); }
    EditorAbsTableCell(QString init_value) { clear(); value = init_value; }

    void set_value(QString init_value) { value = init_value; }
    QString get_value() const { return value; }

    void set_html_property(QString propName, QString propValue) { htmlProperty[propName] = propValue; }
    QString get_html_property(QString propName) const { return htmlProperty[propName]; }
    QMap<QString, QString> get_html_property_map() const { return htmlProperty; }

    void clear();

    enum CELL_TYPE {
        IS_NULL_CELL,
        IS_NORMAL_CELL,
        IS_SUPER_CELL,
        IS_SUB_CELL
    };

    void set_cell_type(EditorAbsTableCell::CELL_TYPE i) { cellType = i; }
    EditorAbsTableCell::CELL_TYPE get_type() const { return cellType; }

    void set_ref_super_cell_xy(int x, int y) { refSuperCellX = x; refSuperCellY = y; }
    void set_ref_super_cell_x(int x) { refSuperCellX = x; }
    void set_ref_super_cell_y(int y) { refSuperCellY = y; }
    int get_ref_super_cell_x() const { return refSuperCellX; }
    int get_ref_super_cell_y() const { return refSuperCellY; }

    void set_supercell_size_is_modify(bool i);
    bool get_supercell_size_is_modify();

    void print_cell();

  private:
    // Свойства ячейки, заполняются из свойств тега <td>
    QMap<QString, QString> htmlProperty;

    // Содержимое ячейки
    QString value;

    // Тип ячейки
    CELL_TYPE cellType;

    // Координаты ведущей ячейки, если ячейка является подчиненной (isSubCell)
    int refSuperCellX;
    int refSuperCellY;

    // Свойство, используемое только в суперячейке
    bool superCellSizeIsModify;
};
