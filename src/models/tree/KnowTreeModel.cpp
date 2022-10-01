#include <QAbstractItemModel>
#include <QMap>
#include <QDomNamedNodeMap>

#include "KnowTreeModel.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "XmlTree.h"

#include "libraries/ClipboardBranch.h"
#include "models/appConfig/AppConfig.h"
#include "models/recordTable/Record.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/DiskHelper.h"
#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "libraries/helpers/MessageHelper.h"
#include "libraries/helpers/UniqueIdHelper.h"
#include "libraries/wyedit/EditorShowTextDispatcher.h"

// Поддерживаемая версия формата базы (хранилища)
#define CURRENT_FORMAT_VERSION    1
#define CURRENT_FORMAT_SUBVERSION 2


/// @brief Конструктор модели дерева, состоящего из Item элементов
KnowTreeModel::KnowTreeModel(QObject *parent) : TreeModel(parent)
{
  xmlFileName="";
  rootItem=nullptr;

  connect(this, &KnowTreeModel::doCloseDetachedWindowByIdSet,
          EditorShowTextDispatcher::instance(), &EditorShowTextDispatcher::closeWindowByIdSet,
          Qt::QueuedConnection);
}


/// @todo По-хорошему деструктор перед удалением корневого элемента должен пробежать по дереву элементов и удалить их
KnowTreeModel::~KnowTreeModel() {
  delete rootItem;
}


void KnowTreeModel::initFromXML(QString fileName) {
  xmlFileName=fileName;

  // Загрузка файла и преобразование его в DOM модель
  XmlTree xmlt;
  if(!xmlt.load( xmlFileName ))
    return;

  init(xmlt.getDomModel());

  lastAccess = QDateTime::currentDateTime();
}


void KnowTreeModel::init(QDomDocument *domModel) {
  // Проверка формата XML-файла
  if( !checkFormat(domModel->documentElement().firstChildElement("format")) )
  {
    criticalError(tr("Unsupported version of the database format.\nYou need to update MyTetra."));
  }


  QMap<QString, QString> rootData;

  // Определяется одно поле в корневом объекте то есть на экране будет один столбец
  rootData["id"]="0";
  rootData["name"]="";

  beginResetModel();

  // Создание корневого Item объекта
  if(rootItem!=nullptr)
    delete rootItem;
  rootItem = new TreeItem(rootData);

  // Динамическое создание дерева из Item объектов на основе DOM модели
  setupModelData(domModel, rootItem);

  endResetModel();
}


bool KnowTreeModel::checkFormat(QDomElement elementFormat) {
  int baseVersion=0;
  int baseSubVersion=0;

  // Если DOM-элемент с версией и подверсией существует
  if( !elementFormat.isNull() )
  {
    baseVersion=elementFormat.attribute("version", "0").toInt(); // Считывается номер версии
    baseSubVersion=elementFormat.attribute("subversion", "0").toInt(); // Считывается номер подверсии
  }

  // Если номер версии или подверсии выше чем поддерживаемые программой
  if(baseVersion > CURRENT_FORMAT_VERSION || baseSubVersion > CURRENT_FORMAT_SUBVERSION)
    return false;

  // В настоящий момент поддерживается формат 1.2
  // В настоящий момент предполагается, что номер версии всегда 1, поэтому вся работа идет по номеру подверсии
  if(baseSubVersion<=1)
    if(updateSubVersionFrom1To2()==false) // Смена формата с 1.1 на 1.2
      return false;

  // На будущее, для перехода с подверсии 2 на подверсию 3, эти строки надо добавлять к существующим (а не заменять)
  // if(baseSubVersion<=2)
  //  if(updateSubVersionFrom2To3()==false)
  //   return false;

  return true;
}


bool KnowTreeModel::updateSubVersionFrom1To2(void)
{
  // Формат 1.2 только расширяет формат 1.1 (т. е. только добавляет данные, но не исключает никакие устаревшие)
  // Поэтому ни в каких преобразующих действиях нет необходимости
  return true;
}


void KnowTreeModel::reload(void) {
  initFromXML(xmlFileName);
}


// Разбор DOM модели и преобразование ее в Item модель
void KnowTreeModel::setupModelData(QDomDocument *dommodel, TreeItem *parent)
{
  QDomElement contentRootNode=dommodel->documentElement().firstChildElement("content").firstChildElement("node");

  if(contentRootNode.isNull()) {
    qDebug() << "Unable load xml tree, first content node not found.";
    return;
   }

  parseNodeElement(contentRootNode, parent);

  return;
}


// Рекурсивный обход DOM дерева и извлечение из него узлов
void KnowTreeModel::parseNodeElement(QDomElement domElement, TreeItem *iParent)
{
  TreeItem *parent = iParent;

  // У данного Dom-элемента ищется таблица конечных записей
  // и данные заполняются в Item-таблицу конечных записей
  parent->recordtableInit(domElement);

  // Пробегаются все DOM элементы текущего уровня
  // и рекурсивно вызывается обработка подуровней
  while(!domElement.isNull()) {
    if(domElement.tagName()=="node") {
      // Обнаруженый подузел прикрепляется к текущему элементу
      parent->insertChildren(parent->childCount(), 1, 1);

      // Определяются атрибуты узла дерева разделов
      QDomNamedNodeMap attributeMap = domElement.attributes();

      // Перебираются атрибуты узла дерева разделов
      QMap<QString, QString> nameAndValue;
      for(int i = 0; i<attributeMap.count(); ++i)
      {
        QDomNode attribute = attributeMap.item(i);

        nameAndValue[ attribute.nodeName() ]=attribute.nodeValue();
      }

      // В дерево разделов устанавливаются считанные атрибуты (сразу все значения)
      parent->child(parent->childCount()-1)->setAllFieldDirect(nameAndValue);

      // Вызов перебора оставшегося DOM дерева с прикреплением обнаруженных объектов
      // к только что созданному элементу
      parseNodeElement(domElement.firstChildElement(), parent->child(parent->childCount()-1) );

    }
    domElement = domElement.nextSiblingElement();
  }

}


