#pragma once

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include "ConfigPage.h"

class AppConfigPage_Synchro : public ConfigPage {
    Q_OBJECT

  public:
    AppConfigPage_Synchro(QWidget *parent = nullptr);

    int applyChanges(void);

  private slots:

    void onEnablePeriodicCheckBase(bool);
    void onEnablePeriodicSynchro(bool);

  protected:
    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    // Виджеты настройки синхронизации
    QLineEdit *synchroCommand;
    QLabel *commandText;
    QLabel *commandAboutText;

    QCheckBox *synchroOnStartup;
    QCheckBox *synchroOnExit;
    QCheckBox *synchroOnPeriodic;
    QVBoxLayout *synchroOnLayout;
    QGroupBox *synchroOnBox;

    QLabel *synchroPeriodText;
    QSpinBox *synchroPeriod;
    QLabel *synchroPeriodPostfix;
    QHBoxLayout *synchroPeriodLayout;

    // Виджеты настройки периодической проверки
    QCheckBox *enablePeriodicCheckBase;

    QLabel *checkBasePeriodText;
    QSpinBox *checkBasePeriod;
    QLabel *checkBasePeriodPostfix;
    QHBoxLayout *checkBasePeriodLayout;

    QCheckBox *enablePeriodicCheckMessage;
    QVBoxLayout *periodicCheckLayout;
    QGroupBox *periodicCheckBox;

    QVBoxLayout *centralLayout;
};
