#include <QBoxLayout>
#include <QByteArray>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QSplitter>
#include <QTextDocument>
#include <QToolButton>
#include <QtGlobal>

#include "FindScreen.h"
#include "FindTableWidget.h"
#include "libraries/helpers/ObjectHelper.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/KnowTreeModel.h"
#include "models/tree/TreeItem.h"
#include "views/mainWindow/MainWindow.h"
#include "views/tree/KnowTreeView.h"
#include "views/tree/TreeScreen.h"

FindScreen::FindScreen(QWidget *parent) : QWidget(parent) {
    setupFindTextAndButton();
    assemblyFindTextAndButton();

    setupCloseButton();
    assemblyCloseButton();

    setupComboOption();
    assemblyComboOption();

    setupWhereFindLine();
    assemblyWhereFindLine();

    setupUI();
    assembly();

    setupSignals();
}

// Текст поиска и кнопка "Поиск"
void FindScreen::setupFindTextAndButton(void) {
    // Поле текста для поиска
    findText = new QLineEdit();

    // Кнопка "Поиск"
    findStartButton = new QPushButton(this);
    findStartButton->setText(tr("Find"));
    findStartButton->setDefault(true);
    findStartButton->setEnabled(false);

    // Кнопка разворачивания инструментов
    toolsExpand = new QToolButton(this);
    toolsExpand->setIcon(QIcon(":/resource/pic/find_in_base_expand_tools.svg"));
    toolsExpand->setEnabled(true);
}

// Текст поиска и кнопка "Поиск"
void FindScreen::assemblyFindTextAndButton(void) {
    toolsAreaFindTextAndButton = new QHBoxLayout();
    toolsAreaFindTextAndButton->addWidget(findText);
    toolsAreaFindTextAndButton->addWidget(findStartButton);
    toolsAreaFindTextAndButton->addWidget(toolsExpand);
}

// Набор опций поиска в виде выпадающих списков
void FindScreen::setupComboOption(void) {
    // Выбор "Любое слово" - "Все слова"
    wordRegard = new QComboBox();
    wordRegard->addItem(QIcon(":/resource/pic/find_in_base_any.svg"), tr("Any word"));
    wordRegard->addItem(QIcon(":/resource/pic/find_in_base_all.svg"), tr("All words"));
    wordRegard->setCurrentIndex(AppConfig::get().get_findscreen_wordregard());

    // Выбор "Только целые слова" - "Подстрока"
    howExtract = new QComboBox();
    howExtract->addItem(QIcon(":/resource/pic/find_in_base_separate.svg"), tr("Whole words"));
    howExtract->addItem(QIcon(":/resource/pic/find_in_base_substring.svg"), tr("Substring"));
    howExtract->setCurrentIndex(AppConfig::get().get_findscreen_howextract());

    // Выбор "Во всей базе" - "В текущей ветке"
    treeSearchArea = new QComboBox();
    treeSearchArea->addItem(QIcon(":/resource/pic/find_in_base_search_all.svg"), tr("Entire base"));          // Вся база
    treeSearchArea->addItem(QIcon(":/resource/pic/find_in_base_search_branch.svg"), tr("Current tree item")); // Текущая ветка
    treeSearchArea->setCurrentIndex(AppConfig::get().getFindScreenTreeSearchArea());
}

// Набор опций поиска в виде выпадающих списков
void FindScreen::assemblyComboOption(void) {
    toolsAreaComboOption = new QHBoxLayout();
    toolsAreaComboOption->addWidget(wordRegard);
    toolsAreaComboOption->addWidget(howExtract);
    toolsAreaComboOption->addWidget(treeSearchArea);
    toolsAreaComboOption->addStretch();
}

void FindScreen::setupCloseButton(void) {
    // Кнопка закрытия виджета
    closeButton = new QToolButton(this);
    closeButton->setVisible(true);
    closeButton->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton)); // SP_TitleBarCloseButton SP_DialogCloseButton

    int w = closeButton->geometry().width();
    int h = closeButton->geometry().height();
    int x = qMin(w, h) / 2;
    closeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
    closeButton->setMinimumSize(x, x);
    closeButton->setMaximumSize(x, x);
    closeButton->resize(x, x);
}

