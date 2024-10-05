#pragma once

#include <QString>

#include "../../views/mainWindow/MainWindow.h"

#include "DebugHelper.h"


/// @brief Поиск объекта от корня по имени
template <typename X>
X *find_object(QString objectName) {
    // Запрошен обычный объект, надо его найти
    auto findObj = MainWindow::get().findChild<X *>(objectName);

    if (!findObj) {
        // Если объекта с указанным именем не найдено
        printf("find_object(): Can't find object with name %s\n", qPrintable(objectName));

        printObjectTree(&MainWindow::get());
        exit(1);
    } else {
        // Объект был найден, и нужно преобразовать указатель на него
        // к указателю c заданным в шаблоне типом
        X *obj = qobject_cast<X *>(findObj);

        if (obj == 0) {
            // Если найденный объект не может быть преобразован к заданному в шаблоне типу
            printf("find_object(): Object %s find, but can't convert type. Check <type> in function call\n", qPrintable(objectName));
            exit(1);
        } else
            return obj; // Объект найден нормально
    }

    return nullptr;
}
