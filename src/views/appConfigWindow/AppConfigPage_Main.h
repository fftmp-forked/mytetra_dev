#pragma once

#include <QComboBox>
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QToolButton>

#include "ConfigPage.h"


class AppConfigPage_Main : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Main(QWidget *parent = nullptr);
  ~AppConfigPage_Main(void);

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);

  int applyChanges(void);
    
private slots:
  void onClickedTetradirSelectDialog(void);

  void onDisableCustomDateTimeFormatToggle(bool checked);
  void onEnableCustomDateTimeFormatToggle(bool checked);
  void onDateTimeFormatHelpButton(void);

private:

  QLabel *tetradirLabel;
  QLineEdit *tetradirInput;
  QToolButton *tetradirButton;

  QLabel *interfaceLanguageLabel;
  QComboBox *interfaceLanguage;

  // Настройки отображения даты и времени
  QGroupBox *dateTimeFormatBox;
  QRadioButton *disableCustomDateTimeFormat;
  QRadioButton *enableCustomDateTimeFormat;
  QLineEdit *customDateTimeFormat;
  QToolButton *dateTimeFormatHelpButton;
};



