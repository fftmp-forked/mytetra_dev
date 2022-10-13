#pragma once

#include <QWidget>

class ConfigPage : public QWidget {
    Q_OBJECT

  public:
    ConfigPage(QWidget *parent = nullptr) { Q_UNUSED(parent); };
    virtual ~ConfigPage(){};

    virtual int applyChanges(void) { return 0; };
};
