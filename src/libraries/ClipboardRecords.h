#pragma once

#include <QList>
#include <QMap>
#include <QMimeData>
#include <QWidget>

#include "../models/recordTable/Record.h"

class ClipboardRecords : public QMimeData {
    Q_OBJECT

  public:
    ClipboardRecords(void) : QMimeData() { init(); }
    ~ClipboardRecords(void) {}

    void init(void);
    void clear(void);
    void addRecord(Record record);
    void print(void) const;
    int getCount(void) const { return records.size(); }

    Record getRecord(int n) const;
    QString getRecordText(int n) const;
    QMap<QString, QString> getRecordFieldList(int n) const;
    QMap<QString, QByteArray> getRecordPictureFiles(int n) const;
    AttachTableData getRecordAttachTable(int n) const;

    // Этот метод QMimeData надо переопределить, так как он виртуальный
    QStringList formats() const { return clipbRecordsFormat; }

  protected:
    // Этот метод QMimeData надо переопределить, так как он виртуальный
    QVariant retrieveData(const QString &format, QMetaType preferredType) const;

  private:
    QList<Record> records;          // Данные, которые передаются через буфер обмена
    QStringList clipbRecordsFormat; // Строка с идентификатором формата
};