void FindScreen::assemblyCloseButton(void) {
    // Вертикальная область с кнопкой закрытия и распоркой
    // чтобы кнопка была вверху
    toolsAreaCloseButton = new QVBoxLayout();
    toolsAreaCloseButton->setContentsMargins(0, 0, 0, 0);
    toolsAreaCloseButton->addWidget(closeButton);
    toolsAreaCloseButton->addStretch();
}

void FindScreen::setupWhereFindLine(void) {
    whereFindLabel = new QLabel(tr("Find in: "));
    auto &cfg = AppConfig::get();
    findInName = new QCheckBox(tr("Title"));
    findInName->setChecked(cfg.get_findscreen_find_in_field("name"));

    findInAuthor = new QCheckBox(tr("Author(s)"));
    findInAuthor->setChecked(cfg.get_findscreen_find_in_field("author"));

    findInUrl = new QCheckBox(tr("Url"));
    findInUrl->setChecked(cfg.get_findscreen_find_in_field("url"));

    findInTags = new QCheckBox(tr("Tags"));
    findInTags->setChecked(cfg.get_findscreen_find_in_field("tags"));

    findInText = new QCheckBox(tr("Text"));
    findInText->setChecked(cfg.get_findscreen_find_in_field("text"));

    findInNameItem = new QCheckBox(tr("Name tree item"));
    findInNameItem->setChecked(cfg.get_findscreen_find_in_field("nameItem"));
}

void FindScreen::assemblyWhereFindLine(void) {
    whereFindLine = new QHBoxLayout();

    whereFindLine->addWidget(whereFindLabel);

    whereFindLine->addWidget(findInName);
    whereFindLine->addWidget(findInAuthor);
    whereFindLine->addWidget(findInUrl);
    whereFindLine->addWidget(findInTags);
    whereFindLine->addWidget(findInText);
    whereFindLine->addWidget(findInNameItem);

    whereFindLine->addStretch();

    whereFindLine->setContentsMargins(3, 0, 0, 0); // Устанавливаются границы
}

void FindScreen::setupSignals(void) {
    // При каждом изменении текста в строке запроса
    connect(findText, &QLineEdit::textChanged, this, &FindScreen::enableFindButton);

    // При каждом изменении текста извне может вырабатыватся этот сигнал
    // Он вырабатывается в слоте setFindText()
    connect(this, &FindScreen::textChangedFromAnother, this, &FindScreen::enableFindButton);

    // При нажатии Enter в строке запроса
    connect(findText, &QLineEdit::returnPressed, this, &FindScreen::findClicked);

    // При нажатии кнопки Find
    connect(findStartButton, &QPushButton::clicked, this, &FindScreen::findClicked);

    // При нажатии кнопки разворачивания/сворачивания инструментов поиска
    connect(toolsExpand, &QToolButton::clicked, this, &FindScreen::toolsExpandClicked);

    // После установки текста извне, вырабатывается этот сигнал
    connect(this, &FindScreen::findClickedAfterAnotherTextChanged, this, &FindScreen::findClicked);

    // При нажатии кнопки закрытия
    connect(closeButton, &QToolButton::clicked, this, &FindScreen::widgetHide);

    // Сигналы для запоминания состояния интерфейса
    connect(wordRegard, qOverload<int>(&QComboBox::currentIndexChanged), this, &FindScreen::changedWordRegard);

    connect(howExtract, qOverload<int>(&QComboBox::currentIndexChanged), this, &FindScreen::changedHowExtract);
    connect(treeSearchArea, qOverload<int>(&QComboBox::currentIndexChanged), this, &FindScreen::changedTreeSearchArea);
    connect(findInName, &QCheckBox::checkStateChanged, this, &FindScreen::changedFindInName);
    connect(findInAuthor, &QCheckBox::checkStateChanged, this, &FindScreen::changedFindInAuthor);
    connect(findInUrl, &QCheckBox::checkStateChanged, this, &FindScreen::changedFindInUrl);
    connect(findInTags, &QCheckBox::checkStateChanged, this, &FindScreen::changedFindInTags);
    connect(findInText, &QCheckBox::checkStateChanged, this, &FindScreen::changedFindInText);
    connect(findInNameItem, &QCheckBox::checkStateChanged, this, &FindScreen::changedFindInNameItem);
}

