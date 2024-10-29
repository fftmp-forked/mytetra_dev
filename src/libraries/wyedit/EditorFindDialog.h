#pragma once

#include <QDialog>
#include <QTextDocument>
#include <QWidget>

class QCheckBox;
class QLineEdit;
class QPushButton;

class EditorFindDialog : public QDialog {
    Q_OBJECT

  public:
    EditorFindDialog(QWidget *parent = nullptr);

  signals:
    void find_text(const QString &text, QTextDocument::FindFlags flags);

  private slots:
    void find_clicked();
    void enable_find_button(const QString &text);

  private:
    QLineEdit *lineEdit;
    QCheckBox *mathCase;
    QCheckBox *wholeWords;
    QCheckBox *searchBackward;
    QPushButton *findButton;

    void setup_ui();
    void setup_signals();
    void assembly();

    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
};