// Генерирование полного DOM дерева хранимых данных
QDomElement KnowTreeModel::exportFullModelDataToDom(TreeItem *root)
{
  QDomDocument doc;
  QDomElement elm=doc.createElement("content");

  QElapsedTimer timer;
  timer.start();

  parseTreeToDom(&doc, &elm, root);

  qDebug() << "Parse tree to DOM elapsed time: " << timer.elapsed() << " ms";

  return elm;
}


// Генерирование полного дерева хранимых данных в поток XML
void KnowTreeModel::exportFullModelDataToStreamWriter(QXmlStreamWriter *xmlWriter, TreeItem *root)
{
  xmlWriter->writeStartElement("content");

  parseTreeToStreamWriter( xmlWriter, root);

  xmlWriter->writeEndElement(); // Закрылся content
}



// Выгрузка ветки и ее подветок в отдельную директорию
// todo: Подумать, и сделать выгрузку через режим StreamWriter.
bool KnowTreeModel::exportBranchToDirectory(TreeItem *startItem, QString exportDir)
{
  // Полный путь до создаваемого файла с деревом
  QString mytetraXmlFile=exportDir+"/mytetra.xml";


  // -----------------------------
  // Подготовка корневого элемента
  // -----------------------------

  // Создается временный корневой Item, содержащий startItem (такова особенность методов KnowTreeModel)
  QMap<QString, QString> rootData;
  rootData["id"]="0";
  rootData["name"]="";
  TreeItem *tempRootItem = new TreeItem(rootData);

  // Стартовый подузел размещается во временном корневом элементе
  tempRootItem->addChildrenItem(startItem);


  // -------------------------------------
  // Подготовка выгружаемого DOM-документа
  // -------------------------------------

  // Коструирование DOM документа для записи в файл
  QDomDocument doc=createStandartDocument();

  // Создание корневого элемента
  QDomElement rootElement=createStandartRootElement(doc);

  // Получение полного DOM дерева хранимых данных
  QDomElement elmDomTree=exportFullModelDataToDom(tempRootItem);

  // Добавление полного дерева DOM хранимых данных к корневому элементу
  rootElement.appendChild(elmDomTree);

  // Добавление корневого элемента в DOM документ
  doc.appendChild(rootElement);

  // Запись DOM данных в файл
  QFile wfile(mytetraXmlFile);
  if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    showMessageBox(tr("The exporting file %1 is not writable.").arg(mytetraXmlFile));
    return false;
  }
  QTextStream out(&wfile);
  out << doc.toString();


  // Удаление временного корневого элемента
  tempRootItem->setDetached(true); // Временный корневой элемент помечается как оторванный, чтобы не удалялись подчиненные элементы, используемые в основной программе
  delete tempRootItem; // Удаляется корневой элемент

  return true;
}


// Импорт ветки из указанной дирекотрии
// Метод возвращает ID новой созданной при импорте ветки или пустую строку, если импорт не произошел
QString KnowTreeModel::importBranchFromDirectory(TreeItem *startItem, QString importDir)
{
  QString importXmlFileName=importDir+"/mytetra.xml";

  // Проверяется наличие mytetra.xml и возможность его чтения
  QFileInfo importXmlFileInfo( importXmlFileName );
  if(!importXmlFileInfo.isReadable())
  {
    showMessageBox(tr("Import error: unable to read the XML file %1.").arg(importXmlFileName));
    return "";
  }

  // Загрузка XML файла и преобразование его в DOM модель
  XmlTree xmlt;
  if(!xmlt.load( importXmlFileName ))
    return "";


  // Создаются таблицы перекодировки идентификаторов веток, записей, имен директорий
  // Если в импортируемых данных есть ID ветки или записи, который уже есть в основной базе,
  // он должен быть заменен при экспорте на новый уникальный ID
  QMap<QString, QString> idNodeTranslate   =getAttributeTranslateTable( *(xmlt.getDomModel()), "node", "id");
  QMap<QString, QString> idRecordTranslate =getAttributeTranslateTable( *(xmlt.getDomModel()), "record", "id");
  QMap<QString, QString> dirRecordTranslate=getAttributeTranslateTable( *(xmlt.getDomModel()), "record", "dir");

  qDebug() << "Map of dirRecordTranslate: " << dirRecordTranslate;

  // Копирование каталогов с записями. При необходимости каталоги получают новые имена согласно переданной таблице трансляции
  if( !copyImportRecordDirectories( *(xmlt.getDomModel()), importDir, idRecordTranslate, dirRecordTranslate ) )
    return "";

  // Преобразование DOM-документа путем замены всех необходимых идентификаторов и названий каталогов
  translateImportDomData( *(xmlt.getDomModel()), "node", "id", idNodeTranslate);
  translateImportDomData( *(xmlt.getDomModel()), "record", "id", idRecordTranslate);
  translateImportDomData( *(xmlt.getDomModel()), "record", "dir", dirRecordTranslate);

  // Динамическое создание ветки основной базы дерева на основе DOM-данных
  beginResetModel();
  setupModelData( xmlt.getDomModel(), startItem);
  endResetModel();

  // Выясняется идентификатор только что импортированной ветки
  QString importItemId="";
  QDomNodeList nodeList=xmlt.getDomModel()->elementsByTagName("node");
  if(nodeList.count()>0)
    importItemId=nodeList.at(0).toElement().attribute("id");

  // После завершения экспорта дерево знаний записывается
  save();

  // Возвращается идентификатор только что импортированной ветки
  return importItemId;
}


