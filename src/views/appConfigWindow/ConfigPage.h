#pragma once

#include <QWidget>


class ConfigPage : public QWidget
{
    Q_OBJECT

public:
    ConfigPage(QWidget *parent = nullptr);
    virtual ~ConfigPage();

    virtual int applyChanges(void);
};


