#include <QAction>
#include <QFile>
#include <QKeySequence>
#include <QToolButton>

#include "ShortcutManager.h"

#include "../helpers/DebugHelper.h"

QMap<QString, QKeySequence> ShortcutManager::keyTable[SECTION_TOTAL_COUNT];
QMap<QString, ShortcutManager::Data> ShortcutManager::defaultKeyTable[SECTION_TOTAL_COUNT];

const QStringList ShortcutManager::availableSection = {"note", "tree", "editor", "attach", "misc"}; // order must be as in enum shortcutSections

void ShortcutManager::init(QString confName) {
    configFileName = confName;
    initDefaultKeyTable();
    initKeyTable();
}

void ShortcutManager::initDefaultKeyTable() {
    {
        auto &s = defaultKeyTable[SECTION_NOTE];

        s.insert("addNewToEnd", {QKeySequence("Ctrl+Alt+N"), tr("Add a new note"), tr("")});
        s.insert("addNewBefore", {QKeySequence("Ctrl+Alt+J"), tr("Add a note before"), tr("Add a note before current selected note")});
        s.insert("addNewAfter", {QKeySequence("Ctrl+Alt+M"), tr("Add a note after"), tr("Add a note after current selected note")});
        s.insert("editField", {QKeySequence("Ctrl+Alt+E"), tr("Edit properties"), tr("Edit note properties (name, author, tags...)")});
        s.insert("block", {QKeySequence("Ctrl+Alt+B"), tr("Block/Unblock note"), tr("Block or unblock current selected note")});
        s.insert("delete", {QKeySequence("Ctrl+Alt+R"), tr("Delete note(s)"), tr("")});
        s.insert("cut", {QKeySequence("Ctrl+Alt+X"), tr("Cut notes(s)"), tr("Cut notes(s) to clipboard")});
        s.insert("copy", {QKeySequence("Ctrl+Alt+C"), tr("Copy note(s)"), tr("Copy note(s) to clipboard")});
        s.insert("paste", {QKeySequence("Ctrl+Alt+V"), tr("Paste note(s)"), tr("Paste note(s) from clipboard")});
        s.insert("moveUp", {QKeySequence("Ctrl+Alt+Up"), tr("Move up"), tr("Move up current note")});
        s.insert("moveDn", {QKeySequence("Ctrl+Alt+Down"), tr("Move down"), tr("Move down current note")});
        s.insert("previousNote", {QKeySequence("Ctrl+Alt+Left"), tr("Previous note"), tr("Previous note has been viewing")});
        s.insert("nextNote", {QKeySequence("Ctrl+Alt+Right"), tr("Next note"), tr("Next note has been viewing")});
    }
    {
        auto &s = defaultKeyTable[SECTION_TREE];
        s.insert("expandAllSubbranch", {QKeySequence("Ctrl+Shift+8"), tr("Expand all sub items"), tr("")});
        s.insert("collapseAllSubbranch", {QKeySequence("Ctrl+Shift+9"), tr("Collapse all sub items"), tr("")});
        s.insert("moveUpBranch", {QKeySequence("Ctrl+Shift+PgUp"), tr("Move item up"), tr("")});
        s.insert("moveDownBranch", {QKeySequence("Ctrl+Shift+PgDown"), tr("Move item down"), tr("")});
        s.insert("insSubbranch", {QKeySequence("Ctrl+F7"), tr("Insert a new sub item"), tr("Insert a new sub item into selected")});
        s.insert("insBranch", {QKeySequence("Ctrl+Shift+F7"), tr("Insert a new sibling item"), tr("Insert a new sibling item after selected")});
        s.insert("editBranch", {QKeySequence("Shift+F6"), tr("Edit item name"), tr("Edit name of selected item")});
        s.insert("delBranch", {QKeySequence("Ctrl+Shift+F8"), tr("Delete item"), tr("Delete selected item and all sub items")});
        s.insert("cutBranch", {QKeySequence("Ctrl+F8"), tr("Cut item"), tr("Cut item including sub items")});
        s.insert("copyBranch", {QKeySequence("Ctrl+Shift+F3"), tr("Copy item"), tr("Copy item including sub items")});
        s.insert("pasteBranch", {QKeySequence("Ctrl+Shift+F5"), tr("Paste item"), tr("Paste sibling item after selected")});
        s.insert("pasteSubbranch", {QKeySequence("Ctrl+F5"), tr("Paste as sub item"), tr("Paste item as sub item for selected")});
        s.insert("setIcon", {QKeySequence("Ctrl+Shift+O"), tr("Set icon"), tr("Set item icon")});
    }
    {
        auto &s = defaultKeyTable[SECTION_EDITOR];
        s.insert("selectAll", {QKeySequence("Ctrl+A"), tr("Select all"), tr("")});
        s.insert("copy", {QKeySequence("Ctrl+C"), tr("Copy"), tr("")});
        s.insert("paste", {QKeySequence("Ctrl+V"), tr("Paste"), tr("")});
        s.insert("pasteAsPlainText", {QKeySequence("Ctrl+W"), tr("Paste plain text"), tr("")});
        s.insert("cut", {QKeySequence("Ctrl+X"), tr("Cut"), tr("")});
        s.insert("undo", {QKeySequence("Ctrl+Z"), tr("Undo"), tr("")});
        s.insert("redo", {QKeySequence("Ctrl+Y"), tr("Redo"), tr("")});

        s.insert("bold", {QKeySequence("Ctrl+B"), tr("Bold"), tr("")});
        s.insert("italic", {QKeySequence("Ctrl+I"), tr("Italic"), tr("")});
        s.insert("underline", {QKeySequence("Ctrl+U"), tr("Underline"), tr("")});
        s.insert("strikeout", {QKeySequence("Ctrl+Shift+S"), tr("Strike out"), tr("")});
        s.insert("superscript", {QKeySequence("Ctrl+Shift+P"), tr("Superscript"), tr("")});
        s.insert("subscript", {QKeySequence("Ctrl+Shift+B"), tr("Subscript"), tr("")});
        s.insert("monospace", {QKeySequence("Ctrl+T"), tr("Monospace"), tr("")});
        s.insert("code", {QKeySequence("Ctrl+M"), tr("Code"), tr("Select a whole paragraphs to format text as code")});
        s.insert("lowercase", {QKeySequence(""), tr("Lowercase"), tr("")});
        s.insert("uppercase", {QKeySequence(""), tr("Uppercase"), tr("")});
        s.insert("clear", {QKeySequence("Ctrl+K"), tr("Clear format"), tr("When selected whole paragraph both text and paragraph format is reset to default or just text format in other case")});
        s.insert("textOnly", {QKeySequence("Ctrl+Shift+K"), tr("Text only"), tr("")});
        s.insert("fixBreakSymbol", {QKeySequence("Ctrl+Shift+R"), tr("Return type replace"), tr("Replace soft carriage return to standard carriage return")});
        s.insert("numericList", {QKeySequence("F12"), tr("Numeric list"), tr("")});
        s.insert("dotList", {QKeySequence("Shift+F12"), tr("Marked list"), tr("")});
        s.insert("indentPlus", {QKeySequence("Ctrl+Alt+I"), tr("Increase indent"), tr("")});
        s.insert("indentMinus", {QKeySequence("Ctrl+Alt+U"), tr("Decrease indent"), tr("")});
        s.insert("alignLeft", {QKeySequence("Ctrl+L"), tr("Align left"), tr("")});
        s.insert("alignCenter", {QKeySequence("Ctrl+E"), tr("Align center"), tr("")});
        s.insert("alignRight", {QKeySequence("Ctrl+R"), tr("Align right"), tr("")});
        s.insert("alignWidth", {QKeySequence("Ctrl+J"), tr("Align width"), tr("")});
        s.insert("fontColor", {QKeySequence("Alt+Shift+C"), tr("Text color"), tr("")});
        s.insert("backgroundColor", {QKeySequence("Alt+Shift+B"), tr("Background color"), tr("")});
        s.insert("fontSelect", {QKeySequence("Alt+F"), tr("Select font"), tr("")});
        s.insert("fontSize", {QKeySequence("Alt+S"), tr("Select font size"), tr("")});
        s.insert("findText", {QKeySequence("Ctrl+F"), tr("Find text"), tr("Find text in current note")});
        s.insert("settings", {QKeySequence("Ctrl+Alt+G"), tr("Editor settings"), tr("")});
        s.insert("reference", {QKeySequence("Ctrl+Shift+U"), tr("Edit reference URL"), tr("")});
        s.insert("showHtml", {QKeySequence("Ctrl+Shift+H"), tr("Edit HTML code"), tr("")});
        s.insert("showFormatting", {QKeySequence("Ctrl+F10"), tr("Show special chars"), tr("")});
        s.insert("createTable", {QKeySequence("Ctrl+F12"), tr("Create a new table"), tr("")});
        s.insert("tableRemoveRow", {QKeySequence(""), tr("Remove row(s)"), tr("")});
        s.insert("tableRemoveCol", {QKeySequence(""), tr("Remove column(s)"), tr("")});
        s.insert("tableAddRow", {QKeySequence(""), tr("Add row(s)"), tr("")});
        s.insert("tableAddCol", {QKeySequence(""), tr("Add column(s)"), tr("")});
        s.insert("tableMergeCells", {QKeySequence(""), tr("Merge cells"), tr("")});
        s.insert("tableSplitCell", {QKeySequence(""), tr("Split cell"), tr("")});
        s.insert("tableProperties", {QKeySequence(""), tr("Table properties"), tr("")});
        s.insert("insertImageFromFile", {QKeySequence("Ctrl+Shift+I"), tr("Insert/edit image"), tr("Insert image from file or edit selected image properties")});
        s.insert("insertHorizontalLine", {QKeySequence("Ctrl+H"), tr("Insert horizontal line"), tr("Insert a horizontal line into the empty paragraph from cursor")});
        s.insert("mathExpression", {QKeySequence("Ctrl+Shift+M"), tr("Insert/edit math expression"), tr("")});
        s.insert("expandEditArea", {QKeySequence("Ctrl+Shift+E"), tr("Expand edit area"), tr("")});
        s.insert("expandToolsLines", {QKeySequence("Ctrl+Shift+L"), tr("Expand tools"), tr("")});
        s.insert("save", {QKeySequence("Ctrl+S"), tr("Forse save note"), tr("")});
        s.insert("showText", {QKeySequence("Ctrl+Shift+W"), tr("Show detached window"), tr("")});
        s.insert("toAttach", {QKeySequence("Ctrl+Shift+A"), tr("Attach files"), tr("")});
        s.insert("gotoReference", {QKeySequence("Alt+Shift+U"), tr("Go to URL or reference"), tr("")}); // Сочетание Alt+U перестает работать после того, как нажато на месте без URL-a
    }
    {
        auto &s = defaultKeyTable[SECTION_ATTACH];
        s.insert("addAttach", {QKeySequence("Ctrl+N"), tr("Attach file"), tr("")});
        s.insert("addAttachFromUrl", {QKeySequence("Ctrl+U"), tr("Attach file from URL"), tr("")});
        s.insert("addLink", {QKeySequence("Ctrl+L"), tr("Add link"), tr("Add link without file copying")});
        s.insert("editFileName", {QKeySequence("Ctrl+E"), tr("Edit file name"), tr("")});
        s.insert("deleteAttach", {QKeySequence("Ctrl+D"), tr("Delete file"), tr("")});
        s.insert("openAttach", {QKeySequence("Ctrl+O"), tr("Preview file"), tr("")});
        s.insert("saveAsAttach", {QKeySequence("Ctrl+S"), tr("Save as..."), tr("")});
        s.insert("showAttachInfo", {QKeySequence("Ctrl+I"), tr("Attach info"), tr("")});
        s.insert("switchToEditor", {QKeySequence("Ctrl+Shift+A"), tr("Return to editor"), tr("")});
    }
    {
        auto &s = defaultKeyTable[SECTION_MISC];
        s.insert("focusTree", {QKeySequence("F7"), tr("Set focus to items tree"), tr("")});
        s.insert("focusNoteTable", {QKeySequence("F6"), tr("Set focus to notes table"), tr("")});
        s.insert("focusEditor", {QKeySequence("F4"), tr("Set focus to editor"), tr("")});
        s.insert("findInBase", {QKeySequence("Ctrl+Shift+F"), tr("Find in base"), tr("")});
        s.insert("synchro", {QKeySequence("F9"), tr("Synchronization"), tr("Run synchronization")});
        s.insert("editConfirm", {QKeySequence(Qt::CTRL | Qt::Key_Return), tr("Ok"), tr("")});
        s.insert("print", {QKeySequence("Ctrl+P"), tr("Print"), tr("")});
        s.insert("exportPdf", {QKeySequence("Ctrl+Shift+D"), tr("Export PDF"), tr("")});
        s.insert("quit", {QKeySequence("Ctrl+Q"), tr("Quit"), tr("")});
    }
}

