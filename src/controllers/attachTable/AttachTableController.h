#pragma once

#include <QModelIndexList>
#include <QObject>

#include "models/attachTable/AttachTableData.h"
#include "models/attachTable/AttachTableModel.h"
#include "views/attachTable/AttachTableView.h"
#include "views/record/MetaEditor.h"

class AttachTableController : public QObject {
    Q_OBJECT

  public:
    AttachTableController(QObject *parent, QObject *iMetaEditor);
    virtual ~AttachTableController();

    AttachTableView *getView() const { return view; }
    void setAttachTableData(AttachTableData *attachTableData);
    AttachTableData *getAttachTableData();

    QList<QString> getSelectedId();

  public slots:

    void onAddAttach() { addSmart(Attach::Type::file); }
    void onAddAttachFromUrl();
    void onAddLink() { addSmart(Attach::Type::link); }
    void onEditFileName();
    void onDeleteAttach();
    void onOpenAttach();
    void onShowAttachInfo();
    void onSaveAsAttach();

    void onSwitchToEditor();

  protected:
    void addSmart(Attach::Type attachType);
    bool addAttach(Attach::Type attachType, QString currFullFileName, QString currShortFileName);
    void saveState();
    QStringList selectFilesForAdding(Attach::Type attachType);
    void saveAttachToUserPlace(QString fromFullFileName, QString toFullFileName);
    void updateAttachListInEditor();

    AttachTableView *view;
    AttachTableModel *model;

    MetaEditor *metaEditor;
};
