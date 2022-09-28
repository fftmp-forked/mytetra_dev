#pragma once

#include <QObject>

class Editor;
class EditorConfig;
class EditorTextArea;


/** @brief
 Основной класс "форматирОвщиков" текста. От него наследуются все форматировщики
 По сути, этот класс просто содержит указатель на область редактирования
 Унаследованные от него форматировщики реализуют ограниченную область форматирования:
 форматирование начертания, форматирование размещения текста, форматирование картинок и т. д.
**/
class Formatter : public QObject
{
  Q_OBJECT

public:
  explicit Formatter(QObject *parent = nullptr);
  virtual ~Formatter();

  void setEditor(Editor *iEditor);
  void setTextArea(EditorTextArea *iTextArea);

protected:

  Editor *editor;
  EditorConfig *editorConfig;

  EditorTextArea *textArea;

};