/// @brief Копирование директорий импортируемой ветки в основную базу
bool KnowTreeModel::copyImportRecordDirectories( QDomDocument &doc,
                                                 QString importDir,
                                                 QMap<QString, QString> idRecordTranslate,
                                                 QMap<QString, QString> dirRecordTranslate )
{
  QDomNodeList nodeList=doc.elementsByTagName("record");
  for(int i=0; i<nodeList.count(); ++i)
  {
    QString shortFromDir=nodeList.at(i).toElement().attribute("dir");
    QString shortToDir=shortFromDir;

    // Если запись содержит нормальный атрибут dir
    if(shortFromDir.length()>0)
    {
      // Если в основной базе есть такая же директория записи
      if(dirRecordTranslate.contains(shortFromDir))
        shortToDir=dirRecordTranslate[shortFromDir]; // Директория назначения изменяется согласно таблице кодировки

      auto fullFromDir = importDir+"/base/"+shortFromDir;
      auto fullToDir = AppConfig::get().get_tetradir()+"/base/"+shortToDir;

      // Создание директории в основной базе
      if( !QDir().mkpath(fullToDir) )
        criticalError("Can't create directory "+fullToDir);

      // Копирование всех файлов из директории импортируемой записи в директорию записи основной базы
      DiskHelper::copyDirectory(fullFromDir, fullToDir);

      // В файле записи меняются внутренние ссылки формата "mytetra:" в случае, если импортируемые записи получили новые идентификаторы
      QString recordFileName=fullToDir+"/"+nodeList.at(i).toElement().attribute("file");
      Record::replaceInternalReferenceByTranslateTable(recordFileName, idRecordTranslate);
    }
    else
    {
      showMessageBox(tr("Find record id=\"%1\" without dir attribute").arg( nodeList.at(i).toElement().attribute("id") ) );
      return false;
    }
  }

  return true;
}


/// @brief Преобразование DOM-документа согласно таблицам трансляции
void KnowTreeModel::translateImportDomData( QDomDocument &doc ,
                                            QString elementName,
                                            QString elementAttribute,
                                            QMap<QString, QString> translateTable )
{
  QDomNodeList nodeList=doc.elementsByTagName( elementName );
  for(int i=0; i<nodeList.count(); ++i)
  {
    QDomElement element=nodeList.at(i).toElement();

    QString attributeValue=element.attribute(elementAttribute);
    if(attributeValue.length()>0)
      if(translateTable.contains(attributeValue))
        element.setAttribute(elementAttribute, translateTable[attributeValue]);
  }
}


// Находятся совпадающие ID в импортируемых данных и в основной базе для веток
QMap<QString, QString> KnowTreeModel::getAttributeTranslateTable(QDomDocument &doc, QString elementName, QString attributeName)
{
  QMap<QString, QString> translateTable;

  QDomNodeList nodeList=doc.elementsByTagName( elementName );
  for(int i=0; i<nodeList.count(); ++i)
  {
    QString attribute=nodeList.at(i).toElement().attribute( attributeName );
    if(attribute.length()>0)
    {
      bool isAppend=false;

      // Если проверяется ID ветки
      if(elementName=="node" && attributeName=="id")
        if(isItemIdExists(attribute)) // Если в основной базе есть ветка с таким же ID
          isAppend=true;

      // Если проверяется ID записи
      if(elementName=="record" && attributeName=="id")
        if(isRecordIdExists(attribute)) // Если в основной базе есть запись с таким же ID
          isAppend=true;

      // Если проверяется директория записи
      if(elementName=="record" && attributeName=="dir")
        if(isRecordDirExists(attribute)) // Если в основной базе есть запись с такой же директорией
          isAppend=true;

      if(isAppend)
      {
        translateTable[attribute]=getUniqueId();
        continue;
      }
    }
  }

  return translateTable;
}


