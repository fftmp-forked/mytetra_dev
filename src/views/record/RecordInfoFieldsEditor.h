#pragma once

#include <QDialog>
#include <QDialogButtonBox>

class InfoFieldEnter;

/// @brief Окно редактирования инфополей записи (не текста записи!).
/// Оно появляется при двойном клике на записи или при клике на кнопку редактирования полей записи
class RecordInfoFieldsEditor : public QDialog {
    Q_OBJECT

  public:
    RecordInfoFieldsEditor(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    ~RecordInfoFieldsEditor() {}

    QString getField(QString name);
    void setField(QString name, QString value);

    void setReadOnly(bool state);
    bool isReadOnly();

  public slots:

    void setupShortcuts(void);

  private slots:

    void okClick(void);

  private:
    // Виджет ввода инфополей записи
    InfoFieldEnter *infoField;

    QDialogButtonBox *buttonBox;

    void setupUI(void);
    void setupSignals(void);
    void assembly(void);
};
