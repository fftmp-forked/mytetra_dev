#pragma once

#include <QString>
#include <QWidget>

class ConfigDialog;
class QListWidgetItem;

/// @brief Объект, подготавливающий и отображающий диалог настройки MyTetra
class AppConfigDialog
{

public:
    explicit AppConfigDialog(const QString &firstPageName, QWidget *parent);
    virtual ~AppConfigDialog();

    void exec();

    void setMenuListVisible(bool visible);
    
private:
    ConfigDialog *configDialog;

    QListWidgetItem *pageMain;
    QListWidgetItem *pageAppearance;
    QListWidgetItem *pageSynchro;
    QListWidgetItem *pageRecordTable;
    QListWidgetItem *pageAttach;
    QListWidgetItem *pageKeyboard;
    QListWidgetItem *pageHistory;
    QListWidgetItem *pageMisc;

    void changePage(QString name);
};

