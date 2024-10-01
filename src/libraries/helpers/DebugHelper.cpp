#include <QDebug>
#include <QMessageBox>

#include "DebugHelper.h"
#include "main.h"

void criticalError(QString message) {
    qDebug() << " ";
    qDebug() << "---------------";
    qDebug() << "Critical error!";
    qDebug() << "---------------";
    qDebug() << message;
    qDebug() << "---------------";
    qDebug() << " ";

    QMessageBox::critical(qobject_cast<QWidget *>(pMainWindow), "Critical error", message + "\n\nProgram will be closed.");

    exit(1);
}

/// @brief Рекурсивная печать дерева объектов, т.к. dumpObjectInfo() и dumpObjectTree() не работают
static void printObjectTreeRecurse(const QObject *obj) {
    static int indent = 0;

    for (const auto &curobj : obj->children()) {
        QString indentline(indent, '.');

        if ((curobj->objectName()).length() == 0)
            qDebug("%s%s", indentline.toLocal8Bit().data(), curobj->metaObject()->className());
        else
            qDebug("%s%s, NAME %s", indentline.toLocal8Bit().data(),
                   curobj->metaObject()->className(),
                   (curobj->objectName()).toLocal8Bit().data());

        ++indent;
        printObjectTreeRecurse(curobj);
        --indent;
    }
}

void printObjectTree(const QObject *obj) {
    qDebug() << "Object tree";

    printObjectTreeRecurse(obj);
}
