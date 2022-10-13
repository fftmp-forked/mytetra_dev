#include "ShortcutSettingsModel.h"

#include "libraries/ShortcutManager/ShortcutManager.h"

ShortcutSettingsModel::ShortcutSettingsModel(QObject *parent) : QStandardItemModel(parent) {
    for (int i = ShortcutManager::SECTION_NOTE; i < ShortcutManager::SECTION_TOTAL_COUNT; ++i) {
        // Создание разделов
        auto s = static_cast<ShortcutManager::shortcutSections>(i);
        auto *sectionItem = new QStandardItem(ShortcutManager::availableSection[i]);
        this->appendRow(sectionItem);
        // Индекс только что созданного раздела шорткатов
        auto sectionIndex = this->index(i, 0); // Индекс элемента раздела

        // Создание строк с шорткатами
        auto actions = ShortcutManager::get().getActionsNameList(s);
        this->insertRows(0, actions.size(), sectionIndex);
        this->insertColumns(0, ShortcutSettingsModel::columnCount(), sectionIndex);

        // Заполнение строк с шорткатами
        int j = 0;
        for (const auto &actionName : actions) {
            auto index = this->index(j, 0, sectionIndex);
            this->setData(index, actionName);

            index = this->index(j, 1, sectionIndex);
            this->setData(index, ShortcutManager::get().getDescription(s, actionName));

            index = this->index(j, 2, sectionIndex);
            this->setData(index, ShortcutManager::get().getKeySequenceAsText(s, actionName));

            index = this->index(j, 3, sectionIndex);
            this->setData(index, ShortcutManager::get().getDefaultKeySequenceAsText(s, actionName));

            ++j;
        }
    }
}

/// @fixme
/// @return Индекс первой ячейки с записью о шорткате
QModelIndex ShortcutSettingsModel::findShortcut(const QString &shortcutFullName) const {
    QStringList chunk = shortcutFullName.split("-");
    QString shortcutSection = chunk[0];
    QString shortcutCommand = chunk[1];

    // Поиск секции
    for (int i = 0; i < this->rowCount(QModelIndex()); ++i) {
        QModelIndex sectionIndex = this->index(i, 0);

        // Если секция найдена
        if (this->data(sectionIndex).toString() == shortcutSection) {

            // Поиск команды
            for (int j = 0; j < this->rowCount(sectionIndex); ++j) {
                QModelIndex commandIndex = this->index(j, 0, sectionIndex);

                // Если команда найдена
                if (this->data(commandIndex).toString() == shortcutCommand) {
                    return commandIndex;
                }
            }
        }
    }
    return QModelIndex(); // Возвращается пустой индекс
}

/// @brief save shortcut settings to disk. Need to reload ShortcutManager to apply.
void ShortcutSettingsModel::save() {
    const QMap<QString, QKeySequence> keyTable[ShortcutManager::SECTION_TOTAL_COUNT];
    assert(ShortcutManager::SECTION_TOTAL_COUNT == this->rowCount());
    // Перебор секций
    for (int i = 0; i < this->rowCount(); ++i) {
        auto sectionIndex = this->index(i, 0);

        // Перебор команд в секции
        for (int j = 0; j < this->rowCount(sectionIndex); ++j) {
            auto actionName = this->data(this->index(j, 0, sectionIndex)).toString();
            auto keysName = this->data(this->index(j, 2, sectionIndex)).toString();
            keyTable[i][actionName] = keysName;
        }
    }
    ShortcutManager::get().saveConfig(keyTable);
}

/// @brief reset both model and ShortcutManager to default key bindings
void ShortcutSettingsModel::resetAllShortcutsToDefault() {
    for (int i = 0; i < this->rowCount(); ++i) { // iterate over sections
        auto sectionIndex = this->index(i, 0);

        for (int j = 0; j < this->rowCount(sectionIndex); ++j) { // iterate over actions
            auto actionName = this->data(this->index(j, 0, sectionIndex)).toString();
            auto keysIndex = this->index(j, 2, sectionIndex);
            auto keysName = this->data(keysIndex).toString();

            auto defaultKeysName = this->data(this->index(j, 3, sectionIndex)).toString();

            // Если сочетание клавиш у данной команды не соответствует стандартному
            if (keysName != defaultKeysName) {
                this->setData(keysIndex, defaultKeysName);                                                                             // Устанавливается в дереве
                ShortcutManager::get().setKeySequence(static_cast<ShortcutManager::shortcutSections>(i), actionName, defaultKeysName); // Устанавливается в менеджере
            }
        }
    }
}

///@brief check hotkeys for conflicts
/// @return string with error description or nullopt
std::optional<QString> ShortcutSettingsModel::check() const {
    QMap<QString, QString> keysSequenceList; // <комбинация клавиш> -> <ассоциированное действие>

    for (int i = 0; i < this->rowCount(); ++i) {
        auto sectionIndex = this->index(i, 0);
        auto sectionName = this->data(sectionIndex).toString();

        for (int j = 0; j < this->rowCount(sectionIndex); ++j) {
            // Полное имя команды
            auto commandName = sectionName + "-" + this->data(this->index(j, 0, sectionIndex)).toString();
            auto keysName = this->data(this->index(j, 2, sectionIndex)).toString();

            if (keysName == "")
                continue; // пустое сочетание

            // Если сочетания нет в списке, оно добавляется в список
            if (!keysSequenceList.contains(keysName)) {
                keysSequenceList[keysName] = commandName;
            } else {
                auto existName = keysSequenceList[keysName];
                // Проверка, допустимо ли повторение сочетания
                if (!ShortcutManager::get().isOverloadEnable(sectionName, existName.left(existName.indexOf('-'))))
                    return tr("Found duplicate key sequense <b>%3</b> for action <b>%1</b> and <b>%2</b>").arg(existName, commandName, keysName);
            }
        }
    }
    return std::nullopt;
}

/// @brief Получение заголовка столбца
QVariant ShortcutSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Command");
        case 1:
            return tr("Name");
        case 2:
            return tr("Shortcut");
        case 3:
            return tr("Default Shortcut"); // Скрытый столбец
        }
    }
    return QVariant();
}

Qt::ItemFlags ShortcutSettingsModel::flags(const QModelIndex &index) const {
    // Для разделов
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    // Для строк с шорткатами
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
