#pragma once

#include <QFontComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include "../../views/appConfigWindow/ConfigPage.h"

class EditorConfig;
class EditorToolbarSettingsScreen;

class EditorConfigToolbars : public ConfigPage {
    Q_OBJECT

  public:
    EditorConfigToolbars(QWidget *parent = nullptr);
    virtual ~EditorConfigToolbars(void) {}

    int applyChanges(void) { return 0; }

  private slots:

    // Запуск диалога распределения кнопок (команд) по панелям инструментов редактора
    void onClickedEditToolButtonsConfigFile(void);

  protected:
    QPushButton *editToolButtonsConfigFile;

    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

  private:
    EditorConfig *conf;

    QLabel *indentStepLabel;
    QLabel *indentStepFlexion;
    QSpinBox *indentStep;

    // Диалог настройки панелей инструментов
    EditorToolbarSettingsScreen *toolbuttonsScreen = nullptr;
};
