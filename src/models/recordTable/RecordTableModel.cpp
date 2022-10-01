#include <QBrush>
#include <QColor>
#include <QIcon>

#include "Record.h"
#include "RecordTableModel.h"
#include "RecordTableData.h"

#include "libraries/FixedParameters.h"
#include "libraries/helpers/DebugHelper.h"
#include "models/appConfig/AppConfig.h"


// Конструктор модели
RecordTableModel::RecordTableModel(QObject *pobj) : QAbstractTableModel(pobj), table(nullptr)
{

}


// Предоставление данных табличной модели
QVariant RecordTableModel::data(const QModelIndex &index, int role) const {
    // Если таблица данных не создана
    if(table==nullptr)
        return QVariant();

    // Если таблица пустая
    if(table->size()==0)
        return QVariant();

    // Если индекс недопустимый, возвращается пустой объект
    if(!index.isValid())
        return QVariant();

    // qDebug() << "RecordTableModel::data(), row:" << index.row() << " column " << index.column();

    // Если запрашивается текст строки для отрисовки или для редактирования
    if(role==Qt::DisplayRole || role==Qt::EditRole || role==SORT_ROLE) {
        auto showFields = AppConfig::get().getRecordTableShowFields();

        // Если длина списка показываемых столбцов меньше или равна номеру запрашиваемого столбца
        if( index.column() < showFields.size() ) {
            auto fieldName = showFields.value( index.column() );
            auto field = table->getField(fieldName, index.row());

            // Некоторые данные при отрисовке в таблице преобразуются в "экранные" представления
            // Преобразование возможно только для отображаемой в таблице информации
            if(role == Qt::DisplayRole) {
                if(fieldName=="ctime") {
                    auto fieldDateTime = QDateTime::fromString(field, "yyyyMMddhhmmss");
                    auto fmt = (AppConfig::get().getEnableCustomDateTimeFormat() ? AppConfig::get().getCustomDateTimeFormat() : QLocale::system().dateTimeFormat(QLocale::ShortFormat));
                    return QLocale::system().toString(fieldDateTime, fmt);
                } else if(fieldName=="hasAttach") {
                    return field=="0" ? "" : tr("..."); // Если аттачей нет, выводится пустая строка. Это повышает читабельность
                } else if(fieldName=="attachCount") {
                    return field=="0" ? "" : field; // Если количество аттачей нулевое, выводится пустая строка. Это повышает читабельность
                } else if(fieldName=="block") {
                    return field=="1" ? tr("...") : "";
                }
            }
            return field;
        }
    }

    if(role==RECORD_ID_ROLE)
        return table->getField("id", index.row());

    if(role==RECORD_BLOCK_ROLE)
        return table->getField("block", index.row());

    // Подсветка заднего фона
    if(role==Qt::BackgroundRole &&  AppConfig::get().getEnableRecordWithAttachHighlight() && table->getField("hasAttach", index.row())=="1")
       return QBrush(QColor(AppConfig::get().getRecordWithAttachHighlightColor()));

    // Оформление иконками
    if(role == Qt::DecorationRole) {
        QStringList showFields=AppConfig::get().getRecordTableShowFields();
        QString fieldName=showFields.value( index.column() );
        QString field=table->getField(fieldName, index.row());

        // Иконка блокировки в названии записи
        if(fieldName=="name")
            if( !showFields.contains("block") ) // Среди отображаемых столбцов нет столбца "block" (чтобы не отрисовывалось два замочка в строке)
                if( table->getField("block", index.row())=="1" ) // Если есть блокировка
                    return QIcon(":/resource/pic/note_block.svg");

        // Иконка наличия аттачей в специально предназначенном для этого столбце
        if(fieldName=="hasAttach" && field=="1")
            return QIcon(":/resource/pic/attach.svg");

        // Иконка блокировки в специально предназначенном для этого столбце
        if(fieldName=="block" && field=="1")
            return QIcon(":/resource/pic/note_block.svg");
    }
    // Во всех остальных случаях
    return QVariant();
}


