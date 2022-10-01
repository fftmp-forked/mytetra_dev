#pragma once

#include <QObject>

#include "../../views/editorToolbarSettings/EditorToolbarCommandsListView.h"
#include "../../models/editorToolbarSettings/EditorToolbarSettingsUsedToolsModel.h"


/// @brief Контроллер для представления и модели списка используемых команд для выбранной строки инструментов редактора текста
class EditorToolbarUsedCommandsController : public QObject
{
    Q_OBJECT

public:

    EditorToolbarUsedCommandsController(EditorToolbarSettingsUsedToolsModel::EditorToolbarLine tb, QObject *parent = nullptr);
    ~EditorToolbarUsedCommandsController();

    // Направление перемещения команды на панели используемых комманд
    enum CommandMove {
        Up = -1,
        Down = 1
    };

    // Инициализация представления
    void init();

    // Получение представления
    EditorToolbarCommandsListView *getView() const;

    // Получение модели
    EditorToolbarSettingsUsedToolsModel *getModel() const;

    // Получение SelectionModel
    QItemSelectionModel *getSelectionModel() const;

    // Перемещение выбранной команды в моделе команд выбранной строки панели инструментов
    void moveCommandUpDown(CommandMove direction);


protected:

    EditorToolbarSettingsUsedToolsModel::EditorToolbarLine tb; // Номер обрабатываемой панели

    EditorToolbarCommandsListView *view;
    EditorToolbarSettingsUsedToolsModel *model;


private:

    // Перемещение выбранной команды в моделе команд выбранной строки панели инструментов
    void moveCommandUpDown(CommandMove direction, int selectedRow);
};

