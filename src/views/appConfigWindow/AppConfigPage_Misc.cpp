#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "AppConfigPage_Misc.h"
#include "libraries/GlobalParameters.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/ConfigEditorHelper.h"
#include "models/appConfig/AppConfig.h"


AppConfigPage_Misc::AppConfigPage_Misc(QWidget *parent) : ConfigPage(parent) {
  setupUi();
  setupSignals();
  assembly();
}


void AppConfigPage_Misc::setupUi(void)
{
  qDebug() << "Create misc config page";

  // Блок настройки подтверждения для действия "cut" на ветке
  cutBranchConfirm=new QCheckBox(this);
  cutBranchConfirm->setText(tr("Confirm item cut"));
  cutBranchConfirm->setChecked(AppConfig::get().get_cutbranchconfirm());

  // Блок настройки отображения отладочных сообщений в консоли
  printDebugMessages=new QCheckBox(this);
  printDebugMessages->setText(tr("Print debug messages to console"));
  printDebugMessages->setChecked(AppConfig::get().get_printdebugmessages());

  // Кнопка редактирования файла конфигурации MyTetra
  editMyTetraConfigFile=new QPushButton(this);
  editMyTetraConfigFile->setText(tr("Edit config file"));
  editMyTetraConfigFile->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));
}


void AppConfigPage_Misc::setupSignals(void)
{
  connect(editMyTetraConfigFile, &QPushButton::clicked, this, &AppConfigPage_Misc::onClickedEditMyTetraConfigFile);
}


void AppConfigPage_Misc::assembly(void)
{
  // Группировщик виджетов для опасной зоны
  dangerBox=new QGroupBox(this);
  dangerBox->setTitle(tr("Danger actions (Attention!)"));

  // Виджеты вставляются в группировщик опасной зоны
  QVBoxLayout *dangerLayout = new QVBoxLayout;
  dangerLayout->addWidget(editMyTetraConfigFile);
  dangerBox->setLayout(dangerLayout);

  // Собирается основной слой
  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addWidget(cutBranchConfirm);
  centralLayout->addWidget(printDebugMessages);
  centralLayout->addWidget(dangerBox);
  centralLayout->addStretch();

  // Основной слой устанавливается
  setLayout(centralLayout);
}


void AppConfigPage_Misc::onClickedEditMyTetraConfigFile(void)
{
  // Сбрасываются в файл конфига все возможные изменения, которые, возможно еще не были записаны
  AppConfig::get().sync();

  ConfigEditorHelper::editConfigFile( GlobalParameters::get().getWorkDirectory()+"/conf.ini", 0.8 );
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Misc::applyChanges(void) {
  qDebug() << "Apply changes misc";

  // Сохраняется настройка подтверждения для действия "cut" на ветке
  if(AppConfig::get().get_cutbranchconfirm()!=cutBranchConfirm->isChecked())
    AppConfig::get().set_cutbranchconfirm(cutBranchConfirm->isChecked());

  AppConfig::get().set_printdebugmessages(printDebugMessages->isChecked());
  setupDebug(printDebugMessages->isChecked());

  return 0;
}
