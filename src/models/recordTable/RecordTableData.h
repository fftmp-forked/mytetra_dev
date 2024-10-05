#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QDomElement>
#include <QList>
#include <QMap>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QXmlStreamWriter>

#include "models/recordTable/Record.h"

class TreeItem;

/// @brief Это набор данных конечной таблицы, с которыми удобно работать
class RecordTableData {

  public:
    RecordTableData() {};
    virtual ~RecordTableData() { empty(); };

    // Получение текста указанной записи
    QString getText(int pos) const;

    // Установка текста и картинок указанной записи
    void setTextAndPictures(int pos,
                            const QString &text,
                            const QMap<QString, QByteArray> &picturesFiles = (QMap<QString, QByteArray>()));

    // Получение значения указанного поля для указанного элемента
    QString getField(QString name, int pos) const;

    // Установка значения указанного поля для указанного элемента
    void setField(QString name, QString value, int pos);

    // Получение образа записи
    Record getRecordLite(int pos) const;
    Record getRecordFat(int pos);

    // Получение указателя на запись по известному номеру записи в таблице
    Record *getRecord(int pos);

    // Получение указателя на запись по известному id записи
    Record *getRecordById(const QString &id);

    // Получение списка идентификаторов всех записей в таблице
    QSet<QString> getRecordsIdList();

    // Первичное заполнение таблицы конечных записей
    void init(TreeItem *item, QDomElement domModel);

    // Удаление всех элементов таблицы конечных записей
    void deleteAllRecords();

    // Количество записей в таблице данных
    unsigned int size() const { return tableData.size(); };

    // Функция создания DOM-документа из данных таблицы конечных записей
    QDomElement exportDataToDom(QDomDocument *doc) const;
    void exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const;

    // Получение ссылки на объект ветки, которой принадлежит таблица
    TreeItem *getItem() const { return treeItem; };

    int insertNewRecord(int mode, int pos, Record record);

    void editRecordFields(int pos, QMap<QString, QString> editFields);

    void deleteRecord(int i);
    void deleteRecordById(QString id);

    bool isRecordExists(QString id);
    bool isBlockRecordExists();

    int getPosById(QString id);

    void empty();

    void moveUp(int pos);
    void moveDn(int pos);

    static void editorLoadCallback(QObject *editor, QString &loadText);

    static void editorSaveCallback(QObject *editor, QString saveText);

    int getWorkPos() const { return workPos; };
    void setWorkPos(int pos) { workPos = pos; };

    void checkAndCreateTextFile(int pos, QString fullFileName);

  private:
    // Функция заполнения таблицы из DOM-документа
    void setupDataFromDom(QDomElement *domModel);

    // Таблица записей (в нормальном виде содержит только "легкие" объекты записей)
    QList<Record> tableData;

    // Ссылка на ветку, которой принадлежит данная таблица
    TreeItem *treeItem = nullptr;

    // Номер записи, с которой работал пользователь
    int workPos = -1;
};
