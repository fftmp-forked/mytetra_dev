#include <QMutableMapIterator>

#include "FixedParameters.h"


// Поля для записей
const QStringList FixedParameters::recordFieldAvailableList=       {"id", "name", "author", "url", "tags", "ctime", "dir", "file", "block", "hasAttach", "attachCount"};
const QStringList FixedParameters::recordNaturalFieldAvailableList={"id", "name", "author", "url", "tags", "ctime", "dir", "file", "block"};
const QStringList FixedParameters::recordCalculableFieldAvailableList={"hasAttach", "attachCount"};

// Поля для веток
const QStringList FixedParameters::itemFieldAvailableList={"id", "name", "ctime", "icon"};

// Директория относительно XML-файла, в которой хранятся директории с иконками
const QString FixedParameters::iconsRelatedDirectory="icons";

// Текстовый идентификатор приложения
const QString FixedParameters::appTextId="mytetra";


/// @brief Получение описаний набора полей
QMap<QString, QString> FixedParameters::recordFieldDescription(QStringList list) {
  QMap<QString, QString> names;

  names["id"]=tr("ID");
  names["name"]=tr("Title");
  names["author"]=tr("Author");
  names["url"]=tr("Url");
  names["tags"]=tr("Tags");
  names["ctime"]=tr("Create time");
  names["dir"]=tr("Directory name");
  names["file"]=tr("File name");
  names["hasAttach"]=tr("Has attaches");
  names["attachCount"]=tr("Attaches count");
  names["block"]=tr("Block");


  // Удаляются строчки, которых нет в переданном списке
  QMutableMapIterator<QString, QString> iterator(names);
  while (iterator.hasNext()) {
    iterator.next();

    if(!list.contains( iterator.key() ))
      iterator.remove();
  }

  return names;
}
