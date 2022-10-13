#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QWidget>
#include <QtGlobal>

class MultiLineInputDialog : public QDialog {
    Q_OBJECT

  public:
    MultiLineInputDialog(QWidget *parent = nullptr);
    virtual ~MultiLineInputDialog();

    void setText(QString text) { textArea->setPlainText(text); }
    QString getText() const { return textArea->toPlainText(); }
    bool isModified() const { return textArea->document()->isModified(); }
    void setWordWrapMode(QTextOption::WrapMode mode) { textArea->setWordWrapMode(mode); }
    void setSizeCoefficient(double f) {
        sizeCoefficient = f;
        updateSize();
    }

  public slots:

    void setupShortcuts(void);

  private:
    double sizeCoefficient;
    QTextEdit *textArea;
    QDialogButtonBox *buttonBox;

    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    void updateSize();
};
