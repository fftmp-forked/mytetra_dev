#include <QWidget>
#include <QBoxLayout>
#include <QLabel>

#include "AppConfigPage_Synchro.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/PeriodicCheckBase.h"
#include "libraries/PeriodicSynchro.h"
#include "libraries/helpers/MessageHelper.h"

extern PeriodicCheckBase periodicCheckBase;
extern PeriodicSynchro periodicSynchro;


AppConfigPage_Synchro::AppConfigPage_Synchro(QWidget *parent) : ConfigPage(parent) {
    qDebug() << "Create sync config page";

    setupUi();
    setupSignals();
    assembly();
}


void AppConfigPage_Synchro::setupUi(void) {
    synchroCommand=new QLineEdit(this);
    synchroCommand->setText(AppConfig::get().get_synchrocommand());
    synchroCommand->setCursorPosition(0);

    synchroOnStartup=new QCheckBox(this);
    synchroOnStartup->setText(tr("Synchronize at MyTetra startup"));
    synchroOnStartup->setChecked(AppConfig::get().get_synchroonstartup());

    synchroOnExit=new QCheckBox(this);
    synchroOnExit->setText(tr("Synchronize when exit from MyTetra"));
    synchroOnExit->setChecked(AppConfig::get().get_synchroonexit());

    synchroOnPeriodic=new QCheckBox(this);
    synchroOnPeriodic->setText(tr("Enable periodic background synchronize"));
    synchroOnPeriodic->setChecked(AppConfig::get().getEnablePeriodicSynchro());

    // Область ввода команды синхронизации
    commandText=new QLabel(this);
    commandText->setText(tr("Synchronization command"));

    commandAboutText=new QLabel(this);
    commandAboutText->setText(tr("Use <b>%a</b> macro for get database directory path"));
    commandAboutText->setWordWrap(true);

    // Группировщик виджетов для настройки автоматического старта синхронизации
    synchroOnBox=new QGroupBox(this);
    synchroOnBox->setTitle(tr("Automatic start synchronization"));

    // Виджеты автоматического старта синхронизации вставляются в группировщик
    synchroOnLayout=new QVBoxLayout();
    synchroOnLayout->addWidget( synchroOnStartup );
    synchroOnLayout->addWidget( synchroOnExit );
    synchroOnLayout->addWidget( synchroOnPeriodic );

    synchroPeriodText=new QLabel(this);
    synchroPeriodText->setText( tr("Synchronization period: ") );

    synchroPeriod=new QSpinBox(this);
    synchroPeriod->setMaximum( 100000 ); // Максимальная граница должна устанавливаться перед заданием значения
    synchroPeriod->setValue(AppConfig::get().getPeriodicSynchroPeriod() );

    synchroPeriodPostfix=new QLabel(this);
    synchroPeriodPostfix->setText( tr("sec.") );

    synchroPeriodLayout=new QHBoxLayout();
    synchroPeriodLayout->addWidget( synchroPeriodText );
    synchroPeriodLayout->addWidget( synchroPeriod );
    synchroPeriodLayout->addWidget( synchroPeriodPostfix );
    synchroPeriodLayout->addStretch();
    synchroOnLayout->addLayout( synchroPeriodLayout );

    synchroOnBox->setLayout( synchroOnLayout );

    // Начальное состояние зависимых параметров (активны-неактивны) для периодической синхронизации
    onEnablePeriodicSynchro(AppConfig::get().getEnablePeriodicSynchro() );


    // Виджеты настройки периодической проверки базы на предмет изменений
    enablePeriodicCheckBase=new QCheckBox(this);
    enablePeriodicCheckBase->setText( tr("Periodic check database tree for change at 3rd-party app") ); // Периодически проверять дерево базы на предмет изменения сторонней программой
    enablePeriodicCheckBase->setChecked(AppConfig::get().getEnablePeriodicCheckBase() );

    checkBasePeriodText=new QLabel(this);
    checkBasePeriodText->setText( tr("Checking period: ") );

    checkBasePeriod=new QSpinBox(this);
    checkBasePeriod->setMaximum( 100000 ); // Максимальная граница должна устанавливаться перед заданием значения
    checkBasePeriod->setValue(AppConfig::get().getCheckBasePeriod() );

    checkBasePeriodPostfix=new QLabel(this);
    checkBasePeriodPostfix->setText( tr("sec.") );

    checkBasePeriodLayout=new QHBoxLayout();
    checkBasePeriodLayout->addWidget( checkBasePeriodText );
    checkBasePeriodLayout->addWidget( checkBasePeriod );
    checkBasePeriodLayout->addWidget( checkBasePeriodPostfix );
    checkBasePeriodLayout->addStretch();

    enablePeriodicCheckMessage=new QCheckBox(this);
    enablePeriodicCheckMessage->setText( tr("Show message if a database tree was changed by external app") );
    enablePeriodicCheckMessage->setChecked(AppConfig::get().getEnablePeriodicCheckMessage() );

    periodicCheckLayout=new QVBoxLayout();
    periodicCheckLayout->addWidget( enablePeriodicCheckBase);
    periodicCheckLayout->addLayout( checkBasePeriodLayout);
    periodicCheckLayout->addWidget( enablePeriodicCheckMessage);

    periodicCheckBox=new QGroupBox(this);
    periodicCheckBox->setTitle( tr("Periodic checking database tree") );
    periodicCheckBox->setLayout( periodicCheckLayout );

    // Начальное состояние зависимых параметров (активны-неактивны) выставляются в зависимости от галки
    onEnablePeriodicCheckBase(AppConfig::get().getEnablePeriodicCheckBase() );
}


