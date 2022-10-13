#include "EditorToolbarSettingsUsedToolsModel.h"

#include "../../../../../views/record/MetaEditor.h"
#include "../../../../ShortcutManager/ShortcutManager.h"
#include "../../../../helpers/ObjectHelper.h"
#include "../../../EditorConfig.h"
#include "../../../EditorToolBarAssistant.h"

EditorToolbarSettingsUsedToolsModel::EditorToolbarSettingsUsedToolsModel(QObject *parent) : EditorToolbarSettingsAbstractModel(parent) {
}

void EditorToolbarSettingsUsedToolsModel::init(EditorToolbarLine tb) {
    // Вначале модель полностью очищается, чтобы не было наложений от предыдущих открытий окна настроек
    this->clear();

    // Создание команд для панели с учетом номера панели
    QStringList commandsInToolsLine = tb == EditorToolbarLine::First
                                          ? editorConfig->get_tools_line_1().split(',')
                                          : editorConfig->get_tools_line_2().split(',');

    EditorToolBarAssistant *editorToolBarAssistant = find_object<MetaEditor>("editorScreen")->editorToolBarAssistant;

    for (int i = 0; i != commandsInToolsLine.size(); ++i) {

        QString command = commandsInToolsLine[i];
        QStandardItem *newItem = new QStandardItem();

        newItem->setData(command, Qt::UserRole);

        if (command == "separator")
            newItem->setData(tr("<Separator>"), Qt::DisplayRole);
        else
            newItem->setData(ShortcutManager::get().getDescription(ShortcutManager::SECTION_EDITOR, command), Qt::DisplayRole);

        newItem->setIcon(editorToolBarAssistant->getIcon(command));

        this->appendRow(newItem); // Владение элементом передается модели
    }

    emit layoutChanged();
}
