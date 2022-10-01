#pragma once

#include <QObject>
#include "views/shortcutSettings/ShortcutSettingsView.h"
#include "models/shortcutSettings/ShortcutSettingsModel.h"


class ShortcutSettingsController : public QObject
{
    Q_OBJECT
public:

    struct ShortcutData {
        QString section;
        QString command;
        QString description;
        QString keys;
        QString defaultKeys;
    };

    ShortcutSettingsController(QObject *parent = nullptr) {Q_UNUSED(parent);};
    ~ShortcutSettingsController() {};

    void init();
    void applyChanges();

    ShortcutSettingsView* getView() const {return view;}

    ShortcutData getShortcutData(const QModelIndex &index) const;
    ShortcutData getEmptyShortcutData() const;

    void setShortcut(QString shortcutFullName, QString sequenceText);
    void resetAllShortcutsToDefault() {model->resetAllShortcutsToDefault();}

protected:

    ShortcutSettingsView *view;
    ShortcutSettingsModel *model;

};

