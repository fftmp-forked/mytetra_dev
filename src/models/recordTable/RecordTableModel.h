#pragma once

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#define TABLE_DATA_ROLE Qt::UserRole + 10
#define ONE_RECORD_ROLE Qt::UserRole + 11
#define RECORD_ID_ROLE Qt::UserRole + 12
#define RECORD_BLOCK_ROLE Qt::UserRole + 13
#define SORT_ROLE Qt::UserRole + 14

class Record;
class RecordTableData;

class RecordTableModel : public QAbstractTableModel {
    Q_OBJECT

    // К закрытым (private) функциям модели может иметь доступ контроллер
    friend class RecordTableController;

  public:
    RecordTableModel(QObject *pobj = nullptr);
    ~RecordTableModel() {}

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    // Интерфейс модели, предоставление данных по указанному индексу
    QVariant data(const QModelIndex &index, int role) const;

    // Интерфейс модели, сохранение вводимых данных по указанному индексу
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    // Интерфейс модели, сколько записей в таблице
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    // Интерфейс модели, сколько столбцов в таблице
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

  private:
    // Установка указателя на таблицу данных, с которой нужно работать модели
    void setTableData(RecordTableData *rtData);

    // Ссылка на данные, с которыми работает модель
    RecordTableData *getTableData(void) const { return table; }

    // Добавление записей
    int addTableData(int mode, QModelIndex posIndex, Record record);

    void onRecordTableConfigChange(void);

  protected:
    // Указатель на таблицу конечных записей
    RecordTableData *table;
};
