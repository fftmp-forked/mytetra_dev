#pragma once

#include <QMap>
#include <QObject>
#include <QStringList>

/// @brief Неизменяемые параметры, то есть параметры, которые заданы жестко в текущей версии MyTetra
class FixedParameters : public QObject {
    Q_OBJECT

  public:
    FixedParameters(QObject *parent = nullptr) { Q_UNUSED(parent); };
    ~FixedParameters(){};

    // Поля для записей
    static const QStringList recordFieldAvailableList;
    static const QStringList recordNaturalFieldAvailableList;
    static const QStringList recordCalculableFieldAvailableList;

    // Поля для веток
    static const QStringList itemFieldAvailableList;

    // Директория относительно XML-файла, в которой хранятся директории с иконками
    static const QString iconsRelatedDirectory;

    // Текстовый идентификатор приложения
    static const QString appTextId;

    static bool isRecordFieldAvailable(QString name) { return recordFieldAvailableList.contains(name); }
    static bool isRecordFieldNatural(QString name) { return recordNaturalFieldAvailableList.contains(name); }
    static bool isRecordFieldCalculable(QString name) { return recordCalculableFieldAvailableList.contains(name); }

    static QMap<QString, QString> recordFieldDescription(QStringList list);
};
