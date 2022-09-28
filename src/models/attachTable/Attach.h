#pragma once

#include <QString>
#include <QMap>
#include <QByteArray>
#include <QDomElement>
#include <QXmlStreamWriter>

class AttachTableData;


/// @brief Класс, реализующий абстракцию одного прикрепляемого файла
class Attach
{
  friend class AttachTableData;

public:

  enum class Type {file, link};
  Attach(AttachTableData *iParentTable);
  Attach(Type type, AttachTableData *iParentTable);
  virtual ~Attach();

  void setupDataFromDom(QDomElement iDomElement);
  QDomElement exportDataToDom(QDomDocument *doc) const;
  void exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const;

  QString getField(QString name) const;
  void setField(QString name, QString value);

  // Работа с именем файла
  static QString constructFileName(const QString type, const QString id, const QString fileName);
  QString getInnerFileName() const;
  QString getFullInnerFileName() const;
  QString getFullInnerDirName() const;
  QString getAbsoluteInnerFileName() const;

  bool setLink(QString iLink);

  qint64 getFileSize() const;

  // todo: подумать, может унаследовать Attach и Record от общего класса LiteFatObject
  bool isEmpty() const;
  bool isLite() const;
  void switchToLite();
  void switchToFat();

  void pushFatDataToDisk();
  void pushFatDataToDirectory(QString dirName);
  void popFatDataFromDisk();

  bool copyFileToBase(QString iFileName);
  void removeFile();
  
  void renameFile(QString newFileName);
protected:

  void init(AttachTableData *iParentTable);
  void setParentTable(AttachTableData *iParentTable); // Защищенный метод, который может вызвать только этот класс и AttachTableData

  static QStringList fieldAvailableList(void);

  bool liteFlag;

  AttachTableData *parentTable; // Указатель на таблицу приаттаченных файлов, которой принадлежит данный аттач

  QMap<QString, QString> fields;

  QByteArray fileContent; // Содержимое файла, используется в режиме полных данных
};

