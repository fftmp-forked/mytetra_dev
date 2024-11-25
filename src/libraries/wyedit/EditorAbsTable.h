#pragma once

#include <QMap>
#include <QObject>

#include "EditorAbsTableCell.h"


class EditorAbsTable : public QObject {
  public:
    EditorAbsTable(int x, int y, QObject *parent = nullptr) : QObject(parent), columns(x), rows(y) { create_cells_array(columns, rows); }
    EditorAbsTable(int x, int y, QString tableText, QObject *parent = nullptr) : EditorAbsTable(x, y, parent) { convert_table_to_internal(tableText); }
    ~EditorAbsTable() {}

    void set_table(int x, int y, QString tableText);

    int get_cell_colspan(int x, int y) { return get_cell_col_or_row_span(x, y, "colspan"); }
    int get_cell_rowspan(int x, int y) { return get_cell_col_or_row_span(x, y, "rowspan"); }

    void split_single_cell_by_horisontal(int x, int y, int div);
    void split_merged_cell_by_horisontal(int x, int y);

    void split_single_cell_by_vertical(int x1, int x2, int y, int div);
    void split_merged_cell_by_vertical(int x1, int x2, int y);

    QString get_table();

  private:
    void create_cells_array(int x, int y);
    void clear_table();
    void clear_supercell_size_is_modify();
    void convert_table_to_internal(QString tableText);
    void print_internal_table();
    int get_cell_col_or_row_span(int x, int y, QString propName);

    void insert_column(int insX);
    void insert_row(int insY);

    int columns;
    int rows;

    // Свойства таблицы, заполняются из свойств тега <table>
    QMap<QString, QString> htmlProperty;

    QVector<QVector<EditorAbsTableCell>> cells;
};
