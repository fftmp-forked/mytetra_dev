#pragma once

#include <QObject>
#include <QModelIndexList>

#include "views/actionLog/ActionLogView.h"
#include "models/actionLog/ActionLogModel.h"


class ActionLogController : public QObject
{
  Q_OBJECT

public:

  ActionLogController(QObject *parent = nullptr);
  virtual ~ActionLogController();

  ActionLogView *getView(void);

public slots:

  void onCopyClicked();

protected:

  ActionLogView *view;
  ActionLogModel *model;

};

