#include <QDebug>
#include <QDialogButtonBox>
#include <QToolBar>
#include <QVBoxLayout>

#include "DatabasesManagementScreen.h"
#include "DatabasesManagementTable.h"
#include "controllers/databasesManagement/DatabasesManagementController.h"

DatabasesManagementScreen::DatabasesManagementScreen(QWidget *parent) : QDialog(parent) {
    // По факту этот класс - синглтон. Синглтон сам задает себе имя
    this->setObjectName("DatabasesManagementScreen");

    this->setWindowTitle(tr("Databases management"));

    // Инициализируется контроллер отображения записей лога
    auto databasesManagementController = new DatabasesManagementController(this);
    databasesManagementController->setObjectName("DatabasesManagementController");

    auto actionSelect = new QAction(QIcon(":/resource/pic/dbmanagement_select.svg"), tr("Select database"), this);
    connect(actionSelect, &QAction::triggered, databasesManagementController, &DatabasesManagementController::onSelectClicked);

    auto actionCreate = new QAction(QIcon(":/resource/pic/dbmanagement_create.svg"), tr("Create new database"), this);
    connect(actionCreate, &QAction::triggered, databasesManagementController, &DatabasesManagementController::onCreateClicked);

    auto actionAdd = new QAction(QIcon(":/resource/pic/dbmanagement_append.svg"), tr("Append exists database"), this);
    connect(actionAdd, &QAction::triggered, databasesManagementController, &DatabasesManagementController::onAddClicked);

    actionCopy = new QAction(QIcon(":/resource/pic/cb_copy.svg"), tr("Copy selected rows"), this);
    connect(actionCopy, &QAction::triggered, databasesManagementController, &DatabasesManagementController::onCopyClicked);

    // Экранная таблица с отображением лога действий
    auto databasesManagementTable = databasesManagementController->getView();

    // Создание тулбара
    auto toolBar = new QToolBar(this);
    toolBar->addActions({actionSelect, actionCreate, actionAdd});
    toolBar->addSeparator();
    toolBar->addAction(actionCopy);

    // Создание набора диалоговых кнопок
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &DatabasesManagementScreen::close);

    auto screenLayout = new QVBoxLayout(this);
    screenLayout->addWidget(toolBar);
    screenLayout->addWidget(databasesManagementTable);
    screenLayout->addWidget(buttonBox);

    setLayout(screenLayout);

    databasesManagementTable->init();
}
