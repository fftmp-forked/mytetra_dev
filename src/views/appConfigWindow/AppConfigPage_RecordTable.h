#pragma once

#include <QWidget>
#include <QCheckBox>
#include <QMap>
#include <QGroupBox>

#include "ConfigPage.h"

class AppConfigPage_RecordTable : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_RecordTable(QWidget *parent = nullptr);
  int applyChanges(void);

signals:
  void recordTableConfigChange(void); // Сигнал, испускающийся когда изменились настройки таблицы конечных записей

private slots:
 void onFieldToggle(bool);

protected:

  void setupSignals(void);

  QMap<QString, QCheckBox *> fields;

  QCheckBox *showHorizontalHeader;
  QCheckBox *showVerticalHeader;

  // Объединяющая рамка
  QGroupBox *showFieldsBox;
  QGroupBox *showHeadersBox;
};


