#include <QTextBlock>
#include <QTextFragment>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QTextDocumentFragment>
#include <QMessageBox>
#include <QImage>
#include <QTemporaryFile>

#include "MathExpressionFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorCursorPositionDetector.h"
#include "../EditorMathExpressionDialog.h"

#include "main.h"
#include "libraries/helpers/DiskHelper.h"
#include "libraries/FixedParameters.h"
#include "views/consoleEmulator/CommandRun.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/UniqueIdHelper.h"


MathExpressionFormatter::MathExpressionFormatter()
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN32) || defined(Q_OS_WINCE) || defined(Q_OS_MSDOS) || defined(Q_OS_CYGWIN)
    // mimetex должен лежать там же где и mytetra
    m_mimetex_bin = QCoreApplication::applicationDirPath() + "/mimetex.exe";
#else
    // try to find mimetex executable in PATH
    // deb-based distros have mimetex.
    // rpm-based distros have mimetex, but not in PATH and somewhy mimetex package depend on HTTP server 'caddy'.
    // ArchLinux have mimetex.cgi binary in PATH

    for (const auto & d : QProcessEnvironment::systemEnvironment().value("PATH").split(':')) {
        if (QFileInfo::exists(d + "/mimetex")) {
            m_mimetex_bin = d + "/mimetex";
            return;
        } else if (QFileInfo::exists(d + "/mimetex.cgi")) {
            m_mimetex_bin = d + "/mimetex.cgi";
            return;
        }
    }
    m_mimetex_bin = QCoreApplication::applicationDirPath() + "/mimetex";
#endif
}


// Исходный код формулы, которая выделена (если выделена единственная картинка формулы)
QString MathExpressionFormatter::mathExpressionOnSelect(void)
{
    // Если выбрано математическое выражение
    if(editor->cursorPositionDetector->isMathExpressionSelect()) {
        QTextImageFormat imageFormat=editor->cursorPositionDetector->getImageSelectFormat();
        if(imageFormat.name().size()>0) {
            return getMathExpressionByImageName(imageFormat.name());
        }
    }

    return QString();
}


// Сождержимое формулы (картинки формулы) на которой находится курсор
QString MathExpressionFormatter::mathExpressionOnCursor(void)
{
    if(editor->cursorPositionDetector->isCursorOnMathExpression()) {
        QTextImageFormat imageFormat = textArea->textCursor().charFormat().toImageFormat();

        return getMathExpressionByImageName(imageFormat.name());
    }

    return QString();
}


// Получение исходного кода математического выражения по имени картинки в ресурсах документа
QString MathExpressionFormatter::getMathExpressionByImageName(QString resourceImageName)
{
    QImage image=textArea->document()->resource(QTextDocument::ImageResource, QUrl(resourceImageName)).value<QImage>();

    if( !image.isNull() ) {
        QString text=image.text("Description");

        if(text.startsWith( FixedParameters::appTextId+":"+FixedParameters::mathExpDescriptionType+":" )) {
            // Учитывается длина префикса, длина номера версии, два двоеточия и символ "v"
            return text.right( text.size() - FixedParameters::mathExpHeaderLen);
        }
    }

    return QString();
}


// Обработка клавиши добавления/редактирования математического выражения
void MathExpressionFormatter::onMathExpressionClicked(void)
{
    // Если выделена картинка математического выражения
    if(editor->cursorPositionDetector->isMathExpressionSelect()) {
        qDebug() << "Math expression on select: " << mathExpressionOnSelect();
        editMathExpression( mathExpressionOnSelect() );
        return;
    }

    // Если курсор стоит на картинке с математическим выражением
    if(editor->cursorPositionDetector->isCursorOnMathExpression()) {
        qDebug() << "Math expression on cursor: " << mathExpressionOnCursor();
        editMathExpression( mathExpressionOnCursor() );
        return;
    }

    // Код выше картинку-формулу не обнаружил
    // Если идет работа с обычной картинкой
    if( editor->cursorPositionDetector->isImageSelect() ||
        editor->cursorPositionDetector->isCursorOnImage() ) {
        return; // При обычной картинке кнопка редактирования формулы срабатывать не должна
    }

    // Математическое выражение не выделено, и срабатывает режим добавления
    addMathExpression();
}


// Вызов окна редактирования формулы
void MathExpressionFormatter::onContextMenuEditMathExpression()
{
    // Формула меняется если формула выделена
    if(editor->cursorPositionDetector->isMathExpressionSelect()) {
        qDebug() << "Math expression on select: " << this->mathExpressionOnSelect();
        editMathExpression( this->mathExpressionOnSelect() );
        return;
    }

    // А так же формула меняется если курсор находится рядом с формулой
    if(editor->cursorPositionDetector->isCursorOnMathExpression()) {
        qDebug() << "Math expression on cursor: " << this->mathExpressionOnCursor();
        editMathExpression( this->mathExpressionOnCursor() );
        return;
    }
}