/// @brief Инициализация таблицы горячих клавиш из конфига
void ShortcutManager::initKeyTable() {
    QSettings config(configFileName, QSettings::IniFormat);

    for (int i = SECTION_NOTE; i < SECTION_TOTAL_COUNT; ++i) {
        keyTable[i].clear();
        // set initial value for each action to default
        for (auto kv = defaultKeyTable[i].constKeyValueBegin(); kv != defaultKeyTable[i].constKeyValueEnd(); ++kv)
            keyTable[i][kv->first] = kv->second.sequence;

        config.beginGroup(availableSection[i]); // Выбирается секция
        for (auto &shortcutName : config.childKeys()) {
            if (!keyTable[i].contains(shortcutName))
                continue; // чтобы в конфиг не попадали устаревшие действия
            keyTable[i][shortcutName] = QKeySequence(config.value(shortcutName).toString());
        }
        config.endGroup(); // Закрывается текущая секция
    }
}

/// @brief Запись всех шорткатов в файл
void ShortcutManager::saveConfig(const QMap<QString, QKeySequence> table[]) const {
    QSettings config(configFileName, QSettings::IniFormat);
    for (int i = SECTION_NOTE; i < SECTION_TOTAL_COUNT; ++i) {
        for (auto kv = table[i].constKeyValueBegin(); kv != table[i].constKeyValueEnd(); ++kv) {
            if (defaultKeyTable[i].contains(kv->first))
                config.setValue(availableSection[i] + "/" + kv->first, kv->second.toString());
            else
                criticalError("Not found action " + kv->first + " in section " + availableSection[i]);
        }
    }
    config.sync();
}