// Рекурсивное преобразование Item-элементов в Dom дерево
void KnowTreeModel::parseTreeToDom(QDomDocument *doc, QDomElement *xmlData, TreeItem *currItem)
{

 // Если в ветке присутсвует таблица конечных записей
 // В первую очередь добавляется она
 if(currItem->recordtableGetRowCount() > 0)
  {
   // Обработка таблицы конечных записей

   // Получение Dom дерева таблицы конечных записей
   // В метод передается QDomDocument, на основе кторого будут создаваться элементы
   QDomElement recordTableDom=currItem->recordtableExportDataToDom( doc );

   // Dom дерево таблицы конечных записей добавляется
   // как подчиненный элемент к текущему элементу
   if(!recordTableDom.isNull())
    xmlData->appendChild(recordTableDom.cloneNode());
  }

 // Обработка каждой подчиненной ветки
 for(int i=0; i<currItem->childCount(); i++)
  {
   // Временный элемент, куда будет внесена текущая перебираемая ветка
   QDomElement  tempElement = doc->createElement("node");

   // Получение всех полей для данной ветки
   QMap<QString, QString> fields=currItem->child(i)->getAllFieldsDirect();

   // Перебираются поля элемента ветки
   QMapIterator<QString, QString> fields_iterator(fields);
   while (fields_iterator.hasNext())
    {
     fields_iterator.next();

     // Установка для временного элемента значения перебираемого поля как атрибута
     tempElement.setAttribute(fields_iterator.key(), fields_iterator.value());
    }


   // Добавление временного элемента к основному документу
   xmlData->appendChild(tempElement);

   // Рекурсивная обработка
   QDomElement workElement=xmlData->lastChildElement();
   parseTreeToDom(doc, &workElement, currItem->child(i) );
  }

}


// Рекурсивное преобразование Item-элементов в поток XML-элементов
void KnowTreeModel::parseTreeToStreamWriter( QXmlStreamWriter *xmlWriter, TreeItem *currItem)
{
  // Если в ветке присутсвует таблица конечных записей
  // В первую очередь добавляется она
  if(currItem->recordtableGetRowCount() > 0)
   currItem->recordtableExportDataToStreamWriter( xmlWriter );

  // Обработка каждой подчиненной ветки
  for(int i=0; i<currItem->childCount(); i++)
   {
    xmlWriter->writeStartElement("node");

    // Получение всех полей для данной ветки
    QMap<QString, QString> fields=currItem->child(i)->getAllFieldsDirect();

    // Перебираются поля элемента ветки
    QMapIterator<QString, QString> fields_iterator(fields);
    while (fields_iterator.hasNext())
     {
      fields_iterator.next();

      // Поля сохраняются как атрибуты элемента
      xmlWriter->writeAttribute(fields_iterator.key(), fields_iterator.value());
     }

    // Рекурсивная обработка вложенных узлов (веток)
    parseTreeToStreamWriter(xmlWriter, currItem->child(i) );

    xmlWriter->writeEndElement(); // Закрылся node
   }
}


/// @brief Запись всех данных в XML файл
void KnowTreeModel::save() {
  // Если имя файла не было проинициализировано
  if(xmlFileName=="")
    criticalError("In KnowTreeModel can't set file name for XML file");

  // Создается новый файл дерева
  QFile writeFile(xmlFileName);
  if (!writeFile.open(QIODevice::WriteOnly)) // implicitly truncate file
    criticalError("Can't open file "+xmlFileName+" for write.");

  // Создание объекта потоковой генерации XML-данных в файл
  QXmlStreamWriter xmlWriter(&writeFile);
  xmlWriter.setAutoFormatting(true);
  xmlWriter.setAutoFormattingIndent(1);

  // Начало документа
  xmlWriter.writeStartDocument();

  // Doctype заголовок
  xmlWriter.writeDTD("<!DOCTYPE mytetradoc>"); // Пишется в явном виде

  // Начало корневого элемента
  xmlWriter.writeStartElement("root");

  // Элемент версии данных
  xmlWriter.writeStartElement("format");
  xmlWriter.writeAttribute("version", QString::number( CURRENT_FORMAT_VERSION ));
  xmlWriter.writeAttribute("subversion", QString::number(CURRENT_FORMAT_SUBVERSION));
  xmlWriter.writeEndElement();

  exportFullModelDataToStreamWriter( &xmlWriter, rootItem );

  // Завершение корневого элемента
  xmlWriter.writeEndElement();

  // Завершение документа
  xmlWriter.writeEndDocument();


  writeFile.close();

  lastAccess = QDateTime::currentDateTime();
}


QDomDocument KnowTreeModel::createStandartDocument()
{
  QDomDocument doc("mytetradoc");

  // Установка заголовка
  doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

  return doc;
}


QDomElement KnowTreeModel::createStandartRootElement(QDomDocument &doc)
{
  // Создание корневого элемента
  QDomElement rootElement=doc.createElement("root");

  // Добавление формата версии к корневому элементу
  QDomElement format=doc.createElement("format");
  format.setAttribute("version",CURRENT_FORMAT_VERSION);
  format.setAttribute("subversion",CURRENT_FORMAT_SUBVERSION);
  rootElement.appendChild(format);

  return rootElement;
}


// Добавление новой подветки к указанной ветке
void KnowTreeModel::addNewChildBranch(const QModelIndex &index, QMap<QString, QString> branchFields)
{
  // Получение ссылки на Item элемент по QModelIndex
  TreeItem *parent=getItem(index);

  beginInsertRows(index, parent->childCount(), parent->childCount());
  addNewBranch(parent, branchFields);
  endInsertRows();
}


// Добавление новой ветки после указанной ветки
void KnowTreeModel::addNewSiblingBranch(const QModelIndex &index, QMap<QString, QString> branchFields)
{
  // Получение ссылки на родительский Item элемент по QModelIndex
  TreeItem *current=getItem(index);
  TreeItem *parent=current->parent();

  beginInsertRows(index.parent(), parent->childCount(), parent->childCount());
  addNewBranch(parent, branchFields);
  endInsertRows();
}


