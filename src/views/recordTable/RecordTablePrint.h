#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QtGlobal>

class RecordTableProxyModel;

class RecordTablePrint : public QDialog {
    Q_OBJECT

  public:
    RecordTablePrint(QWidget *parent = nullptr);
    virtual ~RecordTablePrint();

    void setModel(RecordTableProxyModel *iModel);
    void generateHtmlTableFromModel(void);
    void setTitleToHtml(QString title);

  protected slots:

    void print(void);
    void save(void);

  private:
    QTextEdit *textArea;         // Отображение текста с таблицей, выводимой на печать
    QDialogButtonBox *buttonBox; // Линейка с кнопками

    QPushButton *printButton;
    QPushButton *saveButton;
    QPushButton *cancelButton;

    RecordTableProxyModel *model;

    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);
};