/// @brief Установка клавиатурной последовательности в памяти, без записи на диск
void ShortcutManager::setKeySequence(shortcutSections section, QString actionName, QString keySequence) {
    if (keyTable[section].contains(actionName))
        keyTable[section][actionName] = QKeySequence(keySequence);
    else
        criticalError("Incorrect action for set key sequence: " + actionName + " in section " + availableSection[section]);
}

QString ShortcutManager::getDescription(shortcutSections section, QString actionName) const {
    if (defaultKeyTable[section].contains(actionName))
        return defaultKeyTable[section][actionName].description;
    else
        return "";
}

QString ShortcutManager::getExplanation(shortcutSections section, QString actionName) const {
    if (defaultKeyTable[section].contains(actionName))
        return defaultKeyTable[section][actionName].explanation;
    else
        return "";
}

QString ShortcutManager::getDescriptionWithShortcut(shortcutSections section, QString actionName) const {
    if (keyTable[section].contains(actionName))
        return defaultKeyTable[section][actionName].description + " (" + getKeySequenceAsText(section, actionName) + ")";
    else
        return "";
}

QString ShortcutManager::getFullDescription(shortcutSections section, QString actionName) const {
    auto res = getDescriptionWithShortcut(section, actionName);
    if (res.size() > 0 && defaultKeyTable[section][actionName].explanation.size() > 0)
        res += " - " + defaultKeyTable[section][actionName].explanation;
    return res;
}

