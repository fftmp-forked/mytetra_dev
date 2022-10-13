#pragma once

#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QPushButton>

class HotKeyGrabber : public QWidget {
    Q_OBJECT

  public:
    explicit HotKeyGrabber(QWidget *parent = nullptr);
    ~HotKeyGrabber();

    void clear();
    QKeySequence keySequence();

  signals:
    void editingFinished(); // Повторение сигнала QKeySequenceEdit::editingFinished

  protected:
    QKeySequenceEdit *keySequenceEdit;
    QPushButton *buttonCancel;

    QHBoxLayout *layout;

    void setupUI(void);
    void setupSignals(void);
    void assembly(void);
};
