#pragma once

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>

class EditorSplitCellForm : public QDialog {
    Q_OBJECT

  public:
    EditorSplitCellForm(int fixHorisontalSplit, int fixVerticalSplit);
    ~EditorSplitCellForm();

    int get_num_split_h(void);
    int get_num_split_v(void);

  private:
    int fixHSplit;
    int fixVSplit;

    QCheckBox *checkboxFixHSplit;
    QCheckBox *checkboxFixVSplit;

    QLabel *checkboxLabelFixHSplit;
    QLabel *checkboxLabelFixVSplit;

    QLabel *labelPictogramH;
    QLabel *labelPictogramV;

    QLabel *labelH;
    QLabel *labelV;

    QSpinBox *numSplitH;
    QSpinBox *numSplitV;

    QDialogButtonBox *buttonBox;
};
