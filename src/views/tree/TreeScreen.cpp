#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QInputDialog>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QString>

#include "KnowTreeView.h"
#include "TreeScreen.h"

#include "controllers/recordTable/RecordTableController.h"
#include "libraries/ClipboardBranch.h"
#include "libraries/FixedParameters.h"
#include "libraries/IconSelectDialog.h"
#include "libraries/ShortcutManager/ShortcutManager.h"
#include "libraries/helpers/ActionHelper.h"
#include "libraries/helpers/MessageHelper.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/helpers/UniqueIdHelper.h"
#include "models/appConfig/AppConfig.h"
#include "models/recordTable/RecordTableData.h"
#include "models/tree/KnowTreeModel.h"
#include "models/tree/TreeItem.h"
#include "views/mainWindow/MainWindow.h"
#include "views/recordTable/RecordTableScreen.h"


TreeScreen::TreeScreen(QWidget *parent) : QWidget(parent) {
    lastKnowTreeModifyDateTime = QDateTime();
    lastKnowTreeSize = 0;

    setupActions();
    setupShortcuts();
    setupUI();
    setupModels();
    setupSignals();
    assembly();
}

void TreeScreen::setupActions() {
    QAction *ac;

    // Разворачивание всех подветок
    actions[A_expandAllSubbranch] = ac = new QAction(QIcon(":/resource/pic/expand_all_subbranch.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::expandAllSubbranch);

    // Сворачивание всех подветок
    actions[A_collapseAllSubbranch] = ac = new QAction(QIcon(":/resource/pic/collapse_all_subbranch.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::collapseAllSubbranch);

    // Перемещение ветки вверх
    actions[A_moveUpBranch] = ac = new QAction(QIcon(":/resource/pic/move_up.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::moveUpBranch);

    // Перемещение ветки вниз
    actions[A_moveDownBranch] = ac = new QAction(QIcon(":/resource/pic/move_dn.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::moveDownBranch);

    // Вставка новой подветки
    actions[A_insSubbranch] = ac = new QAction(QIcon(":/resource/pic/add_subbranch.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::insSubbranch);

    // Вставка новой ветки
    actions[A_insBranch] = ac = new QAction(QIcon(":/resource/pic/add_branch.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::insBranch);

    // Редактирование ветки
    actions[A_editBranch] = ac = new QAction(QIcon(":/resource/pic/note_edit.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::editBranch);

    // Удаление ветки
    actions[A_delBranch] = ac = new QAction(QIcon(":/resource/pic/note_delete.svg"), "", this);
    connect(ac, &QAction::triggered, this, [this]() { delBranch(); });

    // Удаление ветки с сохранением копии в буфер обмена
    actions[A_cutBranch] = ac = new QAction(QIcon(":/resource/pic/branch_cut.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::cutBranch);

    // Копирование ветки в буфер обмена
    actions[A_copyBranch] = ac = new QAction(QIcon(":/resource/pic/branch_copy.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::copyBranch);

    // Вставка ветки из буфера обмена
    actions[A_pasteBranch] = ac = new QAction(QIcon(":/resource/pic/branch_paste.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::pasteBranch);

    // Вставка ветки из буфера обмена в виде подветки
    actions[A_pasteSubbranch] = ac = new QAction(QIcon(":/resource/pic/branch_paste.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::pasteSubbranch);

    // Добавление иконки к ветке
    actions[A_setIcon] = ac = new QAction(QIcon(":/resource/pic/set_icon.svg"), "", this);
    connect(ac, &QAction::triggered, this, &TreeScreen::setIcon);

    // Открытие поиска по базе (связывание клика происходит в MainWindow)
    actions[A_findInBase] = ac = new QAction(QIcon(":/resource/pic/find_in_base.svg"), tr("Find in base"), this);
    ac->setStatusTip(tr("Find in base"));
    connect(ac, &QAction::triggered, this, &TreeScreen::treeScreenFindInBaseClicked);
}

void TreeScreen::setupShortcuts() {
    qDebug() << "Setup shortcut for" << staticMetaObject.className();
    QList<QPair<QString, QAction *>> treeActions {
        {"expandAllSubbranch", actions[A_expandAllSubbranch]},
        {"collapseAllSubbranch", actions[A_collapseAllSubbranch]},
        {"moveUpBranch", actions[A_moveUpBranch]},
        {"moveDownBranch", actions[A_moveDownBranch]},
        {"insSubbranch", actions[A_insSubbranch]},
        {"insBranch", actions[A_insBranch]},
        {"editBranch", actions[A_editBranch]},
        {"delBranch", actions[A_delBranch]},
        {"cutBranch", actions[A_cutBranch]},
        {"copyBranch", actions[A_copyBranch]},
        {"pasteBranch", actions[A_pasteBranch]},
        {"pasteSubbranch", actions[A_pasteSubbranch]},
        {"setIcon", actions[A_setIcon]},
    };
    ShortcutManager::get().initActions(ShortcutManager::SECTION_TREE, treeActions);
}

void TreeScreen::setupUI() {
    // Наполнение панели инструментов
    toolsLine = new QToolBar(this);

    toolsLine->addActions({actions[A_insSubbranch], actions[A_insBranch], actions[A_editBranch], actions[A_delBranch]});

    toolsLine->addSeparator();
    toolsLine->addActions({actions[A_expandAllSubbranch], actions[A_collapseAllSubbranch]});

    toolsLine->addSeparator();

    toolsLine->addActions({actions[A_moveUpBranch], actions[A_moveDownBranch]});

    // Добавление скрытых действий, которые не видны на тулбаре, но видны на контекстном меню
    insertActionAsButton(toolsLine, actions[A_cutBranch], false);
    insertActionAsButton(toolsLine, actions[A_copyBranch], false);
    insertActionAsButton(toolsLine, actions[A_pasteBranch], false);
    insertActionAsButton(toolsLine, actions[A_pasteSubbranch], false);

    insertActionAsButton(toolsLine, actions[A_setIcon], false);

    knowTreeView = new KnowTreeView(this);
    knowTreeView->setObjectName("knowTreeView");
    knowTreeView->setMinimumSize(150, 250);
    knowTreeView->setWordWrap(true);

    // Временно сделан одинарный режим выбора пунктов
    /// @todo: Множественный режим надо выставить тогда, когда станет ясно, как удалять несколько произвольных веток так, чтобы
    /// в процессе удаления QModelIndex нижестоящих еще не удаленных веток не менялся
    // knowTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    knowTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

    knowTreeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    knowTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Представление не должно позволять редактировать элементы обычным путем
    knowTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void TreeScreen::setupModels() {
    // Создание и первичная настройка модели
    knowTreeModel = new KnowTreeModel(this);

    // Загрузка данных
    knowTreeModel->initFromXML(AppConfig::get().get_tetradir() + "/mytetra.xml");

    // Модель подключается к виду
    knowTreeView->setModel(knowTreeModel);
}

// Открытие контекстного меню в дереве разделов
void TreeScreen::onCustomContextMenuRequested(const QPoint &pos) {
    qDebug() << "In TreeScreen::onCustomContextMenuRequested";

    // Конструирование меню
    QMenu menu(this);
    menu.addActions({actions[A_insSubbranch], actions[A_insBranch], actions[A_editBranch], actions[A_delBranch], actions[A_setIcon]});
    menu.addSeparator();
    menu.addActions({actions[A_expandAllSubbranch], actions[A_collapseAllSubbranch]});
    menu.addSeparator();
    menu.addActions({actions[A_moveUpBranch], actions[A_moveDownBranch]});
    menu.addSeparator();
    menu.addActions({actions[A_cutBranch], actions[A_copyBranch], actions[A_pasteBranch], actions[A_pasteSubbranch]});

    // Если в буфере есть ветки, соответствующие пункты становятся активными
    bool isBranch = false;
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    if (mimeData && mimeData->hasFormat(FixedParameters::appTextId + "/branch"))
        isBranch = true;

    actions[A_pasteBranch]->setEnabled(isBranch);
    actions[A_pasteSubbranch]->setEnabled(isBranch);

    // Включение отображения меню на экране
    // menu.exec(event->globalPos());
    menu.exec(knowTreeView->viewport()->mapToGlobal(pos));
}

void TreeScreen::setupSignals() {
    // Соединение сигнал-слот чтобы показать контекстное меню по правому клику на ветке
    connect(knowTreeView, &KnowTreeView::customContextMenuRequested, this, &TreeScreen::onCustomContextMenuRequested);

    // Соединение сигнал-слот что ветка выбрана мышкой или стрелками на клавиатуре (через selection-модель)
    connect(knowTreeView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &TreeScreen::onKnowtreeClicked);

    // Обновление горячих клавиш, если они были изменены
    connect(&ShortcutManager::get(), &ShortcutManager::updateWidgetShortcut, this, &TreeScreen::setupShortcuts);
}

void TreeScreen::assembly() {
    auto treeScreenLayout = new QVBoxLayout(this);
    treeScreenLayout->setObjectName("treescreen_QVBoxLayout");

    treeScreenLayout->addWidget(toolsLine);
    treeScreenLayout->addWidget(knowTreeView);

    setLayout(treeScreenLayout);

    // Границы убираются, так как данный объект будет использоваться как виджет
    layout()->setContentsMargins(0, 2, 0, 0);
}

void TreeScreen::expandAllSubbranch() {
    // Получение индексов выделенных строк
    QModelIndexList selectitems = knowTreeView->selectionModel()->selectedIndexes();

    for (int i = 0; i < selectitems.size(); ++i)
        expandOrCollapseRecurse(selectitems.at(i), true);
}

void TreeScreen::collapseAllSubbranch() {
    // Получение индексов выделенных строк
    QModelIndexList selectitems = knowTreeView->selectionModel()->selectedIndexes();

    for (int i = 0; i < selectitems.size(); ++i)
        expandOrCollapseRecurse(selectitems.at(i), false);
}

void TreeScreen::expandOrCollapseRecurse(QModelIndex modelIndex, bool mode) {
    knowTreeView->setExpanded(modelIndex, mode);

    // Перебор дочерних (child) элементов
    int i = 0;
    while ((modelIndex.model()->index(i, 0, modelIndex)).isValid()) {
        expandOrCollapseRecurse(modelIndex.model()->index(i, 0, modelIndex), mode);
        i++;
    }
}

void TreeScreen::moveUpBranch() {
    moveUpDownBranch(1);
}

void TreeScreen::moveDownBranch() {
    moveUpDownBranch(-1);
}

void TreeScreen::moveUpDownBranch(int direction) {
    // Если ветку нельзя перемещать
    if (!moveCheckEnable())
        return;

    // Получение индекса выделенной строки
    QModelIndex index = getCurrentItemIndex();

    // Ветка перемещается
    QModelIndex index_after_move;
    if (direction == 1)
        index_after_move = knowTreeModel->moveUpBranch(index);
    else
        index_after_move = knowTreeModel->moveDownBranch(index);

    // Установка курсора на позицию, куда была перенесена ветка
    if (index_after_move.isValid()) {
        knowTreeView->selectionModel()->setCurrentIndex(index_after_move, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
        knowTreeView->selectionModel()->select(index_after_move, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
    }

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();
}

bool TreeScreen::moveCheckEnable() {
    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = knowTreeView->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки
    if (selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for moving."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return false;
    } else
        return true;
}

void TreeScreen::insBranch() {
    qDebug() << "In ins_branch()";

    insBranchSmart(true);
}

void TreeScreen::insSubbranch() {
    qDebug() << "In ins_subbranch()";

    insBranchSmart(false);
}

// Вспомогательная функция, используется при добавлении ветки
// Если is_branch=true, то добавляется ветка на тот же уровень
// Если is_branch=false, то добавляется подветка
void TreeScreen::insBranchSmart(bool is_branch) {
    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = knowTreeView->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки
    if (selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling insert operation."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return;
    }

    // Создается окно ввода данных
    bool ok;
    QString title, text;
    if (is_branch) {
        title = tr("Create new item");
        text = tr("Item name:");
    } else {
        title = tr("Create new sub item");
        text = tr("Sub item name:");
    }
    QString name = QInputDialog::getText(this, title, text, QLineEdit::Normal, "", &ok);
    if (!(ok && !name.isEmpty()))
        return; // Если была нажата отмена

    // Получение индекса выделенной строки
    QModelIndex index = getCurrentItemIndex();

    // Введенные данные добавляются
    insBranchProcess(index, name, is_branch);
}

void TreeScreen::insBranchProcess(QModelIndex index, QString name, bool is_branch) {
    // Получение ссылки на узел, который соответствует выделенной строке
    TreeItem *item = knowTreeModel->getItem(index);

    MainWindow::get().setDisabled(true);

    // Получение уникального идентификатора
    QString id = getUniqueId();

    // Инфополя создаваемой ветки
    QMap<QString, QString> branchFields;
    branchFields["id"] = id;
    branchFields["name"] = name;

    // Вставка данных и установка курсора

    // Одноранговая ветка
    if (is_branch) {
        // Вставка новых данных в модель дерева записей
        knowTreeModel->addNewSiblingBranch(index, branchFields);

        // Установка курсора на только что созданную позицию

        // Чтобы вычислить позицию, надо синхронно получить parent элемента,
        // на уровне которого должен был создасться новый элемент.
        // Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
        // Затем у объекта Item выяснить количество элементов, и установить
        // засветку через метод index() относительно parent в виде QModelIndex
        QModelIndex setto = knowTreeModel->index(item->parent()->childCount() - 1, 0, index.parent());
        knowTreeView->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
    } else {
        // Подветка

        // Вставка новых данных в модель дерева записей
        knowTreeModel->addNewChildBranch(index, branchFields);

        // Установка курсора на только что созданную позицию
        QModelIndex setto = knowTreeModel->indexChildren(index, item->childCount() - 1);
        knowTreeView->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

        // А можно было установить курсор на нужную позицию и так
        // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
        //                                             QItemSelectionModel::ClearAndSelect);
    }

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();

    MainWindow::get().setEnabled(true);
}

void TreeScreen::editBranch() {
    qDebug() << "In edit_branch()";

    // Получение списка индексов QModelIndex выделенных элементов
    auto selectitems = knowTreeView->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки
    if (selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling edit operation."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return;
    }

    // Получение индекса выделенной строки
    auto index = getCurrentItemIndex();

    // Получение ссылки на узел, который соответствует выделенной строке
    auto item = knowTreeModel->getItem(index);

    // Получение имени ветки
    auto name = item->getField("name");

    // Создается окно ввода данных
    bool ok;
    auto newname = QInputDialog::getText(this, tr("Edit item name"), tr("Item name:"), QLineEdit::Normal, name, &ok);

    // Если была нажата отмена
    if (!(ok && !newname.isEmpty()))
        return;

    MainWindow::get().setDisabled(true);

    item->setField("name", newname);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();

    MainWindow::get().setEnabled(true);
}

/// @brief Удаление выбранных веток, вызывается при выборе соотвествущей кнопки или пункта меню
void TreeScreen::delBranch(QString mode) {
    qDebug() << "In del_branch()";

    // На время удаления блокируется главное окно
    MainWindow::get().setDisabled(true);
    MainWindow::get().blockSignals(true);

    // Получение списка индексов QModelIndex выделенных элементов
    auto selectItems = knowTreeView->selectionModel()->selectedIndexes();

    // Список имен веток, которые нужно удалить
    QStringList branchesName;
    for (int i = 0; i < selectItems.size(); ++i) {
        auto index = selectItems.at(i);
        auto item = knowTreeModel->getItem(index);
        branchesName << item->getField("name");
    }

    // Перебираются ветки, которые нужно удалить, и в них проверяется наличие заблокированных записей
    bool isSelectionContainBlockRecords = false;
    for (int i = 0; i < selectItems.size(); ++i) {
        auto index = selectItems.at(i);
        auto item = knowTreeModel->getItem(index);

        if (knowTreeModel->isItemContainsBlockRecords(item)) {
            isSelectionContainBlockRecords = true;
            break;
        }
    }

    // Если есть записи, помеченные как заблокированные
    if (isSelectionContainBlockRecords) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Confirmation request")); // Запрос подтверждения
        messageBox.setText(tr("In the selected item has been found blocked notes. Do you really want to delete one?"));
        messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        messageBox.setDefaultButton(QMessageBox::Cancel);

        int ret = messageBox.exec();

        if (ret == QMessageBox::Cancel) {
            // Разблокируется главное окно
            MainWindow::get().setEnabled(true);
            MainWindow::get().blockSignals(false);
            return;
        }
    }

    // Создается окно с вопросом, нужно удалять ветки или нет
    QString title, text, del_button;
    bool enable_question = true;
    if (mode == "delete") {
        title = tr("Delete item(s)");
        text = tr("Are you sure you wish to delete item(s) <b>") + branchesName.join(", ") + tr("</b> and all sub items?");
        del_button = tr("Delete");

        enable_question = true;
    } else if (mode == "cut") {
        title = tr("Cut item");
        text = tr("Are you sure you wish to cut item <b>") + branchesName.join(", ") + tr("</b> and all sub items?");
        del_button = tr("Cut");

        enable_question = AppConfig::get().get_cutbranchconfirm();
    }

    bool enable_del = true;
    if (enable_question) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(title);
        messageBox.setText(text);
        messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        QAbstractButton *deleteButton = messageBox.addButton(del_button, QMessageBox::AcceptRole);
        messageBox.exec();

        enable_del = (messageBox.clickedButton() == deleteButton);
    }

    // Если удаление подтверждено
    if (enable_del) {
        // Сохраняется текст в окне редактирования
        // Нужно, чтобы нормально удалилась текущая редактируемая запись,
        // если она находится в удаляемой ветке
        MainWindow::get().saveTextarea();

        knowTreeModel->deleteItemsByModelIndexList(selectItems);

        qDebug() << "Delete finish";

        // Сохранение дерева веток
        find_object<TreeScreen>("treeScreen")->saveKnowTree();

        qDebug() << "Save new tree finish";
    }

    // Разблокируется главное окно
    MainWindow::get().setEnabled(true);
    MainWindow::get().blockSignals(false);

    treeEmptyControl();
}

void TreeScreen::cutBranch() {
    if (copyBranch())
        delBranch("cut");
}

bool TreeScreen::copyBranch() {
    qDebug() << "In copy_branch()";

    // Сохраняется текст в окне редактирования
    MainWindow::get().saveTextarea();

    // Получение списка индексов QModelIndex выделенных элементов
    auto selectitems = knowTreeView->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки
    if (selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("Please select a single item for copy."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return false;
    }

    // Получение индекса выделенной ветки
    auto index = getCurrentItemIndex();

    // Получение ссылки на узел, который соответствует выделенной ветке
    auto item = knowTreeModel->getItem(index);

    // Получение пути к выделенной ветке
    auto path = item->getPath();

    // Получение путей ко всем подветкам
    const auto subbranchespath = item->getAllChildrenPath();

    // -------------------
    // Копирование в буфер
    // -------------------

    qDebug() << "Tree item copy to buffer";

    // Создается ссылка на буфер обмена
    auto cbuf = QApplication::clipboard();

    // Данные в буфере обмена очищаются
    cbuf->clear();

    // Создается объект с данными для заполнения буфера обмена
    auto branch_clipboard_data = new ClipboardBranch();

    // Добавление корневой ветки
    addBranchToClipboard(branch_clipboard_data, path, true);

    // Добавление прочих веток
    for (auto & curr_path : subbranchespath)
        addBranchToClipboard(branch_clipboard_data, curr_path, false);

    // branch_clipboard_data->print();

    // Объект с ветками помещается в буфер обмена, владение указателем передается
    // глобальному объекту буфера обмена, поэтому утечки нет
    cbuf->setMimeData(branch_clipboard_data);

    return true;
}

// Вспомогательная функция при копировании ветки в буфер
void TreeScreen::addBranchToClipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root) {
    TreeItem *curr_item;
    QMap<QString, QString> curr_item_fields;
    QString branch_id;
    RecordTableData *curr_item_record_table;

    // Добавление ветки
    curr_item = knowTreeModel->getItem(path);
    curr_item_fields = curr_item->getAllFields(); // Раньше было getAllFieldsDirect()
    branch_id = curr_item_fields["id"];
    if (is_root)
        branch_clipboard_data->addBranch("-1", curr_item_fields);
    else
        branch_clipboard_data->addBranch(curr_item->getParentId(), curr_item_fields);

    // Добавление конечных записей
    curr_item_record_table = curr_item->recordtableGetTableData();
    for (unsigned int i = 0; i < curr_item_record_table->size(); i++) {
        // Полный образ записи (с файлами и текстом)
        auto record = curr_item_record_table->getRecordFat(i);

        branch_clipboard_data->addRecord(branch_id, record);
    }
}

// Вставка ветки из буфера на том же уровне, что и выбранная
void TreeScreen::pasteBranch() {
    qDebug() << "In paste_branch";

    pasteBranchSmart(true);
}

// Вставка ветки из буфера в виде подветки выбранной ветки
void TreeScreen::pasteSubbranch() {
    qDebug() << "In paste_subbranch";

    pasteBranchSmart(false);
}

void TreeScreen::pasteBranchSmart(bool is_branch) {
    // Проверяется, содержит ли буфер обмена данные нужного формата
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    if (mimeData == nullptr)
        return;

    if (!(mimeData->hasFormat(FixedParameters::appTextId + "/branch")))
        return;

    // Получение списка индексов QModelIndex выделенных элементов
    auto selectitems = knowTreeView->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки или вообще ветка не выбрана
    if (selectitems.size() != 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling paste operation."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return;
    }

    // Блокируется главное окно, чтобы при продолжительном выполнении
    // не было возможности сделать другие действия
    MainWindow::get().setDisabled(true);

    // Создается ссылка на буфер обмена
    auto cbuf = QApplication::clipboard();

    // Извлечение объекта из буфера обмена
    const ClipboardBranch *branch = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());
    branch->print();
    branch->printIdTree();

    // Получение индекса выделенной строки дерева
    auto index = this->getCurrentItemIndex();

    // Добавление ветки
    QString pasted_branch_id;
    if (is_branch)
        pasted_branch_id = knowTreeModel->pasteNewSiblingBranch(index, (ClipboardBranch *)branch);
    else
        pasted_branch_id = knowTreeModel->pasteNewChildBranch(index, (ClipboardBranch *)branch);

    // Установка курсора на новую созданную ветку
    auto pasted_branch_item = knowTreeModel->getItemById(pasted_branch_id);
    auto pasted_branch_path = pasted_branch_item->getPath();
    MainWindow::get().setTreePosition(pasted_branch_path);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();

    // Разблокируется главное окно
    MainWindow::get().setEnabled(true);
}

// Установка иконки для ветки
void TreeScreen::setIcon() {
    QString startDirectory = AppConfig::get().get_tetradir() + "/" + FixedParameters::iconsRelatedDirectory;
    qDebug() << "Set start directory for select icon: " << startDirectory;

    // Создается окно выбора файла иконки
    IconSelectDialog iconSelectDialog;
    iconSelectDialog.setDefaultSection(AppConfig::get().getIconCurrentSectionName());
    iconSelectDialog.setPath(startDirectory);

    int result = iconSelectDialog.exec();

    if (result == QDialog::Accepted && iconSelectDialog.getSelectFileName().isEmpty()) {
        showMessageBox(tr("No icon selected."));                                               // Сообщение "Вы не выбрали иконку"
    } else if (result == QDialog::Accepted && !iconSelectDialog.getSelectFileName().isEmpty()) // Если был выбран файл иконки и нажат Ok
    {
        QString fullIconFileName = iconSelectDialog.getSelectFileName();
        QFileInfo iconFileInfo(fullIconFileName);
        QString iconFileName = iconFileInfo.fileName();
        QString iconDir = iconFileInfo.dir().dirName();
        QString relatedFileName = "/" + iconDir + "/" + iconFileName;

        auto currentItem = knowTreeModel->getItem(getCurrentItemIndex());

        currentItem->setField("icon", relatedFileName);

        // Обновляеются на экране ветка и ее подветки
        updateBranchOnScreen(getCurrentItemIndex());

        // Записывается дерево
        saveKnowTree();
    } else if (result == IconSelectDialog::RemoveIconCode) // Если было выбрано действие убирание иконки, назначенной для ветки
    {
        TreeItem *currentItem = knowTreeModel->getItem(getCurrentItemIndex());
        currentItem->setField("icon", "");

        // Обновляеются на экране ветка и ее подветки
        updateBranchOnScreen(getCurrentItemIndex());

        // Записывается дерево
        saveKnowTree();
    }

    // Если текущая секция изменилась, ее имя запоминается чтобы в последующем открывать виджет с этой секцией
    if (iconSelectDialog.getCurrentSection() != "" &&
        iconSelectDialog.getCurrentSection() != AppConfig::get().getIconCurrentSectionName())
        AppConfig::get().setIconCurrentSectionName(iconSelectDialog.getCurrentSection());
}

void TreeScreen::exportBranchToDirectory(QString exportDir) {
    // Проверка, является ли выбранная директория пустой. Выгрузка возможна только в полностью пустую директорию
    if (!QDir(exportDir).isEmpty()) {
        showMessageBox(tr("Directory %1 is not empty. Please, select an empty export directory.").arg(exportDir));
        return;
    }

    // Текущая выбранная ветка будет экспортироваться
    if (!getCurrentItemIndex().isValid()) {
        showMessageBox(tr("No export tree item selected. Please select a item."));
        return;
    }

    auto startItem = knowTreeModel->getItem(getCurrentItemIndex());

    // Экспорт данных
    bool result = knowTreeModel->exportBranchToDirectory(startItem, exportDir);

    if (result)
        showMessageBox(tr("Done exporting into <b>%1</b>.").arg(exportDir));
    else
        showMessageBox(tr("Errors occurred while exporting."));
}

void TreeScreen::importBranchFromDirectory(QString importDir) {
    // Импорт будет идти в текущую выбранную ветку
    if (!getCurrentItemIndex().isValid()) {
        showMessageBox(tr("No tree item selected for importing. Please select a item."));
        return;
    }

    auto startItem = knowTreeModel->getItem(getCurrentItemIndex());

    // Импорт данных
    auto importNodeId = knowTreeModel->importBranchFromDirectory(startItem, importDir);

    // Если импорт данных был успешным
    if (importNodeId.size() > 0)
        setCursorToId(importNodeId); // Курсор устанавливается на только что импортированную ветку

    showMessageBox(tr("Item importing finished."));
}

/// @brief Обновление на экране ветки и подветок
void TreeScreen::updateBranchOnScreen(const QModelIndex &index) {
    // Для корневой ветки дается команда чтобы модель сообщила о своем изменении
    knowTreeModel->emitSignalDataChanged(index);

    // По модельному индексу выясняется указатель на ветку
    auto item = knowTreeModel->getItem(index);

    // Перебираются подветки
    const auto updatePathts = item->getAllChildrenPath();
    for (auto & currentPath : updatePathts) {
        auto currentItem = knowTreeModel->getItem(currentPath);
        auto currentIndex = knowTreeModel->getIndexByItem(currentItem);

        // Для подветки дается команда чтобы модель сообщила о своем изменении
        knowTreeModel->emitSignalDataChanged(currentIndex);
    }
}

// Действия при клике на ветку дерева через selection-модель
void TreeScreen::onKnowtreeClicked(const QModelIndex &index) {
    // Данный слот может повторно вызываться, когда его работа еще не завершена,
    // например в момент завершения синхронизации. Это возможно, так как
    // в данном слоте может быть вызван диалог запроса пароля, а диалог
    // может ожидать ввода пользователя неограниченное время.
    // Переменная isThisSlotWork блокирует работу слота, ели он
    // повторно вызван когда он еще не закончил работу
    static bool isThisSlotWork = false;
    if (isThisSlotWork) {
        return;
    } else {
        isThisSlotWork = true; // Следить чтобы перед каждым return данный флаг сбрасывался
    }

    // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

    // Сохраняется текст в окне редактирования в соответсвующий файл
    MainWindow::get().saveTextarea();

    // Получаем указатель на текущую выбранную ветку дерева
    auto item = knowTreeModel->getItem(index);

    // Все инструменты по работе с записями выключаются
    find_object<RecordTableScreen>("recordTableScreen")->disableAllActions();

    // Вначале все инструменты работы с веткой включаются
    for(auto a : actions)
        a->setEnabled(true);

    // Получаем указатель на данные таблицы конечных записей
    auto rtdata = item->recordtableGetTableData();

    // Устанавливаем данные таблицы конечных записей
    find_object<RecordTableController>("recordTableController")->setTableData(rtdata);

    // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
    knowTreeView->resizeColumnToContents(0);

    isThisSlotWork = false;
}

void TreeScreen::updateSelectedBranch() {
    // Получение списка выделенных Item-элементов
    auto selectitems = knowTreeView->selectionModel()->selectedIndexes();

    // Обновление на экране
    for (int i = 0; i < selectitems.size(); ++i)
        knowTreeView->update(selectitems.at(i));
}

QItemSelectionModel *TreeScreen::getSelectionModel() {
    return knowTreeView->selectionModel();
}

void TreeScreen::setCursorToIndex(QModelIndex index) {
    // Если индекс невалидный
    if (!index.isValid()) {
        qDebug() << "Try set cursor to bad index. Set cursor disabled.";
        return;
    }

    // Курсор устанавливается на нужный элемент дерева
    // В desktop-варианте на сигнал currentRowChanged() будет вызван слот on_knowtree_clicked()
    knowTreeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
}

void TreeScreen::setCursorToId(QString nodeId) {
    auto item = knowTreeModel->getItemById(nodeId);
    auto index = knowTreeModel->getIndexByItem(item);

    setCursorToIndex(index);
}

// Получение номера первого выделенного элемента
int TreeScreen::getFirstSelectedItemIndex() {
    // Получение списка выделенных Item-элементов
    auto selectitems = knowTreeView->selectionModel()->selectedIndexes();

    if (selectitems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectitems.at(0)).row(); // Индекс первого выделенного элемента
}

// Получение индекса текущего элемента на котором стоит курсор
QModelIndex TreeScreen::getCurrentItemIndex() {
    return knowTreeView->selectionModel()->currentIndex();
}

// Метод, следящий, не обнулилось ли дерево
// Если дерево стало пустым, данный метод добавит одну ветку
void TreeScreen::treeEmptyControl() {
    qDebug() << "treescreen::tree_empty_control() : Tree item count " << knowTreeModel->rowCount();

    if (knowTreeModel->rowCount() == 0) {
        qDebug() << "treescreen::tree_empty_control() : Tree empty, create blank item";

        insBranchProcess(QModelIndex(), tr("Rename me"), false);
    }
}

// Сохранение дерева веток на диск
void TreeScreen::saveKnowTree() {
    knowTreeModel->save();

    updateLastKnowTreeData(QFileInfo(), false);
}

// Перечитывание дерева веток с диска
// Метод возвращает true, если обнаружено что данные были изменены и перечитаны
bool TreeScreen::reloadKnowTree() {
    // Если по каким-то причинам нет данных о файле сохраненного дерева
    if (lastKnowTreeModifyDateTime.isValid() == false || lastKnowTreeSize == 0) {
        knowTreeModel->reload();
        updateLastKnowTreeData(QFileInfo(), false);
        qDebug() << "Reload XML data if last data not found";
        return true;
    }

    // Если сохраненные ранее данные отличаются от текущих
    QFileInfo fileInfo(knowTreeModel->getXmlFileName());
    if (fileInfo.lastModified() != lastKnowTreeModifyDateTime || fileInfo.size() != lastKnowTreeSize) {
        knowTreeModel->reload();
        updateLastKnowTreeData(fileInfo, true);
        qDebug() << "Reload XML data";
        return true;
    }

    qDebug() << "Skip reload XML data";
    return false;
}

void TreeScreen::updateLastKnowTreeData(QFileInfo fileInfo, bool fileInfoValid) {
    if (fileInfoValid == false)
        fileInfo = QFileInfo(knowTreeModel->getXmlFileName());

    lastKnowTreeModifyDateTime = fileInfo.lastModified();
    lastKnowTreeSize = fileInfo.size();
}

// Установка фокуса на базовый виджет (на список веток дерева)
void TreeScreen::setFocusToBaseWidget() {
    knowTreeView->setFocus();
}
