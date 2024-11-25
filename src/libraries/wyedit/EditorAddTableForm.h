#pragma once

#include <QDialog>
#include <QSpinBox>

class QLabel;
class QDialogButtonBox;

class EditorAddTableForm : public QDialog {
    Q_OBJECT

  public:
    EditorAddTableForm();

    int get_columns() { return spinColumns->value(); }
    int get_rows() { return spinRows->value(); }
    int get_width() { return spinWidth->value(); }

  private:
    QLabel *labelColumns;
    QLabel *labelRows;
    QLabel *labelWidth;
    QLabel *labelPercent;

    QSpinBox *spinColumns;
    QSpinBox *spinRows;
    QSpinBox *spinWidth;

    QDialogButtonBox *buttonBox;
};
