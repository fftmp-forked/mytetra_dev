#pragma once

#include <QObject>

#include "libraries/GlobalParameters.h"
#include "EditorToolbarSettingsAbstractModel.h"

class EditorConfig;


/// @brief Модель списка используемых команд на выбранной строке панели инструментов редактора
class EditorToolbarSettingsUsedToolsModel : public EditorToolbarSettingsAbstractModel
{
    Q_OBJECT

public:

    EditorToolbarSettingsUsedToolsModel(QObject *parent = nullptr);

    // Первичное наполнение модели
    void init(GlobalParameters::EditorToolbar tb);

};