void FindScreen::setupUI(void) {
    findTable = new FindTableWidget();
}

void FindScreen::assembly(void) {
    centralDesktopLayout = new QVBoxLayout();

    toolsLine = new QHBoxLayout();
    toolsLine->addLayout(toolsAreaFindTextAndButton);
    toolsLine->addLayout(toolsAreaComboOption);
    toolsLine->addLayout(toolsAreaCloseButton);

    centralDesktopLayout->addLayout(toolsLine);

    centralDesktopLayout->addLayout(whereFindLine);
    centralDesktopLayout->addWidget(findTable, 10);
    centralDesktopLayout->setContentsMargins(0, 0, 0, 0); // Границы убираются
    centralDesktopLayout->setSizeConstraint(QLayout::SetNoConstraint);

    this->setLayout(centralDesktopLayout);

    switchToolsExpand(AppConfig::get().getFindInBaseExpand());
}

void FindScreen::enableFindButton(const QString &text) {
    findStartButton->setEnabled(!text.isEmpty());
}

/// @brief Слот, с помощью которого другие виджеты могут устанавливать текст для поиска
void FindScreen::setFindText(QString text) {
    findText->setText(text);

    emit textChangedFromAnother(text);
    emit findClickedAfterAnotherTextChanged();
}

/// @brief Слот, срабатывающий при нажатии на кнопку начала поиска
void FindScreen::findClicked(void) {
    // Поля, где нужно искать (Заголовок, текст, теги...)
    if(findInName->isChecked())     scanFields << "name";
    if(findInAuthor->isChecked())   scanFields << "author";
    if(findInUrl->isChecked())      scanFields << "url";
    if(findInTags->isChecked())     scanFields << "tags";
    if(findInText->isChecked())     scanFields << "text";
    if(findInNameItem->isChecked()) scanFields << "nameItem"; // Поиск по именам веток

    // Если не отмечены поля для поиска
    if (scanFields.empty()) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Cannot start find process"));
        messageBox.setText(tr("Verify that you selected fields for search for starting find process."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return;
    }

    // Выясняется список слов, которые нужно искать
    searchWordList = textDelimiterDecompose(findText->text());

    if (searchWordList.empty()) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Can not start find process"));
        messageBox.setText(tr("The search request is too short. Enter at least one word."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return;
    }

    findStart();
}

void FindScreen::findStart(void) {
    // Сохраняется текущая редактируемая запись, чтобы и в ней
    // были найдены введенные перед нажатием Find данные, если они есть
    MainWindow::get().saveTextarea();

    // Очищается таблица результата поиска
    findTable->clearAll();

    // Очищается надпись поверх результата поиска
    findTable->setOverdrawMessage("");

    // Выясняется ссылка на модель дерева данных
    KnowTreeModel *searchModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>("knowTreeView")->model());

    // Выясняется стартовый элемент в дереве, с которого будет начат поиск
    // Выясняется сколько всего конечных записей
    TreeItem *startItem = 0;
    int totalRec = 0;
    if (AppConfig::get().getFindScreenTreeSearchArea() == 0) {
        // поиск во всем дереве
        // Корневой элемент дерева
        startItem = searchModel->rootItem;

        // Количество конечных записей во всем дереве
        totalRec = searchModel->getAllRecordCount();
    } else if (AppConfig::get().getFindScreenTreeSearchArea() == 1) {
        // поиск в текущей ветке

        // Индекс текущей выбранной ветки
        QModelIndex currentItemIndex = find_object<TreeScreen>("treeScreen")->getCurrentItemIndex();

        // Текущая ветка
        startItem = searchModel->getItem(currentItemIndex);

        // Количество конечных записей в текущей ветке и всех подветках
        totalRec = searchModel->getRecordCountForItem(startItem);
    }

    qDebug() << "Start finding in " << totalRec << " records";

    // Если стартовый элемент не был установлен
    if (startItem == 0) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Cannot start find process"));
        messageBox.setText(tr("Starting position for a tree searching is not set."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return;
    }

    // Динамически создается виджет линейки наполяемости
    progress = new QProgressDialog(this);

    // Корректировка регрессии в Qt 5.5 - 5.6.x QTBUG-47042 QTBUG-47049
    // QProgressDialog показывает сам себя через 4 секунды (точнее, спустя minimumDuration() сек.) после отработки конструктора
    // метод hide() не может скрыть такое окно
    /// @todo: recheck
    progress->cancel();

    // Показывается виджет линейки наполняемости
    progress->reset();
    progress->setLabelText(tr("Search..."));
    progress->setRange(0, totalRec);
    progress->setModal(true);
    progress->setMinimumDuration(0);
    progress->show(); /// @todo: recheck Эта команда под вопросом, возможно она не нужна

    // Обнуляется счетчик обработанных конечных записей
    totalProgressCounter = 0;
    cancelFlag = false;

    //Вызывается рекурсивный поиск в дереве
    findRecurse(startItem);

    // После вставки всех данных подгоняется ширина колонок
    findTable->updateColumnsWidth();

    // Линейка наполяемости удаляется
    delete progress;

    // Если ничего не было найдено
    if (findTable->getRowCount() == 0) {
        // если поиск проводился не во всей базе
        if (AppConfig::get().getFindScreenTreeSearchArea() != 0)
            findTable->setOverdrawMessage(tr("No search results.\nHint: search produced in current tree item.\nTry to search for entire database."));
    }
}

void FindScreen::findRecurse(TreeItem *curritem) {
    // Если была нажата отмена поиска
    if (cancelFlag)
        return;

    // Проверка имени ветки
    if (scanFields.contains("nameItem")) {
        auto itemName = curritem->getField("name");
        if (findInTextProcess(itemName)) {
            // В таблицу результатов добавляется запись о найденой ветке
            findTable->addRow(itemName, tr("[Tree item]"), "", curritem->getPath(), curritem->getField("id"));
        }
    }

    // Выясняется ссылка на таблицу конечных записей
    auto searchRecordTable = curritem->recordtableGetTableData();

    // Перебираются записи таблицы
    for (int i = 0; i < static_cast<int>(searchRecordTable->size()); ++i) {
        // Обновляется линейка наполняемости
        progress->setValue(++totalProgressCounter);
        qApp->processEvents();
        if (progress->wasCanceled()) {
            cancelFlag = true;
            return;
        }

        bool found = false;
        for (auto & f : std::as_const(scanFields)) {
            if(f == "nameItem") {
                continue; // search by branch name. Alreary processed upper; have no sense, when iterating over records.
            } else if (f == "text") {
                // Поиск в обычном поле
                auto inspectText = searchRecordTable->getText(i);
                QTextDocument textdoc;
                textdoc.setHtml(inspectText);
                found = findInTextProcess(textdoc.toPlainText());
            } else {
                auto inspectText = searchRecordTable->getField(f, i);
                found = findInTextProcess(inspectText);
            }
            if(found)
                break;
        }

        // Если запись найдена
        if (found) {
            qDebug() << "Find succesfull in " << searchRecordTable->getField("name", i);
            findTable->addRow(searchRecordTable->getField("name", i),
                              curritem->getField("name"),
                              searchRecordTable->getField("tags", i),
                              curritem->getPath(),
                              searchRecordTable->getField("id", i));
        }

    } // Закрылся цикл перебора записей

    // Рекурсивная обработка каждой подчиненной ветки
    for (int i = 0; i < curritem->childCount(); i++)
        findRecurse(curritem->child(i));
}

// Поиск в переданном тексте
// Учитываются состояния переключателей wordregard и howextract
bool FindScreen::findInTextProcess(const QString &text) {
    int findWordCount = 0;
    int findFlag = 0;

    // Перебираются искомые слова
    for (int i = 0; i < searchWordList.size(); ++i) {
        findFlag = 0;

        // Если надо найти совпадение целого слова
        if (howExtract->currentIndex() == 0) {
            // Текст разбивается на слова с очисткой от лишних знаков
            // и проверяется, есть ли в полученном списке текущее слово
            if (textDelimiterDecompose(text).contains(searchWordList.at(i), Qt::CaseInsensitive))
                findFlag = 1;
        } else {
            // Если надо найти слово как подстроку
            if (text.contains(searchWordList.at(i), Qt::CaseInsensitive))
                findFlag = 1;
        }

        // Если слово было найдено, количество найденных слов увеличивается
        if (findFlag == 1)
            findWordCount++;

        // Если ищется хотя бы одно совпадение
        if (findFlag == 1 && wordRegard->currentIndex() == 0)
            return true; // То при первом же совпадении цикл прекращается
    }

    // Искалось хотя бы одно совпадение, но не было найдено
    if (wordRegard->currentIndex() == 0)
        return false;
    else {
        // Иначе требовалось найти все слова в запросе
        if (findWordCount == searchWordList.size())
            return true;
        else
            return false;
    }
}

void FindScreen::changedWordRegard(int pos) {
    AppConfig::get().set_findscreen_wordregard(pos);
}

void FindScreen::changedHowExtract(int pos) {
    AppConfig::get().set_findscreen_howextract(pos);
}

void FindScreen::changedTreeSearchArea(int pos) {
    AppConfig::get().setFindScreenTreeSearchArea(pos);
}

void FindScreen::changedFindInField(QString fieldname, int state) {
    AppConfig::get().set_findscreen_find_in_field(fieldname, state == Qt::Checked);
}

void FindScreen::widgetShow(void) {
    AppConfig::get().set_findscreen_show(true);
    this->show();

    // При появлении виджета курсор должен сразу стоять на поле ввода
    findText->setFocus();
}

// Полное сокрытие виджета
void FindScreen::widgetHide(void) {
    // Запоминается размер сплиттера перед скрытием виджета
    QSplitter *findSplitterRel = find_object<QSplitter>("findsplitter");
    AppConfig::get().set_findsplitter_size_list(findSplitterRel->sizes());

    // Виджет скрывается
    AppConfig::get().set_findscreen_show(false);
    this->close();
}

// Слот, срабатывающий при клике на кнопку expand
void FindScreen::toolsExpandClicked(void) {
    // Если нужно сомкнуть инструменты
    if (findInName->isVisible()) {
        switchToolsExpand(false);
        AppConfig::get().setFindInBaseExpand(false);
    } else {
        switchToolsExpand(true);
        AppConfig::get().setFindInBaseExpand(true);
    }
}

void FindScreen::switchToolsExpand(bool flag) {
    whereFindLabel->setVisible(flag);

    // Флаги поиска скрываются для любого интерфейса, так как они всегда находятся на отдельной строке
    findInName->setVisible(flag);
    findInAuthor->setVisible(flag);
    findInUrl->setVisible(flag);
    findInTags->setVisible(flag);
    findInText->setVisible(flag);
    findInNameItem->setVisible(flag);
}

QStringList FindScreen::textDelimiterDecompose(QString text) {
    const int len = text.length();

    QStringList list;
    QString buf;
    bool quoted = false;

    QString delimiter = " '.,;:-?!`";
    delimiter.append(QChar::Tabulation);
    delimiter.append(QChar::Nbsp);

    for (int i = 0; i < len; i++) {
        // Если обнаружен разделитель
        if (delimiter.contains(text[i])) {
            if (!quoted) {
                if (buf.length() > 0) {
                    list.append(buf);
                    buf = "";
                }
            } else
                buf += text[i];
        } else if (text[i] == '"')
            quoted = !quoted;
        else
            buf += text[i];
    }

    if (buf.length() > 0)
        list.append(buf);

    qDebug() << "Find split list:" << list;

    return list;
}