// Добавление новой подветки к Item элементу
void KnowTreeModel::addNewBranch(TreeItem *parent, QMap<QString, QString> branchFields)
{
  // Прикрепляется пустой подузел в конец списка подчиненных элементов
  parent->addChildrenEmpty();

  // Определяется ссылка на только что созданную ветку
  TreeItem *newBranch=parent->child(parent->childCount()-1);

  // Обязательно должны быть установлены поля id и name
  if(!branchFields.contains("id"))
    criticalError("In KnowTreeModel::addNewBranch() can't setted ID field");
  if(!branchFields.contains("name"))
    criticalError("In KnowTreeModel::addNewBranch() can't setted name field");

  // Перебираются поля новой ветки и их значения
  foreach(QString fieldName, branchFields.keys())
  {

    // Добавляются только поля, разрешенные для ветки. Вспомогательные поля отбрасываются
    if(!FixedParameters::itemFieldAvailableList.contains(fieldName))
      continue;

    QString value=branchFields.value(fieldName);
    newBranch->setField(fieldName, value); // Новой ветке устанавливается поле и значение поля
  }

  // Инициализируется таблица конечных записей
  newBranch->recordtableGetTableData()->init(newBranch, QDomElement());
}


// Добавление новой подветки к указанной ветке из буфера обмена
QString KnowTreeModel::pasteNewChildBranch(const QModelIndex &index, ClipboardBranch *subbranch)
{
  QString pasted_branch_id;

  // Получение ссылки на Item элемент по QModelIndex
  TreeItem *parent=getItem(index);

  beginInsertRows(index, parent->childCount(), parent->childCount());
  pasted_branch_id=pasteSubbranch(parent, (ClipboardBranch *)subbranch);
  endInsertRows();

  return pasted_branch_id;
}


QString KnowTreeModel::pasteNewSiblingBranch(const QModelIndex &index, ClipboardBranch *subbranch)
{
  QString pasted_branch_id;

  // Получение ссылки на родительский Item элемент по QModelIndex
  TreeItem *current=getItem(index);
  TreeItem *parent=current->parent();

  beginInsertRows(index.parent(), parent->childCount(), parent->childCount());
  pasted_branch_id=pasteSubbranch(parent, (ClipboardBranch *)subbranch);
  endInsertRows();

  return pasted_branch_id;
}


// Перемещение ветки вверх
QModelIndex KnowTreeModel::moveUpBranch(const QModelIndex &index)
{
  return moveUpDownBranch(index,1);
}


// Перемещение ветки вниз
QModelIndex KnowTreeModel::moveDownBranch(const QModelIndex &index)
{
  return moveUpDownBranch(index,-1);
}


// Перемещение ветки вверх или вниз
QModelIndex KnowTreeModel::moveUpDownBranch(const QModelIndex &index,int direction)
{
  // Получение QModelIndex расположенного над или под элементом index
  QModelIndex swap_index=index.sibling(index.row()-direction,0);

  // Проверяется допустимость индекса элемента, куда будет сделано перемещение
  if(!swap_index.isValid())
    return QModelIndex(); // Возвращается пустой указатель

  // Запоминаются параметры для абсолютного позиционирования засветки
  // после перемещения ветки
  int         swpidx_row=swap_index.row();
  int         swpidx_column=swap_index.column();
  QModelIndex swpidx_parent=swap_index.parent();

  // Получение ссылки на Item элемент по QModelIndex
  TreeItem *current=getItem(index);

  // Перемещается ветка
  emit layoutAboutToBeChanged();

  bool moveok;
  if(direction==1) moveok=current->moveUp();  // Перемещение в Item представлении
  else             moveok=current->moveDn();

  if(moveok)
  {
    changePersistentIndex(swap_index,index);
    changePersistentIndex(index,swap_index);
  }

  emit layoutChanged();

  // Возвращается указатель на перемещенную ветку
  if(moveok) return this->index(swpidx_row, swpidx_column, swpidx_parent);
  else return QModelIndex(); // Возвращается пустой указатель
}


// Получение индекса подчиненного элемента с указанным номером
// Нумерация элементов считается что идет с нуля
QModelIndex KnowTreeModel::indexChildren(const QModelIndex &parent, int n) const
{
  // Проверяется, передан ли правильный QModelIndex
  // Если он неправильный, возвращается пустой индекс
  if(!parent.isValid())
  {
    qDebug() << "In indexChildren() unavailable model index";
    return QModelIndex();
  }

  // Выясняется указатель на основной Item элемент
  TreeItem *item = getItem(parent);

  // Если у основного Item элемента запрашивается индекс
  // несуществующего подэлемента, возвращается пустой индекс
  if( n<0 || n>( (int)(item->childCount())-1) )
  {
    qDebug() << "In indexChildren() unavailable children number";
    return QModelIndex();
  }
  
  // Выясняется указатель на подчиненный Item элемент
  TreeItem *childitem = item->child(n);

  // Если указатель на подчиненный Item элемент непустой
  if(childitem)
  {
    // return createIndex(0, 0, childitem); // Индекс подчиненного элемента
    // return createIndex(n, 0, parent.internalPointer());

    return index(n,0,parent);
  }
  else
  {
    qDebug() << "In indexChildren() empty child element";
    return QModelIndex(); // Возвращается пустой индекс
  }
}


