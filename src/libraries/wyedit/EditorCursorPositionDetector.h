#pragma once

#include <QTextImageFormat>

class EditorTextArea;

/// @brief Вспомогательный класс, определяющий положение/состояние курсора в тексте в зависимости от окружающего форматирования
class EditorCursorPositionDetector {

  public:
    EditorCursorPositionDetector(){};

    void setTextArea(EditorTextArea *iTextArea) { textArea = iTextArea; }

    bool isCursorOnImage();
    bool isImageSelect();
    QTextImageFormat getImageSelectFormat();
    int whereImageAtCursor();

    bool isCursorOnMathExpression();
    bool isMathExpressionSelect();

    bool isCursorOnSpaceLine();
    bool isCursorOnEmptyLine();
    bool isBlockSelect();

    bool isCursorOnReference();
    QString referenceHref();

  private:
    EditorTextArea *textArea;

    bool isCursorOnReferenceSmart(QString &resultHref);
    bool isMathExpressionSmartDetect(QString &resourceName);
};
