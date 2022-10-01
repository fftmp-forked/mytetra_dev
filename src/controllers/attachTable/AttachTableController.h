#pragma once

#include <QObject>
#include <QModelIndexList>

#include "views/attachTable/AttachTableView.h"
#include "models/attachTable/AttachTableModel.h"
#include "models/attachTable/AttachTableData.h"
#include "views/record/MetaEditor.h"


class AttachTableController : public QObject
{
  Q_OBJECT

public:

  AttachTableController(QObject *parent, QObject *iMetaEditor );
  virtual ~AttachTableController();

  AttachTableView *getView(void) const {return view;}
  void setAttachTableData(AttachTableData *attachTableData);
  AttachTableData *getAttachTableData();

  QList<QString> getSelectedId(void);

public slots:

  void onAddAttach(void) {addSmart(Attach::Type::file);}
  void onAddAttachFromUrl(void);
  void onAddLink(void) {addSmart(Attach::Type::link);}
  void onEditFileName(void);
  void onDeleteAttach(void);
  void onOpenAttach(void);
  void onShowAttachInfo(void);
  void onSaveAsAttach(void);

  void onSwitchToEditor(void);


protected:
  void addSmart(Attach::Type attachType);
  bool addAttach(Attach::Type attachType, QString currFullFileName, QString currShortFileName);
  void saveState();
  QStringList selectFilesForAdding(Attach::Type attachType);
  void saveAttachToUserPlace(QString fromFullFileName, QString toFullFileName);
  void updateAttachListInEditor(void);

  AttachTableView *view;
  AttachTableModel *model;

  MetaEditor *metaEditor;
};