// Получение QModelIndex по известному TreeItem
QModelIndex KnowTreeModel::getIndexByItem(TreeItem *item)
{
  // Данный метод используется только функциями видов, поэтому скорость от него не требуется, можно вводить любые проверки
  if(item)
  {
    if(item!=rootItem)
    {
      int itemrow=item->childNumber();
      return this->createIndex(itemrow, 0, item);
    }
    else
      return QModelIndex(); // Индекс для корня
  }
  else
  {
    return QModelIndex(); // Возвращается некорректный индекс модели
  }
}


// Возвращает общее количество записей, хранимых в дереве
int KnowTreeModel::getAllRecordCount(void)
{
  // Обнуление счетчика
  getAllRecordCountRecurse(rootItem, 0);

  return getAllRecordCountRecurse(rootItem, 1);
}


// Возвращает количество записей в ветке и всех подветках
int KnowTreeModel::getRecordCountForItem(TreeItem *item)
{
  // Обнуление счетчика
  getAllRecordCountRecurse(rootItem, 0);

  return getAllRecordCountRecurse(item, 1);
}


int KnowTreeModel::getAllRecordCountRecurse(TreeItem *item, int mode)
{
  static int n=0;

  if(mode==0)
  {
    n=0;
    return 0;
  }

  n=n+item->recordtableGetRowCount();

  for(int i=0; i < item->childCount(); i++)
    getAllRecordCountRecurse(item->child(i), 1);

  return n;
}


// Проверка наличия идентификатора ветки во всем дереве
bool KnowTreeModel::isItemIdExists(QString findId) const
{
  // Обнуление счетчика
  isItemIdExistsRecurse(rootItem, findId, 0);

  return isItemIdExistsRecurse(rootItem, findId, 1);
}


bool KnowTreeModel::isItemIdExistsRecurse(TreeItem *item, QString findId, int mode) const
{
  static bool isExists=false;

  // Инициализация
  if(mode==0)
  {
    isExists=false;
    return false;
  }

  // Если ветка найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
  if(isExists)
    return true;

  // Если текущая ветка содержит искомый идетнификатор
  if(item->getField("id")==findId)
  {
    isExists=true;
    return true;
  }

  // Перебираются подветки
  for(int i=0; i < item->childCount(); i++)
    isItemIdExistsRecurse(item->child(i), findId, 1);

  return isExists;
}


// Проверка наличия идентификатора записи во всем дереве
bool KnowTreeModel::isRecordIdExists(QString findId) const
{
  // Обнуление флага наличия записи с указанным ID
  isRecordIdExistsRecurse(rootItem, findId, 0);

  return isRecordIdExistsRecurse(rootItem, findId, 1);
}


bool KnowTreeModel::isRecordIdExistsRecurse(TreeItem *item, QString findId, int mode) const
{
  static bool isExists=false;

  // Инициализация
  if(mode==0)
  {
    isExists=false;
    return false;
  }

  // Если запись найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
  if(isExists)
    return true;

  // Если таблица записей текущей ветки содержит искомый идентификатор
  if( item->recordtableGetTableData()->isRecordExists(findId) )
  {
    isExists=true;
    return true;
  }

  // Перебираются подветки
  for(int i=0; i < item->childCount(); i++)
    isRecordIdExistsRecurse(item->child(i), findId, 1);

  return isExists;
}


// Получение списка всех идентификаторов записей в ветке и ее подветках
QSharedPointer< QSet<QString> > KnowTreeModel::getRecordsIdList(TreeItem *item) const
{
    QSharedPointer< QSet<QString> > idListPointer( new QSet<QString> );

    this->getAllRecordsIdListRecurse(item, idListPointer.data() );

    return idListPointer;
}


// Получение списка всех идентификаторов записей в базе
QSharedPointer< QSet<QString> > KnowTreeModel::getAllRecordsIdList() const
{
    QSharedPointer< QSet<QString> > idListPointer( new QSet<QString> );

    this->getAllRecordsIdListRecurse(rootItem, idListPointer.data() );

    return idListPointer;
}


void KnowTreeModel::getAllRecordsIdListRecurse(TreeItem *item, QSet<QString> *ids) const
{
    // Множество идентификаторов записей текущей ветки добавляется в итоговое множество
    ids->unite( item->recordtableGetTableData()->getRecordsIdList() );

    // Перебираются подветки
    for(int i=0; i < item->childCount(); i++)
      getAllRecordsIdListRecurse(item->child(i), ids);
}


