#pragma once

#include <QDomDocument>
#include <QDomElement>
#include <QIcon>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include <QXmlStreamWriter>

#include "../../models/recordTable/RecordTableData.h"

class TreeItem {
  public:
    TreeItem(const QMap<QString, QString> &data, TreeItem *parent = nullptr);
    ~TreeItem() { empty(); /* очищение ветки без физического удаления данных на диске */ };

    TreeItem *child(int number) { return childItems.value(number); }

    int childCount() const { return childItems.count(); }

    int fieldCount() const { return fieldsTable.count(); }

    // Получение значения поля по имени
    QString getField(QString name);

    // Получение всех полей данных
    QMap<QString, QString> getAllFields();

    // Получение всех полей данных напрямую, без преобразований
    QMap<QString, QString> getAllFieldsDirect() const { return fieldsTable; }

    // Заполнение указанного поля
    void setField(QString name, QString value);

    // Заполнение указанного поля данными напрямую, без преобразований
    void setAllFieldDirect(const QMap<QString, QString> nameAndValue);

    // Добавление потомка (потомков) к текущему элементу
    // position - после какой позиции массива childItems вставить
    // count - сколько потомков вставить (обычно 1, но можно и несколько)
    // columns - сколько столбцов должен содержать потомок
    bool insertChildren(int position, int count, int columns);

    // Добавление нового пустого подчиненного элемента в конец списка подчиненных элементов
    bool addChildrenEmpty();

    // Добавление уже существующего Item-элемента
    bool addChildrenItem(TreeItem *item);

    // Возвращение ссылки на родительский элемент
    TreeItem *parent() const { return parentItem; }

    // Удаление потомков, начиная с позиции position массива childItems
    bool removeChildren(int position, int count);

    // Удаление всех потомков элемента
    void removeAllChildren();

    // Возвращает номер, под которым данный объект хранится
    // в массиве childItems своего родителя
    int childNumber() const;

    bool moveUp();
    bool moveDn();

    // Возвращает id путь (список идентификаторов от корня до текущего элемента)
    QStringList getPath() { return getPathAsField("id"); }

    // Возвращает путь в виде названий веток дерева
    QStringList getPathAsName() { return getPathAsField("name"); }

    QString getPathAsNameWithDelimeter(QString delimeter);

    // Возвращает набор значений указанного поля для пути от корня к ветке
    QStringList getPathAsField(QString fieldName);

    // Возвращает массив путей всех подветок, которые содержит ветка
    QList<QStringList> getAllChildrenPath();

    // Возвращает набор значений указанного поля для подветок
    QList<QStringList> getAllChildrenPathAsField(QString fieldName);

    // Получение идентификатора элемента
    QString getId();

    // Получение идентификатора родительской ветки
    QString getParentId();

    // возвращается иконка, не важно, пустая или с рисунком
    QIcon getIcon() const { return icon; }

    // Первичное заполнение таблицы конечных записей, "промежуточный" метод
    void recordtableInit(QDomElement domModel) { recordsTable.init(this, domModel); }

    // Взятие количества записей в таблице конечных записей, "промежуточный" метод
    int recordtableGetRowCount() const { return recordsTable.size(); }

    // Удаление всех элементов в таблице конечных записей, "промежуточный" метод
    void recordtableDeleteAllRecords() { recordsTable.deleteAllRecords(); }

    // Преобразование таблицы конечных записей в DOM представление, "промежуточный" метод
    QDomElement recordtableExportDataToDom(QDomDocument *doc) const { return recordsTable.exportDataToDom(doc); }
    void recordtableExportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const { recordsTable.exportDataToStreamWriter(xmlWriter); }

    // Взятие ссылки на данные конечных записей
    RecordTableData *recordtableGetTableData() { return &recordsTable; }

    void setDetached(bool state) { detachedState = state; }
    bool isDetached() const { return detachedState; }

  private:
    bool removeChildrenLink(int position, int count);

    void empty();

    QList<QStringList> getAllChildrenPathAsFieldRecurse(TreeItem *item, QString fieldName, int mode);

    QList<TreeItem *> childItems; // Список ссылок на потомков
    TreeItem *parentItem;         // Ссылка на родителя

    // Таблица инфополей данной ветки
    QMap<QString, QString> fieldsTable;

    // Каждая ветка может содержать таблицу конечных записей
    RecordTableData recordsTable;

    bool detachedState; // Флаг, влияющий на деструктор. Если detachedState=true, подчиненные элементы удаляться не будут

    QIcon icon; // Иконка ветки
};
