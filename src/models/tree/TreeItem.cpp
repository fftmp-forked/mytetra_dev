#include <QStringList>
#include <QString>
#include <QMap>

#include "TreeItem.h"
#include "libraries/FixedParameters.h"
#include "../appConfig/AppConfig.h"
#include "libraries/helpers/DebugHelper.h"


TreeItem::TreeItem(const QMap<QString, QString> &data, TreeItem *parent) {
  detachedState=false; // По-умолчанию элемент полноценный, не оторванный

  parentItem = parent;
  fieldsTable = data;
}


TreeItem::~TreeItem() {
 // Вызывается процедура очищения ветки без физического удаления данных на диске
 empty();
}


// Возвращает номер, под которым данный объект хранится
// в массиве childItems своего родителя
int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}


// Метод мягкого удаления всех данных из узла
// При таком удалении полностью очищается сам узел, 
// а физические данные на диске не затрагиваются
void TreeItem::empty(void)
{
    fieldsTable.clear();

    recordsTable.empty();

    // Если элемент не является "оторванным"
    if(detachedState==false)
        qDeleteAll(childItems); // Удаляются все подветки

    parentItem=nullptr;
}


QString TreeItem::getField(QString name) {
    // Если запрашивается динамическое имя из имени и количества потомков
    if(name=="dynamicname")
    {
        // Имя ветки
        QString itemName=fieldsTable["name"];

        // Выясняется, есть ли у текущего элемента конечные записи
        int recordCount=this->recordtableGetRowCount();

        // Если конечных элементов нет, возвращатся просто имя
        if(recordCount==0)
            return itemName;
        else
        {
            // Иначе конечные элементы есть, возвращается имя записи
            // и количество конечных элементов
            QString r,i;
            r=itemName+" ["+i.setNum(recordCount)+"]";
            return r;
        }
    }


    // Если имя поля допустимо
    if(FixedParameters::itemFieldAvailableList.contains(name))
    {
        // Если поле с таким именем существует
        if(fieldsTable.contains(name))
            return fieldsTable[name];
        else
            return QString(""); // Если поле не существует, возвращается пустая строка
    }
    else
    {
        criticalError("In TreeItem::get_field() unavailable name '"+name+"'");
    }

    return "";
}


// Получение всех установленных полей "имя_поля"->"значение"
QMap<QString, QString> TreeItem::getAllFields() {
  qDebug() << "TreeItem::getAllFields() : Fields data " << fieldsTable;

  QMap<QString, QString> result;

  QList<QString> names=fieldsTable.keys();

  foreach(QString name, names)
  {
    // В результат добаляются только параметры с разрешенным именем
    if(FixedParameters::itemFieldAvailableList.contains(name))
      result[name]=getField(name);
  }

  return result;
}


void TreeItem::setField(QString name, QString value) {
  // Если имя поля допустимо
  if(FixedParameters::itemFieldAvailableList.contains(name))
  {
    // Если это иконка
    if(name=="icon")
    {
      if(value.length()>0)
        icon=QIcon(AppConfig::get().get_tetradir()+"/"+FixedParameters::iconsRelatedDirectory+"/"+value); // Изображение иконки кешируется
      else
        icon=QIcon(); // Изображение иконки обнуляется
    }

    // qDebug() << "Set to item data " << name << value;
    fieldsTable[name]=value;
  }
  else
    criticalError("TreeItem::setField() : Set unavailable field \""+ name +"\" to tree item");
}


// Установка данных напрямую - какие данные переданы, те и запомнятся
// Метод используется в одном месте - при инициализации дерева из XML файла
void TreeItem::setAllFieldDirect(const QMap<QString, QString> nameAndValue)
{
  foreach(QString name, nameAndValue.keys())
    if( !FixedParameters::itemFieldAvailableList.contains(name) )
      criticalError("TreeItem::setFieldDirect() : Set unavailable field \""+ name +"\" to tree item");

  // Устанавливаются значения полей
  fieldsTable=nameAndValue; // Qt сам должен правильно сделать привязку к переданным данным и оставить их в памяти

  // Если есть иконка, изображение иконки кешируется
  if(nameAndValue.value("icon").length()>0)
    icon=QIcon(AppConfig::get().get_tetradir()+"/"+FixedParameters::iconsRelatedDirectory+"/"+nameAndValue.value("icon"));

  if(nameAndValue.value("icon").length()==0)
    icon=QIcon();
}


QString TreeItem::getId()
{
    if(fieldsTable.contains("id"))
        return (fieldsTable["id"]);
    else
        criticalError("In TreeItem data getting field with unavailable name 'id'");

    return "";
}


QString TreeItem::getParentId() {
  if(parentItem)
    return parentItem->getField("id");
  else
    return "";
}


