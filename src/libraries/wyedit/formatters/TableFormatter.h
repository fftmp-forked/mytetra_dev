#pragma once

#include "Formatter.h"

/// @brief Класс форматирования таблиц в тексте
class TableFormatter : public Formatter {
    Q_OBJECT

  public:
    TableFormatter() {};

  public slots:

    void onCreatetableClicked();
    void onTableRemoveRowClicked();
    void onTableRemoveColClicked();
    void onTableAddRowClicked();
    void onTableAddColClicked();
    void onTableMergeCellsClicked();
    void onTableSplitCellClicked();
    void onTablePropertiesClicked();
};