// Сохранение вводимых данных по указанному индексу
bool RecordTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Если таблица данных не создана
    if(table==nullptr)
        return false;

    // Если таблица пустая
    if(table->size()==0)
        return false;

    // Если индекс недопустимый
    if(!index.isValid())
        return false;

    // Если происходит редактирование
    if(role==Qt::EditRole)
    {
        QStringList showFields=AppConfig::get().getRecordTableShowFields();

        // Если длина списка показываемых столбцов меньше или равна номеру запрашиваемого столбца
        if( index.column() < showFields.size() )
        {
            QString fieldName=showFields.value( index.column() );

            // Новое значение ячейки записывается в строковую переменную
            QString cellValue;
            cellValue=value.value<QString>();

            // Изменяется поле в таблице конечных записей
            table->setField(fieldName, cellValue, index.row());

            emit dataChanged(index,index); // Посылается сигнал что данные были изменены

            return true;
        }
    }

    // Во всех остальных случаях
    return false;
}


// Получение заголовков столбцов и строк
QVariant RecordTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    auto showFields = AppConfig::get().getRecordTableShowFields();

    // Если ни один столбец не показывается (чего, в принципе не может быть)
    if(showFields.size() == 0)
        return QVariant();

    // Если запрашивается заголовок столбца
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        // Если запрашиваемый номер столбца больше количества показываемых
        if(section>showFields.size())
            return QVariant();

        auto fieldName = showFields.value(section);

        auto descriptionFields = FixedParameters::recordFieldDescription( showFields );
        return descriptionFields.value(fieldName);
    }

    // Если запрашивается заголовок строки
    if(orientation == Qt::Vertical && role == Qt::DisplayRole)
        return section+1;

    return QAbstractTableModel::headerData(section, orientation, role);
}


int RecordTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    if(!table)
        return 0;

    return static_cast<int>( table->size() );
}


int RecordTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    static int previousColumnCount=0;

    int currentColumnCount=AppConfig::get().getRecordTableShowFields().size();

    if( currentColumnCount!=previousColumnCount) {
        qDebug() << "Column count change. New column count: " << currentColumnCount;
        previousColumnCount=currentColumnCount;
    }

    return currentColumnCount;
}


// Удаление строк в таблице
// note: Переопределение метода removeRows() влияет и на метод removeRow(),
// так как он просто вызывает removeRows() для удаления одной строки
bool RecordTableModel::removeRows(int row, int count, const QModelIndex &parent) {
    Q_UNUSED(parent)

    if(!table)
        return true;

    if(row<0 || row>=rowCount() || (row+count-1)<0 || (row+count-1)>=rowCount())
        criticalError("Bad arguments in RecordTableModel::removeRows(). row: "+QString::number(row)+" count: "+QString::number(count));

    beginRemoveRows(QModelIndex(), row, row+count-1);

    // Удаляются строки непосредственно в таблице
    for(int i=row; i<row+count; ++i)
        table->deleteRecord(i);

    endRemoveRows();

    return true;
}


// Установка данных в таблицу данных
void RecordTableModel::setTableData(RecordTableData *rtData) {
    beginResetModel();

    table=rtData;

    endResetModel();
}


/// @brief Добавление данных
/// @return позицию нового добавленного элемента
int RecordTableModel::addTableData(int mode, QModelIndex posIndex, Record record) {
    if(!table)
        return -1;

    beginResetModel(); // Подумать, возможно нужно заменить на beginInsertRows

    // Вставка новых данных в таблицу конечных записей
    int selPos = table->insertNewRecord(mode, posIndex.row(), record);

    endResetModel(); // Подумать, возможно нужно заменить на endInsertRows

    return selPos;
}


void RecordTableModel::onRecordTableConfigChange(void) {
    beginResetModel();
    endResetModel();
}

