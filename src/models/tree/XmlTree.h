#pragma once

#include <QObject>
#include <QtXml>
#include <QTreeWidgetItem>


class XmlTree : public QObject
{
 Q_OBJECT

 public:
  XmlTree(void) {domModel=new QDomDocument();}
  ~XmlTree(void) {delete domModel;}
  bool load(QString file);
  QDomDocument* getDomModel(void) const {return domModel;}
  
 protected:
  QDomDocument *domModel; // DOM-представление документа
};


