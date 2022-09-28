#pragma once

#include <QTextImageFormat>
#include <QTextDocumentFragment>

#include "Formatter.h"


/// @brief Класс для работы с математическими выражениями
class MathExpressionFormatter : public Formatter
{
  Q_OBJECT

public:
  MathExpressionFormatter();

  QString mathExpressionOnSelect(void);
  QString mathExpressionOnCursor(void);

  void createGifFromMathExpression(QString iMathExpression, QString iFileName);

signals:

public slots:

  void onMathExpressionClicked(void);
  void onContextMenuEditMathExpression(void);
  void onDoubleClickOnImage(void);

protected:

  void addMathExpression(void);
  void editMathExpression(QString iMathExpressionText);

  QString getMathExpressionFromUser(QString iMathExpressionText="");
  void insertMathExpressionToTextArea(QString iMathExpressionText);

  QString getMathExpressionByImageName(QString resourceImageName);

private:
  QString m_mimetex_bin;
};

