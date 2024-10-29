#include <QDebug>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextFragment>
#include <QTextImageFormat>

#include "../FixedParameters.h"
#include "Editor.h"
#include "EditorCursorPositionDetector.h"
#include "EditorTextArea.h"

// Проверка, находится ли курсор на позиции, где находится картинка
bool EditorCursorPositionDetector::isCursorOnImage() {
    // Проверка срабатывает только если нет выделения
    if (textArea->textCursor().hasSelection())
        return false;

    auto imageFormat = textArea->textCursor().charFormat().toImageFormat();
    return imageFormat.isValid();
}

/// @brief Метод, определяющий, выбрана ли только одна картинка
bool EditorCursorPositionDetector::isImageSelect() {
    auto format = getImageSelectFormat();
    return format.isImageFormat() && format.isValid() && format.name().size() > 0;
}

/// @brief Метод, получающий информацию о выбранной выделением одной картинке
QTextImageFormat EditorCursorPositionDetector::getImageSelectFormat() {
    // Происходит анализ, выделена ли картинка

    // Блок, в пределах которого находится курсор
    auto currentBlock = textArea->textCursor().block();

    if (!textArea->textCursor().hasSelection())
        return {};

    // Перебираются фрагменты блока
    for (auto it = currentBlock.begin(); !it.atEnd(); ++it) {
        auto fragment = it.fragment();

        // Если фрагмент содержит изображение
        if (fragment.isValid() && fragment.charFormat().isImageFormat()) {
            int fragmentStart = fragment.position();
            int fragmentEnd = fragmentStart + fragment.length();
            int selectionStart = textArea->textCursor().selectionStart();
            int selectionEnd = textArea->textCursor().selectionEnd();

            // Если начало и конец фрагмента совпадает с координатами выделения
            // Проверяется и случай, когда блок выделен в обратную сторону
            if ((fragmentStart == selectionStart && fragmentEnd == selectionEnd) ||
                (fragmentStart == selectionEnd && fragmentEnd == selectionStart)) {
                return fragment.charFormat().toImageFormat();
            }
        }
    }

    return {};
}

// Где находится картинка относительно курсора (-1 - слева, 0 - курсор не на картинке, 1 - справа)
int EditorCursorPositionDetector::whereImageAtCursor() {
    if (!isCursorOnImage())
        return 0;

    // Qt считает, что курсор находится на картинке, если курсор находится справа от нее: K|
    // Но если картинка стоит первым символом на строке, и курсор тоже в начале строки: |K
    // то charFormat() для такой позиции курсора будет тоже выдаваться как валидный QTextImageFormat
    return textArea->textCursor().atBlockStart() ? 1 : -1;
}

// Проверка, находится ли курсор на позиции, где находится математическое выражение (формула)
bool EditorCursorPositionDetector::isCursorOnMathExpression() {
    // Если курсор не находится возле картинки
    if (!isCursorOnImage())
        return false;

    auto resourceName = textArea->textCursor().charFormat().toImageFormat().name();
    return isMathExpressionSmartDetect(resourceName);
}

// Проверка, выделено ли математическое выражение (формула)
bool EditorCursorPositionDetector::isMathExpressionSelect() {
    // Если картинка не выделена
    if (!isImageSelect())
        return false;

    auto resourceName = getImageSelectFormat().name();
    return isMathExpressionSmartDetect(resourceName);
}

bool EditorCursorPositionDetector::isMathExpressionSmartDetect(QString &resourceName) {
    auto image = textArea->document()->resource(QTextDocument::ImageResource, QUrl(resourceName)).value<QImage>();
    if (image.isNull())
        return false;

    auto text = image.text("Description");
    return text.startsWith(FixedParameters::appTextId + ":" + mathExpDescriptionType + ":");
}

bool EditorCursorPositionDetector::isCursorOnSpaceLine() {
    // Если есть выделение, функция работать не должна
    if (textArea->textCursor().hasSelection())
        return false;

    // Создаётся дополнительный курсор
    auto cursor = textArea->textCursor();

    // Выделяется анализируемая строка
    cursor.select(QTextCursor::LineUnderCursor);

    // Текст анализируемой строки
    auto selectedText = cursor.selectedText();
    return selectedText.simplified().size() <= 0;
}