// Добавление новых подчиненных элементов
// position - позиция в списке подчиненных элементов для вставки элементов
// count - сколько новых элементов будет вставлено
// columns - сколько колонок содержит добавляемый элемент
bool TreeItem::insertChildren(int position, int count, int columns)
{
  Q_UNUSED(columns);

  if(position < 0 || position > childItems.size())
    return false;

  for(int row = 0; row < count; ++row)
  {
    QMap<QString, QString> data;
    TreeItem *item = new TreeItem(data, this); // Создается объект item
    childItems.insert(position, item); // Вставка item в нужную позицию массива childItems
  }

  return true;
}


// Добавление нового пустого подчиненного элемента
// в конец списка подчиненных элементов
bool TreeItem::addChildrenEmpty(void)
{
  QMap<QString, QString> data;

  TreeItem *item = new TreeItem(data, this); // Создается объект item

  childItems << item; // Добавление item в конец массива childItems

  return true;
}


// Добавление уже существующего Item-элемента
bool TreeItem::addChildrenItem(TreeItem *item)
{
  childItems << item; // Добавление item в конец массива childItems
  return true;
}


bool TreeItem::removeChildren(int position, int count)
{
  if(position < 0 || position + count > childItems.size())
    return false;

  for(int row = 0; row < count; ++row)
    delete childItems.takeAt(position);

  return true;
}


void TreeItem::removeAllChildren()
{
  for(int i=0; i< childItems.size(); i++)
    delete childItems.takeAt(0);
}


bool TreeItem::removeChildrenLink(int position, int count)
{
  if(position < 0 || position + count > childItems.size())
    return false;

  // Ссылка на удаленный элемент убирается из списка подчиненных элементов
  for (int row = 0; row < count; ++row)
    childItems.removeAt(position);

  return true;
}


bool TreeItem::moveUp(void)
{
  // Выясняется номер данного элемента в списке родителя
  int num=childNumber();

  // Если двигать вверх некуда, ничего делать не нужно
  if(num==0)return false;

  // Элемент перемещается вверх по списку
  ( parentItem->childItems ).swapItemsAt(num,num-1);

  return true;
}


bool TreeItem::moveDn(void)
{
    // Выясняется номер данного элемента в списке родителя
    int num=childNumber();

    // Если двигать вниз некуда, ничего делать не нужно
    if(num>=(parentItem->childCount()-1))
        return false;

    // Элемент перемещается вниз по списку
    ( parentItem->childItems ).swapItemsAt(num,num+1);

    return true;
}


// Путь к элементу в виде строки, разделенной указанным разделителем
QString TreeItem::getPathAsNameWithDelimeter(QString delimeter)
{
  QStringList path=getPathAsName();

  // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
  int emptyStringIndex=path.indexOf("");
  path.removeAt(emptyStringIndex);

  return path.join(delimeter);
}


QStringList TreeItem::getPathAsField(QString fieldName)
{
    QStringList path;
    TreeItem *currentItem=this;

    path << currentItem->getField(fieldName);

    while(currentItem->parent()!=nullptr)
    {
        currentItem=currentItem->parent();
        path << currentItem->getField(fieldName);
    }

    // Поворот массива идентификаторов задом наперед
    std::reverse(path.begin(), path.end());

    return path;
}


// Возвращает массив путей всех подветок, которые содержит ветка
QList<QStringList> TreeItem::getAllChildrenPath(void)
{
    // Очищение списка путей
    getAllChildrenPathAsFieldRecurse(this, "", 0);

    // Получение списка путей
    QList<QStringList> pathList=getAllChildrenPathAsFieldRecurse(this, "id", 1);

    return pathList;
}


QList<QStringList> TreeItem::getAllChildrenPathAsField(QString fieldName)
{
    // Очищение списка путей
    getAllChildrenPathAsFieldRecurse(this, "", 0);

    // Получение списка путей
    QList<QStringList> pathList=getAllChildrenPathAsFieldRecurse(this, fieldName, 1);

    return pathList;
}


// Возвращает массив указанных полей всех подветок, которые содержит ветка
// Внутренняя рекурсивная функция
QList<QStringList> TreeItem::getAllChildrenPathAsFieldRecurse(TreeItem *item, QString fieldName, int mode)
{
  static QList<QStringList> pathList;

  // Если дана команда очистить список путей
  if(mode==0)
  {
    pathList.clear();
    return QList<QStringList>();
  }

  for(int i=0; i<(item->childCount()); i++)
  {
    QStringList path=(item->child(i))->getPathAsField(fieldName);
    pathList << path;
    getAllChildrenPathAsFieldRecurse(item->child(i), fieldName, 2);
  }

  if(mode==1)return pathList;
  else return QList<QStringList>();
}
