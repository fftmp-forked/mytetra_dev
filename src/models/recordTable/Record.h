#pragma once

#include <QByteArray>
#include <QDomElement>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QTextDocument>
#include <QXmlStreamWriter>

#include "../attachTable/Attach.h"
#include "../attachTable/AttachTableData.h"

/// @brief Класс одной записи в таблице записей
class Record {

    // К закрытым функциям может иметь доступ объекты приаттаченного файла
    friend class Attach;
    friend class AttachTableData;

  public:
    Record();
    Record(const Record &obj);
    virtual ~Record(){};

    void setupDataFromDom(QDomElement iDomElement);
    QDomElement exportDataToDom(QDomDocument *doc) const;
    void exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const;

    QString getText() const;
    QString getTextDirect() const;
    QSharedPointer<QTextDocument> getTextDocument() const;
    void setText(const QString &iText);

    QString getField(const QString &name) const;
    void setField(const QString &name, const QString &value);
    bool isNaturalFieldExists(const QString &name) const;

    // Установка и чтение данных без преобразований. Используется при генерации/чтении XML
    QString getNaturalFieldSource(QString name) const;
    void setNaturalFieldSource(QString name, QString value);

    QMap<QString, QString> getNaturalFieldList() const;

    QMap<QString, QByteArray> getPictureFiles() const;
    void setPictureFiles(QMap<QString, QByteArray> iPictureFiles);

    AttachTableData getAttachTable() const;
    AttachTableData *getAttachTablePointer();
    void setAttachTable(AttachTableData iAttachTable);

    bool isEmpty() const;
    bool isLite() const;
    void switchToLite();
    void switchToFat();

    void pushFatAttributes();

    static void replaceInternalReferenceByTranslateTable(QString recordFileName, QMap<QString, QString> idRecordTranslate);

  protected:
    // ----------------------------------------------------------------------
    // Свойства класса (не забыть перечислить все в конструкторе копирования)
    // ----------------------------------------------------------------------

    bool liteFlag;

    // Установка содержимого свойств происходит в вышестоящем коде

    // Легкие свойства
    QMap<QString, QString> fieldList; // Перечень свойств записи (атрибутов) ИмяАтрибута - Значение

    // Полновесные свойства
    QByteArray text;                        // Содержимое файла с текстом записи
    QMap<QString, QByteArray> pictureFiles; // Содержимое картинок, используемых в тексте записи (используется при переносе через буфер обмена, при DragAndDrop)

    // Таблица прикрепляемых файлов
    AttachTableData attachTableData;

    void saveTextDirect(QString iText) const;
    void saveText();

    QString getIdAndNameAsString() const; // Внутренний метод для облегчения печати отладочной информации

    QString getFullDirName() const;
    QString getShortDirName() const;

    QString getFullTextFileName() const;
    QString getFullFileName(QString fileName) const;

    void checkAndFillFileDir(QString &nameDirFull, QString &nameFileFull);
    void checkAndCreateTextFile() const;

    QString getNaturalField(QString name) const;
    QString getCalculableField(QString name) const;
};
