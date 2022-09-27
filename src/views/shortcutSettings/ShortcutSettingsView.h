#pragma once

#include <QTreeView>


class ShortcutSettingsView : public QTreeView
{
    Q_OBJECT
public:
    ShortcutSettingsView(QWidget *parent=nullptr);
    ~ShortcutSettingsView();

    void init();

//    void setController(ShortcutSettingsController *pController);

protected slots:

    void onCurrentChanged(const QModelIndex &index, const QModelIndex &prevIndex);

};

