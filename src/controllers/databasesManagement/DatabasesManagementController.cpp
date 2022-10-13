#include <QAbstractButton>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QModelIndexList>
#include <QObject>
#include <algorithm>

#include "DatabasesManagementController.h"
#include "qclipboard.h"
#include "views/databasesManagement/DatabasesManagementTable.h"

DatabasesManagementController::DatabasesManagementController(QObject *parent) : QObject(parent) {
    // Создается область со списком записей лога
    view = new DatabasesManagementTable(qobject_cast<QWidget *>(parent)); // Вид размещается внутри виджета Screen
    view->setObjectName("DatabasesManagementTable");
    view->setController(this);

    // Создание модели данных
    model = new DatabasesManagementModel(this);
    model->setObjectName("DatabasesManagementModel");

    // Модель данных задается для вида
    view->setModel(model);
}

DatabasesManagementController::~DatabasesManagementController() {
    delete view;
    delete model;
}

DatabasesManagementTable *DatabasesManagementController::getView(void) {
    return view;
}

void DatabasesManagementController::onSelectClicked() {
    QModelIndexList indexList = view->selectionModel()->selectedRows();

    if (indexList.size() != 1) {
        return;
    }

    int row = indexList[0].row();

    // Установка пометки выбора базы данных
    model->selectDatabase(row);

    // Выбор текущей строки
    const QModelIndex index = model->index(row, 0);
    view->setCurrentIndex(index);
}

void DatabasesManagementController::onCreateClicked() {
}

/// @brief Добавление существующей базы
void DatabasesManagementController::onAddClicked() {
    QString dbPath;

    QString title = tr("Append exists database");

    // Диалог, поясняющий что нужно выбрать директорию с существующей БД
    QMessageBox firstBox;
    firstBox.setWindowTitle(title);
    firstBox.setText(tr("<b>First step:</b> select database directiory"));
    firstBox.setInformativeText(tr("For database append please select\ndirectory with exists database.\nThis directory should contain the file mytetra.xml."));
    firstBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    firstBox.setDefaultButton(QMessageBox::Ok);
    firstBox.button(QMessageBox::Ok)->setText(tr("I understand"));
    firstBox.setIconPixmap(QIcon(":/resource/pic/dbmanagement_db_icon.svg").pixmap(QSize(32, 32)));
    if (firstBox.exec() != QMessageBox::Ok) {
        return;
    }

    // Диалог выбора директории с существующей БД
    QFileDialog tetradirSelectDialog;
    tetradirSelectDialog.setFileMode(QFileDialog::Directory);
    tetradirSelectDialog.setWindowTitle(tr("Select directory with exists database"));
    tetradirSelectDialog.setDirectory("");
    if (tetradirSelectDialog.exec() == QDialog::Accepted) {
        if (!tetradirSelectDialog.directory().absolutePath().isEmpty()) {
            // Запоминается выбранный пользователем путь
            dbPath = tetradirSelectDialog.directory().absolutePath();

            // Выход, если в директории нет файла mytetra.xml
            QFileInfo checkFile(dbPath + "/mytetra.xml");
            if (!(checkFile.exists() && checkFile.isFile())) {
                QMessageBox msgBox;
                msgBox.setText(tr("Can not find file mytetra.xml in this directory"));
                msgBox.exec();
                return;
            }
        } else {
            return; // Выход, если был установлен пустой путь
        }
    } else {
        return; // Выход, если была нажата отмена в диалоге выбора директории
    }

    // Если добавляемая директория уже есть в списке баз данных
    if (model->isDbPathExists(dbPath)) {
        QMessageBox msgBox;
        msgBox.setText(tr("This database directory already using in databses list"));
        msgBox.exec();
        return;
    }

    model->addDatabaseByUser(dbPath);
}

void DatabasesManagementController::onCopyClicked() {
    // Перечень индексов ячеек, которые были выбраны
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();

    if (indexes.size() < 1)
        return;

    // Индексы выбранных ячеек идут не так как на экране, поэтому их нужно отсортировать
    std::sort(indexes.begin(), indexes.end());

    // Размещаемый в буфере обмена текст
    QString selectedText;

    for (int i = 0; i < indexes.size(); ++i) {
        QVariant data = model->data(indexes.at(i));
        QString text = data.toString();

        // Добавляется текст из ячейки
        selectedText.append(text);

        // Если не последняя ячейка
        if (i < indexes.size() - 1) {
            if (indexes.at(i).row() == indexes.at(i + 1).row()) // Если текущая ячейка на той же строке что и последующая ячейка
                selectedText.append('\t');
            else // Иначе последующая ячейка на другой строке
                selectedText.append('\n');
        }
    }

    QApplication::clipboard()->setText(selectedText);
}
