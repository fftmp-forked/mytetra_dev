#pragma once

#include "../../main.h"

#include "DebugHelper.h"

/// @brief Поиск объекта от корня по имени
template <typename X>
X *find_object(QString objectName) {
    QObject *findObj;

    // Если запрошен сам корень
    if (objectName == "mainwindow") {
        QObject *mvp = qobject_cast<X *>(pMainWindow);

        if (mvp->metaObject()->className() != pMainWindow->metaObject()->className()) {
            // Если запрошенный класс объекта не является классом главного окна
            printf("find_object(): Can't find mainwindow object. Check <type> in function call\n");
            exit(1);
            return nullptr;
        } else
            return qobject_cast<X *>(pMainWindow);
    }

    // Запрошен обычный объект, надо его найти
    findObj = pMainWindow->findChild<X *>(objectName);

    if (!findObj) {
        // Если объекта с указанным именем не найдено
        printf("find_object(): Can't find object with name %s\n", qPrintable(objectName));

        printObjectTree(pMainWindow);
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