void AppConfigPage_Synchro::assembly(void) {
    centralLayout=new QVBoxLayout();

    centralLayout->addWidget( commandText );
    centralLayout->addWidget( synchroCommand );
    centralLayout->addWidget( commandAboutText );
    centralLayout->addWidget( synchroOnBox );
    centralLayout->addWidget( periodicCheckBox );
    centralLayout->addStretch();

    // Основной слой устанавливается
    setLayout( centralLayout );
}


void AppConfigPage_Synchro::setupSignals(void) {
    connect( enablePeriodicCheckBase, &QCheckBox::toggled, this, &AppConfigPage_Synchro::onEnablePeriodicCheckBase);
    connect( synchroOnPeriodic,       &QCheckBox::toggled, this, &AppConfigPage_Synchro::onEnablePeriodicSynchro);
}


void AppConfigPage_Synchro::onEnablePeriodicCheckBase(bool state) {
    checkBasePeriodText->setEnabled(state);
    checkBasePeriod->setEnabled(state);
    checkBasePeriodPostfix->setEnabled(state);

    enablePeriodicCheckMessage->setEnabled(state);
}


void AppConfigPage_Synchro::onEnablePeriodicSynchro(bool state) {
    synchroPeriodText->setEnabled(state);
    synchroPeriod->setEnabled(state);
    synchroPeriodPostfix->setEnabled(state);
}


/// @return уровень сложности сделанных изменений
/// 0 - изменения не требуют перезапуска программы
/// 1 - изменения требуют перезапуска программы
int AppConfigPage_Synchro::applyChanges(void) {
    qDebug() << "Apply changes synchro";

    // Сохраняется строка с командой синхронизации
    if(AppConfig::get().get_synchrocommand()!=synchroCommand->text())
        AppConfig::get().set_synchrocommand(synchroCommand->text());

    // Сохраняется настройка запуска синхронизации при старте
    if(AppConfig::get().get_synchroonstartup()!=synchroOnStartup->isChecked())
        AppConfig::get().set_synchroonstartup(synchroOnStartup->isChecked());

    // Сохраняется настройка запуска синхронизации при выходе
    if(AppConfig::get().get_synchroonexit()!=synchroOnExit->isChecked())
        AppConfig::get().set_synchroonexit(synchroOnExit->isChecked());


    // Сохраняется период синхронизации
    // установка периода должна производиться перед обработкой галки включения/выключения
    if(AppConfig::get().getPeriodicSynchroPeriod()!=synchroPeriod->value()) {
        AppConfig::get().setPeriodicSynchroPeriod( synchroPeriod->value() );
        periodicSynchro.setDelay( synchroPeriod->value() );
    }


    // Сохраняется настройка париодического запуска синхронизации
    if(AppConfig::get().getEnablePeriodicSynchro()!=synchroOnPeriodic->isChecked()) {
        AppConfig::get().setEnablePeriodicSynchro(synchroOnPeriodic->isChecked());

        if(AppConfig::get().getEnablePeriodicSynchro() ) {
            showMessageBox(tr("The first background <b>synchronizing</b> starting.<br/>Maybe a slight delay or freezing window..."));
            periodicSynchro.start();
        }
        else
            periodicSynchro.stop();
    }


    // Сохраняется период проверки дерева базы
    // установка периода должна производиться перед обработкой галки включения/выключения
    if(AppConfig::get().getCheckBasePeriod()!=checkBasePeriod->value()) {
        AppConfig::get().setCheckBasePeriod( checkBasePeriod->value() );
        periodicCheckBase.setDelay( checkBasePeriod->value() );
    }


    // Сохраняется разрешение переодически проверять дерево базы
    if(AppConfig::get().getEnablePeriodicCheckBase()!=enablePeriodicCheckBase->isChecked()) {
        AppConfig::get().setEnablePeriodicCheckBase(enablePeriodicCheckBase->isChecked());

        if(AppConfig::get().getEnablePeriodicCheckBase() ) {
            showMessageBox(tr("The first background <b>checking base</b> starting.<br/>Maybe a slight delay or freezing window..."));
            periodicCheckBase.start();
        }
        else
            periodicCheckBase.stop();
    }

    // Сохраняется разрешение выводить сообщение при внешнем изменении базы
    if(AppConfig::get().getEnablePeriodicCheckMessage()!=enablePeriodicCheckMessage->isChecked())
        AppConfig::get().setEnablePeriodicCheckMessage(enablePeriodicCheckMessage->isChecked());

    return 0;
}
