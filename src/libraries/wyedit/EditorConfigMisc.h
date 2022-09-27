#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

#include "views/appConfigWindow/ConfigPage.h"

class EditorConfig;


class EditorConfigMisc : public ConfigPage
{
 Q_OBJECT

public:
  EditorConfigMisc(QWidget *parent = nullptr);
  
  int applyChanges(void);
    
private slots:
  void onClickedEditWyEditConfigFile(void);
 
private:

  QLabel *indentStepLabel;
  QLabel *indentStepFlexion;
  QSpinBox *indentStep;

  // Размер табуляции для клавиши Tab
  QLabel *tabStopDistanceLabel;
  QLabel *tabStopDistanceFlexionLabel;
  QSpinBox *tabStopDistanceSpinBox;

  QPushButton *editWyEditConfigFile;
  
  EditorConfig *conf;

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);
  
};



