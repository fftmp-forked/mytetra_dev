#pragma once

#include <QObject>
#include <QStandardItemModel>

#include "EditorToolbarSettingsAbstractModel.h"

/// @brief Модель списка всех доступных инструментов редактора
class EditorToolbarSettingsAvailableToolsModel : public EditorToolbarSettingsAbstractModel {
    Q_OBJECT

  public:
    EditorToolbarSettingsAvailableToolsModel(QObject *parent = nullptr);

    // Первичное наполнение модели
    void init();
};