void KnowTreeModel::deleteItemsByModelIndexList(QModelIndexList &selectItems)
{
    // Сортировка списка индексов по вложенности методом пузырька
    // Индексы с длинным путем перемещаются в начало списка
    for(int i = 0; i < selectItems.size(); i++)
     for(int j=selectItems.size()-1; j>i; j--)
      {
       QStringList path_1=(this->getItem(selectItems.at(j-1)))->getPath();
       QStringList path_2=(this->getItem(selectItems.at(j)))->getPath();
       if(path_1.size() < path_2.size())
           selectItems.swapItemsAt(j-1, j);
      }

    qDebug() << "Path for delete";
    for(int i = 0; i < selectItems.size(); ++i)
     qDebug() << (this->getItem(selectItems.at(i)))->getPath();

    QSet< QString > deleteResordsId;

    // Вызов удаления веток
    for(int i = 0; i < selectItems.size(); ++i)
    {
        // Собирается список удаляемых записей, чтобы закрыть открепляемые окна
        // у записей, которые были удалены
        deleteResordsId.unite( *(this->getRecordsIdList(this->getItem(selectItems.at(i))).data()) );

        this->deleteOneBranch(selectItems.at(i));
    }

    // Закрываются открепляемые окна для удаленных записей
    emit doCloseDetachedWindowByIdSet( deleteResordsId );
}


// Удаление одной ветки и её подветок
void KnowTreeModel::deleteOneBranch(QModelIndex index)
{

 if(!index.isValid())
   return;

 // Получение узла, который соответствует обрабатываемому индексу
 TreeItem *item=this->getItem(index);

 // qDebug() << "Delete tree item id:" << item->getField("id") << "name:" << item->getField("name");

 // Получение пути к элементу
 QStringList path=item->getPath();

 // Получение путей ко всем подветкам
 QList<QStringList> subbranchespath=item->getAllChildrenPath();

 // Сортировка массива веток по длине пути
 std::sort(subbranchespath.begin(), subbranchespath.end(), [](const auto & a, const auto & b){return a.size() < b.size();});

 // Удаление всех таблиц конечных записей для нужных подветок
 // Удаление всех подчиненных элементов для нужных подветок
 // Вначале пробегаются самые длинные ветки а потом более короткие
 for(int i=subbranchespath.size()-1;i>=0;i--)
 {
  qDebug() << "Delete subbranch, id:" << this->getItem(subbranchespath.at(i))->getField("id") << "name:" << this->getItem(subbranchespath.at(i))->getField("name");
  ( this->getItem(subbranchespath.at(i)) )->recordtableDeleteAllRecords();
  // ( knowTreeModel->getItem(subbranchespath.at(i)) )->removeAllChildren(); // Команда действительно не нужна. Далее в removeRows() вызывается removeChildren()
 }

 // Удаление таблицы конечных записей для самой удаляемой ветки
 // Удаление подчиненных элементов для самой удаляемой ветки
 qDebug() << "Delete rootbranch, id:" << this->getItem(path)->getField("id") << "name:" << this->getItem(path)->getField("name");
 ( this->getItem(path) )->recordtableDeleteAllRecords();
 // ( knowTreeModel->getItem(path) )->removeAllChildren(); // Команда действительно не нужна. Далее в removeRows() вызывается removeChildren()

 // Удаление ветки на экране, при этом удалятся все подветки
 qDebug() << "This branch have row() as" << index.row();
 if(index.isValid()) qDebug() << "Index valid";
 else qDebug() << "Index non valid";
 this->removeRows(index.row(), 1, index.parent());

 /*
 // Удаление всех нужных подветок
 // Пробегаются самые длинные ветки а потом более короткие
 for (int i=subbranchespath.size()-1;i>=0;i--)
  if(knowTreeModel->isItemValid(subbranchespath.at(i)))
   {
    TreeItem *current_item=knowTreeModel->getItem(subbranchespath.at(i));
    delete current_item;
   }
 */
}


// Проверка наличия имени директории записи во всем дереве
bool KnowTreeModel::isRecordDirExists(QString findDir) const
{
  // Обнуление счетчика
  isRecordDirExistsRecurse(rootItem, findDir, 0);

  return isRecordDirExistsRecurse(rootItem, findDir, 1);
}


bool KnowTreeModel::isRecordDirExistsRecurse(TreeItem *item, QString findDir, int mode) const
{
  static bool isExists=false;

  // Инициализация
  if(mode==0)
  {
    isExists=false;
    return false;
  }

  // Если запись найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
  if(isExists)
    return true;

  // Если в какой-нибудь записи текущей ветки содержится искомое имя директории
  for(unsigned int i=0; i<item->recordtableGetTableData()->size(); ++i)
    if( item->recordtableGetTableData()->getRecord( static_cast<int>(i) )->getField("dir")==findDir )
    {
      isExists=true;
      return true;
    }

  // Перебираются подветки
  for(int i=0; i < item->childCount(); i++)
    isRecordDirExistsRecurse(item->child(i), findDir, 1);

  return isExists;
}


// Добавление подветки из буфера обмена относительно указанного элемента
// Функция возвращает новый идентификатор стартовой добавленной подветки
QString KnowTreeModel::pasteSubbranch(TreeItem *item, ClipboardBranch *subbranch)
{
  qDebug() << "In paste_subbranch()";

  // Выясняется линейная структура добавляемого дерева
  QList<CLIPB_TREE_ONE_LINE> tree=subbranch->getIdTree();

  // Идентификатор стартовой ветки лежит в первом элементе списка
  QString start_branch_id=tree[0].branch_id;

  return pasteSubbranchRecurse(item, start_branch_id, subbranch);
}


