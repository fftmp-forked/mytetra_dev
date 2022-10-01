#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMimeData>
#include <QtDebug>

#include "../models/recordTable/Record.h"
#include "../models/attachTable/AttachTableData.h"
#include "ClipboardRecords.h"
#include "FixedParameters.h"
#include "helpers/DebugHelper.h"


/// @brief Подготовка объекта для загрузки данных
void ClipboardRecords::init(void) {
  if(records.size() > 0)
    records.clear();
  
  clipbRecordsFormat.clear();
  clipbRecordsFormat << FixedParameters::appTextId + "/records";
}


void ClipboardRecords::clear(void) {
  init();
  clipbRecordsFormat << "";
}


void ClipboardRecords::addRecord(Record record) {
  records << record;
}


// Печать информации о содержимом записи
void ClipboardRecords::print(void) const {
  QListIterator< Record > list(records);
  
  // Перебор записей
  while (list.hasNext()) {
    Record record=list.next();

    qDebug() << record.getText();

    // Перебор полей в записи
    QMap<QString, QString> fieldList=record.getNaturalFieldList();
    QMapIterator<QString, QString> currentField(fieldList);
    while(currentField.hasNext())
    {  
      currentField.next();
      qDebug() << currentField.key() << ": " << currentField.value();
    }

    // Перебор информации о праттаченных файлах в записи
    if(record.getAttachTable().size()>0)
      record.getAttachTable().print();
  }
}


Record ClipboardRecords::getRecord(int n) const {
    if(n < records.size())
        return records.at(n);

    criticalError("In ClipboardRecords::getRecord() unavailable number " + QString::number(n));
    return Record();
}


/// @brief Получение текста записи с указанным номером
QString ClipboardRecords::getRecordText(int n) const {
    if(n < records.size())
        return records.at(n).getText();

    criticalError("In ClipboardRecords::getRecordText() unavailable number " + QString::number(n));
    return QString();
}


/// @brief Получение полей записи с указанным номером
QMap<QString, QString> ClipboardRecords::getRecordFieldList(int n) const {
    if(n<records.size())
        return records.at(n).getNaturalFieldList();

    criticalError("In ClipboardRecords::getRecordFieldTable() unavailable number " + QString::number(n));
    return QMap<QString, QString>();
}


/// @brief Получение информации о приаттаченных файлах для записи с указанным номером
AttachTableData ClipboardRecords::getRecordAttachTable(int n) const {
    if(n < records.size())
        return getRecord(n).getAttachTable();

    criticalError("In ClipboardRecords::getRecordAttachTable() unavailable number " + QString::number(n));
    return AttachTableData();
}


/// @brief Получение файлов картинок
QMap<QString, QByteArray> ClipboardRecords::getRecordPictureFiles(int n) const
{
    if(n < records.size())
        return records.at(n).getPictureFiles();

    criticalError("In ClipboardRecords::getRecordPictureFiles() unavailable number "+QString::number(n));
    return QMap<QString, QByteArray>();
}


QVariant ClipboardRecords::retrieveData(const QString &format, QMetaType preferredType) const
{
 Q_UNUSED(preferredType);

 if(format==clipbRecordsFormat[0])
  {
   QVariant v;
   v.setValue(records);
   return v;
  }

 return 0;
} 

