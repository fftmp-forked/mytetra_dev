#pragma once

#include <QWidget>
#include "views/appConfigWindow/ConfigDialog.h"


class EditorConfigDialog : public QWidget
{
 Q_OBJECT

public:
 EditorConfigDialog(QWidget *parent=nullptr);
    
private:
 ConfigDialog *configDialog;
};