// Рекурсивное добавление дерева
// item - элемент главного дерева, к которому добавляется ветка
// start_branch_id - идентификатор ветки в переданном линейном представлении
//                   добавляемого дерева
// subbranch - добавляемое дерево
QString KnowTreeModel::pasteSubbranchRecurse(TreeItem *item,
                                             QString startBranchId,
                                             ClipboardBranch *subbranch)
{
  qDebug() << "In paste_subbranch_recurse()";

  // ---------------------------
  // Добавляется стартовая ветка
  // ---------------------------

  // Получение уникального идентификатора, под которым будет добавляться ветка
  QString id=getUniqueId();

  // Выясняются поля стартовой ветки
  QMap<QString, QString> subbranchFields=subbranch->getBranchFieldsById(startBranchId);

  // В полях заменяется идентификатор ветки на новый уникальный
  subbranchFields["id"]=id;

  // Стартовая ветка добавляется
  addNewBranch(item, subbranchFields);

  // Выясняется указатель на эту добавленную ветку
  TreeItem *newitem=getItemById(id);


  qDebug() << "KnowTreeModel::paste_subbranch_recurse() : create tree item with field" << newitem->getAllFields();

  // -----------------------------------------------
  // Для стартовой ветки добавляются конечные записи
  // -----------------------------------------------

  // Выясняются данные конечных записей
  QList< Record > records=subbranch->getBranchRecords(startBranchId);

  foreach(Record record, records)
  {
    qDebug() << "Add table record "+record.getField("name");
    newitem->recordtableGetTableData()->insertNewRecord(GlobalParameters::AddNewRecordBehavior::ADD_TO_END,
                                                        0,
                                                        record);
  }

  // --------------------
  // Добавляются подветки
  // --------------------

  // Выясняется линейная структура добавляемого дерева
  QList<CLIPB_TREE_ONE_LINE> tree=subbranch->getIdTree();

  // Выясняется список подветок для стартовой ветки
  QStringList subbranch_list;
  foreach(CLIPB_TREE_ONE_LINE one_line, tree)
    if(one_line.branch_id==startBranchId)
      subbranch_list=one_line.subbranches_id;

  foreach(QString current_subbranch, subbranch_list)
    pasteSubbranchRecurse(newitem, current_subbranch, subbranch);

  return id;
}


// Функция ищет ветку с указанным ID и возвращает ссылку не неё в виде TreeItem *
// Если ветка с указанным ID не будет найдена, возвращается NULL
TreeItem *KnowTreeModel::getItemById(const QString &id)
{
    // Инициализация поиска
    getItemByIdRecurse(rootItem, "", 0);

    // Запуск поиска и возврат результата
    return getItemByIdRecurse(rootItem, id, 1);
}


TreeItem *KnowTreeModel::getItemByIdRecurse(TreeItem *item, const QString &id, int mode)
{
    static TreeItem *findItem=nullptr;

    if(mode==0)
    {
        findItem=nullptr;
        return nullptr;
    }

    if(findItem!=nullptr)
    {
        return findItem;
    }

    if(item->getId()==id)
    {
        findItem=item;
    }
    else
    {
        for(int i=0; i < item->childCount(); i++)
            getItemByIdRecurse(item->child(i), id, 1); // Рекурсивный поиск с установкой findItem в момент когда запись найдена
    }

    return findItem;
}


// Получение указателя на запись по известному идентификатору записи
Record *KnowTreeModel::getRecord(const QString &recordId)
{
    QStringList path=this->getRecordPath(recordId);

    return getItem(path)->recordtableGetTableData()->getRecordById(recordId);
}


// Получение пути к ветке, где находится запись
QStringList KnowTreeModel::getRecordPath(const QString &recordId)
{
    QStringList path;

    getRecordPathRecurse(rootItem, path, "0", 0);

    return getRecordPathRecurse(rootItem, path, recordId, 1);
}


QStringList KnowTreeModel::getRecordPathRecurse(TreeItem *item,
                                                QStringList &currentPath,
                                                const QString &recordId,
                                                int mode)
{
    static QStringList findPath;

    if(mode==0)
    {
        findPath.clear();
        return QStringList();
    }

    if(findPath.size()!=0)
    {
        return findPath;
    }

    // Если в данной ветке есть искомая запись
    if( item->recordtableGetTableData()->isRecordExists(recordId) )
    {
        findPath=currentPath;
        findPath << item->getId();
    }
    else
    {
        // Иначе перебираются подветки
        for(int i=0; i < item->childCount(); i++)
        {
            // Рекурсивный поиск с установкой findPath в момент когда запись найдена
            currentPath << item->getId();
            getRecordPathRecurse(item->child(i), currentPath, recordId, 1);
            currentPath.removeLast();
        }
    }

    return findPath;
}


// Метод определяющий есть ли в элементе и подэлементах заблокированные записи
bool KnowTreeModel::isItemContainsBlockRecords(TreeItem *item)
{
  isContainsBlockRecordsRecurse(item, 0);

  return isContainsBlockRecordsRecurse(item, 1);
}


bool KnowTreeModel::isContainsBlockRecordsRecurse(TreeItem *item, int mode)
{
  static bool isBlock=false;

  if(mode==0)
  {
    isBlock=false;
    return isBlock;
  }

  // Если таблица записей текущей ветки содержит заблокированную запись
  if(item->recordtableGetTableData()->isBlockRecordExists())
  {
    isBlock=true;
    return isBlock;
  }

  for(int i=0; i < item->childCount(); i++)
    isContainsBlockRecordsRecurse(item->child(i), 1);

  return isBlock;
}
