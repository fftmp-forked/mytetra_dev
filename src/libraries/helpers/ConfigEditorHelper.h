#pragma once

#include <QString>

class ConfigEditorHelper {
  public:
    ConfigEditorHelper(){};

    // Редактирование какого-либо конфиг-файла
    static void editConfigFile(QString fileName, double sizeCoeffitient);
};
