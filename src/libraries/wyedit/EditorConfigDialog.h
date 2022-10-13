#pragma once

#include "../../views/appConfigWindow/ConfigDialog.h"
#include <QWidget>

class EditorConfigDialog : public QWidget {
    Q_OBJECT

  public:
    EditorConfigDialog(QWidget *parent = nullptr);

  private:
    ConfigDialog *configDialog;
};
