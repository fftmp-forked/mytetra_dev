#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QFlags>
#include <QLabel>
#include <QTextEdit>

/// @brief Виджет, похожий на QMessageBox, только позволяющий выделять и копировать текст
/// (т. к. в Windows невозможно скопировать текст, отображаемый в QMessageBox)
class ReduceMessageBox : public QDialog {
    Q_OBJECT

  public:
    ReduceMessageBox(QWidget *parent = nullptr);

    void setText(QString iText) { text.setText(iText); }
    void setDetailedText(QString iDetailedText) { detailedText.setText(iDetailedText); }
    void setDetailedTextReadOnly(bool iReadOnly) { detailedText.setReadOnly(iReadOnly); }
    void setStandardButtons(QFlags<QDialogButtonBox::StandardButton> buttons) { buttonBox.setStandardButtons(buttons); }

  protected:
    void setupSignals(void);
    void setupUI(void);
    void assembly(void);

    QLabel text;
    QTextEdit detailedText;
    QDialogButtonBox buttonBox;
};
