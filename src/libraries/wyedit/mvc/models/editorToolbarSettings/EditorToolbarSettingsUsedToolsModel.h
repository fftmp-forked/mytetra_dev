#pragma once

#include <QObject>

#include "EditorToolbarSettingsAbstractModel.h"

class EditorConfig;


/// @brief Модель списка используемых команд на выбранной строке панели инструментов редактора
class EditorToolbarSettingsUsedToolsModel : public EditorToolbarSettingsAbstractModel
{
    Q_OBJECT

public:
    /// Указание на обрабатываемую панель инструментов редактора текста
    enum class EditorToolbarLine {
        First = 0,
        Second
    };
    EditorToolbarSettingsUsedToolsModel(QObject *parent = nullptr);

    // Первичное наполнение модели
    void init(EditorToolbarLine tb);

};