void ShortcutManager::initAction(shortcutSections section, QString actionName, QAction *action) {
    if (!keyTable[section].contains(actionName))
        criticalError("Not found available action name " + actionName + " in section " + availableSection[section]);
    action->setShortcut(getKeySequence(section, actionName));
    action->setStatusTip(getFullDescription(section, actionName));
    action->setToolTip(getDescriptionWithShortcut(section, actionName));
    action->setText(getDescriptionWithShortcut(section, actionName));
}

void ShortcutManager::initActions(shortcutSections section, const QList<QPair<QString, QAction *>> &actions) {
    for (const auto &a : actions)
        initAction(section, a.first, a.second);
}

void ShortcutManager::initToolButton(shortcutSections section, QString actionName, QToolButton *action) {
    if (!keyTable[section].contains(actionName))
        criticalError("Not found available action name " + actionName + " in section " + availableSection[section]);
    action->setShortcut(getKeySequence(section, actionName));
    action->setStatusTip(getFullDescription(section, actionName));
    action->setToolTip(getDescriptionWithShortcut(section, actionName));
    action->setText(getDescriptionWithShortcut(section, actionName));
}

/// @brief Проверка имен секций, допустимо ли одно и то же сочетание клавиш для указанных двух секций
bool ShortcutManager::isOverloadEnable(QString sectionNameA, QString sectionNameB) const {
    return (sectionNameA == "attach" && sectionNameB == "editor") ||
           (sectionNameB == "attach" && sectionNameA == "editor");
}
