#include <QDebug>

#include "AttachTableScreen.h"
#include "AttachTableView.h"
#include "controllers/attachTable/AttachTableController.h"
#include "libraries/ShortcutManager/ShortcutManager.h"
#include "libraries/helpers/ActionHelper.h"

AttachTableScreen::AttachTableScreen(QWidget *parent) : QWidget(parent) {
    // По факту этот класс - синглтон. Синглтон сам задает себе имя
    this->setObjectName("attachTableScreen");

    // Создаются действия. Они используются как в данном классе (на кнопках), так и в контекстном меню в AttachTableView
    setupActions();
    setupShortcuts();

    // Инициализируется контроллер списка файлов
    attachTableController = new AttachTableController(this, parent);
    attachTableController->setObjectName("attachTableController");

    setupUI();
    setupSignals();
    assembly();
}

// Настройка возможных действий
void AttachTableScreen::setupActions(void) {
    actionAddAttach = new QAction(QIcon(":/resource/pic/attach_add.svg"), "", this);
    actionAddAttachFromUrl = new QAction(QIcon(":/resource/pic/attach_add_from_url.svg"), "", this);
    actionAddLink = new QAction(QIcon(":/resource/pic/attach_add_link.svg"), "", this);
    actionEditFileName = new QAction(QIcon(":/resource/pic/attach_edit.svg"), "", this);
    actionDeleteAttach = new QAction(QIcon(":/resource/pic/attach_delete.svg"), "", this);
    actionOpenAttach = new QAction(QIcon(":/resource/pic/attach_preview.svg"), "", this);
    actionSaveAsAttach = new QAction(QIcon(":/resource/pic/attach_save_as.svg"), "", this);
    actionShowAttachInfo = new QAction(QIcon(":/resource/pic/attach_info.svg"), "", this);
    actionSwitchToEditor = new QAction(QIcon(":/resource/pic/attach_switch_to_editor.svg"), "", this);
}

void AttachTableScreen::setupShortcuts(void) {
    qDebug() << "Setup shortcut for" << staticMetaObject.className();
    QList<QPair<QString, QAction *>> attachActions{
        {"addAttach", actionAddAttach},
        {"addAttachFromUrl", actionAddAttachFromUrl},
        {"addLink", actionAddLink},
        {"editFileName", actionEditFileName},
        {"deleteAttach", actionDeleteAttach},
        {"openAttach", actionOpenAttach},
        {"saveAsAttach", actionSaveAsAttach},
        {"showAttachInfo", actionShowAttachInfo},
        {"switchToEditor", actionSwitchToEditor},
    };
    ShortcutManager::get().initActions(ShortcutManager::SECTION_ATTACH, attachActions);
}

void AttachTableScreen::setupUI() {
    // Создание тулбара
    toolsLine = new QToolBar(this);

    // Создание кнопок на тулбаре
    insertActionAsButton(toolsLine, actionAddAttach);
    insertActionAsButton(toolsLine, actionAddAttachFromUrl);
    insertActionAsButton(toolsLine, actionAddLink);
    insertActionAsButton(toolsLine, actionEditFileName);
    insertActionAsButton(toolsLine, actionDeleteAttach);
    insertActionAsButton(toolsLine, actionSaveAsAttach);
    insertActionAsButton(toolsLine, actionOpenAttach);
    insertActionAsButton(toolsLine, actionShowAttachInfo);

    toolsLine->addSeparator();

    insertActionAsButton(toolsLine, actionSwitchToEditor);
}

void AttachTableScreen::setupSignals(void) {
    // Связывание действий
    connect(actionAddAttach, &QAction::triggered, attachTableController, &AttachTableController::onAddAttach);
    connect(actionAddAttachFromUrl, &QAction::triggered, attachTableController, &AttachTableController::onAddAttachFromUrl);
    connect(actionAddLink, &QAction::triggered, attachTableController, &AttachTableController::onAddLink);
    connect(actionEditFileName, &QAction::triggered, attachTableController, &AttachTableController::onEditFileName);
    connect(actionDeleteAttach, &QAction::triggered, attachTableController, &AttachTableController::onDeleteAttach);
    connect(actionOpenAttach, &QAction::triggered, attachTableController, &AttachTableController::onOpenAttach);
    connect(actionSaveAsAttach, &QAction::triggered, attachTableController, &AttachTableController::onSaveAsAttach);

    connect(actionShowAttachInfo, &QAction::triggered, attachTableController, &AttachTableController::onShowAttachInfo);

    connect(actionSwitchToEditor, &QAction::triggered, attachTableController, &AttachTableController::onSwitchToEditor);

    // Обновление горячих клавиш, если они были изменены
    connect(&ShortcutManager::get(), &ShortcutManager::updateWidgetShortcut, this, &AttachTableScreen::setupShortcuts);
}

void AttachTableScreen::assembly() {

    auto screenLayout = new QVBoxLayout(this);

    screenLayout->addWidget(toolsLine);
    screenLayout->addWidget(attachTableController->getView());

    setLayout(screenLayout);

    // Границы убираются, так как данный объект будет использоваться как виджет
    layout()->setContentsMargins(0, 0, 0, 0);
}

// Очистка таблицы приаттаченных файлов
// Список на экране должен быть пустой
// Вызывается в моменты, когда ни одна запись не выбрана или содержимое записи недоступно
void AttachTableScreen::clear() {
    attachTableController->setAttachTableData(NULL);
}

void AttachTableScreen::setReadOnly(bool state) {
    // Обрабатываются разрешаемые/запрещаемые действия
    actionAddAttach->setEnabled(!state);
    actionAddAttachFromUrl->setEnabled(!state);
    actionAddLink->setEnabled(!state);
    actionEditFileName->setEnabled(!state);
    actionDeleteAttach->setEnabled(!state);
}
