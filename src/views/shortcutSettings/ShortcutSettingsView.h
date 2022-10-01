#pragma once

#include <QTreeView>


class ShortcutSettingsView : public QTreeView
{
    Q_OBJECT
public:
    ShortcutSettingsView(QWidget *parent=nullptr);
    ~ShortcutSettingsView();

    void init();

protected slots:

    void onCurrentChanged(const QModelIndex &index, const QModelIndex &prevIndex);

};

