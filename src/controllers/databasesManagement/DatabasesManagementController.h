#pragma once

#include <QObject>

#include "models/databasesManagement/DatabasesManagementModel.h"
#include "views/databasesManagement/DatabasesManagementTable.h"

class DatabasesManagementController : public QObject {
    Q_OBJECT

  public:
    DatabasesManagementController(QObject *parent = nullptr);
    virtual ~DatabasesManagementController();

    DatabasesManagementTable *getView(void);

  public slots:

    void onSelectClicked();
    void onCreateClicked();
    void onAddClicked();
    void onCopyClicked();

  protected:
    DatabasesManagementTable *view;
    DatabasesManagementModel *model;
};