// Двойной клик по картинке с формулой
void MathExpressionFormatter::onDoubleClickOnImage(void)
{
    if(editor->cursorPositionDetector->isCursorOnMathExpression()) {
        qDebug() << "Math expression on double click: " << this->mathExpressionOnCursor();
        editMathExpression( this->mathExpressionOnCursor() );
    }
}


// Добавление новой формулы
void MathExpressionFormatter::addMathExpression(void)
{
    // Открывается окно запроса математического выражения
    QString mathExpressionText=getMathExpressionFromUser();

    if(mathExpressionText.size()>0) {
        insertMathExpressionToTextArea(mathExpressionText);
    }
}


// Редактирование существующей формулы
void MathExpressionFormatter::editMathExpression(QString iMathExpressionText)
{
    qDebug() << "Edit math expression: " << mathExpressionOnSelect();

    // Открывается окно запроса математического выражения
    QString mathExpressionText=getMathExpressionFromUser( iMathExpressionText );

    if(mathExpressionText.size()>0) {
        // Удаляется предыдущая картинка формулы
        // Если было выделение картинки - удалится выделение, иначе удалится картинка как символ (так работает deleteChar)
        int whereImageAtCursor=editor->cursorPositionDetector->whereImageAtCursor();
        if(whereImageAtCursor==-1) {
            textArea->textCursor().deletePreviousChar();
        }
        if(whereImageAtCursor==1) {
            textArea->textCursor().deleteChar();
        }

        insertMathExpressionToTextArea(mathExpressionText);
    }
}


// Запрос математического выражения от пользователя
QString MathExpressionFormatter::getMathExpressionFromUser(QString iMathExpressionText)
{
    EditorMathExpressionDialog dialog(this, textArea); // Диалог написания Tex формулы

    dialog.setMathExpressionText(iMathExpressionText);
    dialog.setWindowTitle(tr("Edit TeX math expression"));
    int result=dialog.exec();

    // Если не нажато OK
    if(result!=QDialog::Accepted)
        return QString();

    // Если нажато OK, исходное выражение было непустым, и пользователь обнулил выражение
    if(result==QDialog::Accepted &&
       iMathExpressionText.size()>0 &&
       dialog.getMathExpressionText().trimmed().size()==0) {
        // Нельзя превращать картинку в картинку с пустой формулой
        QMessageBox msgBox;
        msgBox.setText(tr("Error while input TeX source"));
        msgBox.setInformativeText("You can not replace a mathematical expression with an empty mathematical expression.\nMaybe you just need to delete this mathematical expression as a picture?");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.exec();

        return iMathExpressionText;
    }

    // Если ничего небыло изменено в TeX коде
    if(!dialog.isModified())
        return QString();

    return dialog.getMathExpressionText().trimmed();
}

void MathExpressionFormatter::createGifFromMathExpression(QString iMathExpression, QString iFileName)
{
    CommandRun exCommand;
    exCommand.setCommand(m_mimetex_bin);
    exCommand.setArgs({iMathExpression, "-e", iFileName});
    exCommand.runSimple();
}


void MathExpressionFormatter::insertMathExpressionToTextArea(QString iMathExpressionText)
{
    QString tempFileName=QDir::tempPath()+"/"+getUniqueId()+".gif";

    qDebug() << "Formula code: " << iMathExpressionText;
    qDebug() << "Formula temporary file name: " << tempFileName;

    createGifFromMathExpression( iMathExpressionText, tempFileName );

    bool isSuccess=false;

    // Если картинка сформировалась (подумать, возможно надо вставлсять картинку в текст через форматтер ImageFormatter)
    if ( QFile::exists(tempFileName) ) {
        // Временная картинка загружается из файла в память
        QImage image = QImageReader(tempFileName).read();

        // И сразу удаляется в корзину
        DiskHelper::removeFileToTrash( tempFileName );

        // Если картинка была нормально загружена из файла
        if( !image.isNull() ) {
            // Картинка в памяти запоминает исходный код формулы
            image.setText("Description", FixedParameters::appTextId + ":" +
                                         FixedParameters::mathExpDescriptionType + ":" +
                                         "v" + QString::number(FixedParameters::mathExpVersion).rightJustified(FixedParameters::mathExpVersionNumberLen, '0') + ":" +
                                         iMathExpressionText);

            // Внутреннее имя картинки
            QString imageName=getUniqueImageName();

            // Картинка добавляется в хранилище документа под своим внутренним именем
            textArea->document()->addResource(QTextDocument::ImageResource, QUrl(imageName), image );

            // Создается описание форматированной картинки
            // QTextImageFormat imageFormat;
            // imageFormat.setName(link.toString());

            // Картинка вставляется в текст
            QTextCursor cursor=textArea->textCursor();
            cursor.insertImage(imageName);

            // Ставится пометка что текст записи был изменен
            textArea->document()->setModified(true);

            isSuccess=true;
        }
    }

    if(!isSuccess) {
        QMessageBox msgBox;
        msgBox.setText(tr("Error while parse TeX syntax"));
        msgBox.setInformativeText("Can't generate math expression picture");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.exec();
    }
}