// Стоит ли курсор на пустой строке (т.е. в строке есть только символ перевода на новую строку)
bool EditorCursorPositionDetector::isCursorOnEmptyLine() {
    if (!textArea->textCursor().atBlockStart())
        return false;

    // Создаётся дополнительный курсор
    auto cursor = textArea->textCursor();

    // Выделяется анализируемая строка
    cursor.select(QTextCursor::LineUnderCursor);

    // Текст анализируемой строки
    auto selectedText = cursor.selectedText();

    return selectedText.size() <= 0;
}

// Выбран ли блок текста (т.е. находятся ли начало и конец выделения точно на границах блока)
bool EditorCursorPositionDetector::isBlockSelect() {
    // Выясняются позиции начала и конца выделения
    int start = textArea->textCursor().selectionStart();
    int stop = textArea->textCursor().selectionEnd();

    // Высталяются флаги, находится ли начало и конец выделения на границах блока.
    // Это определяется с помощью дополнительного курсора
    bool flagStartBlockEdge = false;
    bool flagStopBlockEdge = false;
    auto cursor = textArea->textCursor();
    cursor.setPosition(start);
    if (cursor.atBlockStart() || cursor.atBlockEnd())
        flagStartBlockEdge = true;
    cursor.setPosition(stop);
    if (cursor.atBlockStart() || cursor.atBlockEnd())
        flagStopBlockEdge = true;

    return flagStartBlockEdge && flagStopBlockEdge;
}

// Проверка, находится ли курсор на ссылке
bool EditorCursorPositionDetector::isCursorOnReference() {
    QString href = "";
    return isCursorOnReferenceSmart(href);
}

// Текст ссылки, на которой стоит курсор
QString EditorCursorPositionDetector::referenceHref() {
    QString href = "";
    return isCursorOnReferenceSmart(href) ? href : "";
}

bool EditorCursorPositionDetector::isCursorOnReferenceSmart(QString &resultHref) {
    // Если есть выделение
    if (textArea->textCursor().hasSelection()) {
        // Выясняются позиции начала и конца выделения
        int start = textArea->textCursor().selectionStart() + 1; // +1 так как Qt определяет формат символа стоящего слева от курсора (как минимум для языков с записью слева направо)
        int stop = textArea->textCursor().selectionEnd();

        qDebug() << "Reference detector. Selection start:" << start << "selection stop:" << stop;

        // Создаётся дополнительный курсор
        auto cursor = textArea->textCursor();

        // Курсор перемещается в начало выделения
        cursor.setPosition(start);

        // Текст ссылки в самом начале выделения
        auto href = cursor.charFormat().anchorHref();

        // Если ссылки нет
        if (href.length() == 0)
            return false; // Нечего проверять дальше

        // В пределах выделения должна содержаться только одна ссылка
        for (int i = start + 1; i <= stop; ++i) {
            cursor.setPosition(i);
            if (href != cursor.charFormat().anchorHref())
                return false; // Есть различие, выделена не только ссылка
        }

        resultHref = href;
        return true; // Все проверки пройдены, в выделении есть ссылка и она одна
    } else {
        // Иначе выделения нет

        // Значение ссылки у символа слева от курсора
        auto hrefLeft = textArea->textCursor().charFormat().anchorHref();

        // Создаётся дополнительный курсор
        auto cursor = textArea->textCursor();
        cursor.movePosition(QTextCursor::Right);
        auto hrefRight = cursor.charFormat().anchorHref(); // Значение ссылки у символа справа от основного курсора

        // Если ни слева ни справа нет ссылок
        if (hrefLeft.isEmpty() && hrefRight.isEmpty())
            return false;

        resultHref = hrefLeft.isEmpty() ? hrefRight : hrefLeft; // Если ссылка и слева и справа, берется левая ссылка
        return true;
    }
}
