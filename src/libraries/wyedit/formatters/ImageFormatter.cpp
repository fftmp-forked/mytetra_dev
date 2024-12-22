#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QMessageBox>
#include <QTextBlock>
#include <QTextFragment>

#include "ImageFormatter.h"

#include "../Editor.h"
#include "../EditorCursorPositionDetector.h"
#include "../EditorImageProperties.h"
#include "../EditorTextArea.h"

#include "../../Downloader.h"
#include "../../helpers/UniqueIdHelper.h"


/// @brief Формат картинки, которая выделена (если выделена единственная картинка)
QTextImageFormat ImageFormatter::imageFormatOnSelect() {
    return editor->cursorPositionDetector->getImageSelectFormat();
}

/// @brief Формат картинки на которой находится курсор
QTextImageFormat ImageFormatter::imageFormatOnCursor() {
    // Проверка срабатывает только если нет выделения
    if (!textArea->textCursor().hasSelection()) {
        auto imageFormat = textArea->textCursor().charFormat().toImageFormat();

        if (imageFormat.isValid())
            return imageFormat;
    }

    return QTextImageFormat();
}

void ImageFormatter::editImageProperties() {
    // Для картинки с формулой свойства изображения редактироваться не должны
    if (editor->cursorPositionDetector->isMathExpressionSelect() ||
        editor->cursorPositionDetector->isCursorOnMathExpression()) {
        return;
    }

    // Данные обрабатываемой картинки
    QTextImageFormat imageFormat;

    // Если выбрано изображение
    if (editor->cursorPositionDetector->isImageSelect()) {
        imageFormat = imageFormatOnSelect();
    } else if (editor->cursorPositionDetector->isCursorOnImage()) {
        // Если изображение не выбрано, но курсор находится в позиции изображения
        imageFormat = imageFormatOnCursor();
    }

    auto imageName = imageFormat.name();

    // По имени из ресурсов вытягивается картинка
    QUrl urlName(imageName);
    auto imageData = textArea->document()->resource(QTextDocument::ImageResource, urlName);
    auto image = imageData.value<QImage>();

    // Выясняются реальные размеры картинки
    int realImageWidth = image.width();
    int realImageHeight = image.height();

    qDebug() << "Real image width " << realImageWidth << " height " << realImageHeight;
    qDebug() << "Format image width " << imageFormat.width() << " height " << imageFormat.height();

    // Создается и запускается диалог запроса размеров картинки
    EditorImageProperties dialog;
    dialog.set_info(tr("Real image size ") +
                    QString::number(realImageWidth) +
                    " x " +
                    QString::number(realImageHeight) +
                    tr(" pixels"));
    dialog.set_real_width(realImageWidth);
    dialog.set_real_height(realImageHeight);

    // Если в форматировании картинки не задан размер картинки
    if (imageFormat.width() == 0 && imageFormat.height() == 0) {
        // В окне настройки стартовый размер задается как размер картинки
        dialog.set_width(realImageWidth);
        dialog.set_height(realImageHeight);
    } else {
        // В окне настройки стартовый размер задается согласно форматированию
        dialog.set_width(imageFormat.width());
        dialog.set_height(imageFormat.height());
    }

    dialog.update_percent();

    // Запуск диалога на выполнение
    if (dialog.exec() != QDialog::Accepted)
        return;

    imageFormat.setWidth(dialog.get_width());
    imageFormat.setHeight(dialog.get_height());

    if (!imageFormat.isValid())
        return;

    // Если выбрано изображение
    if (editor->cursorPositionDetector->isImageSelect()) {
        QTextFragment fragment;

        // Блок, в пределах которого находится курсор
        auto currentBlock = textArea->textCursor().block();

        // Перебираются фрагметы блока
        // Так как известно, что картинка выделена, поиск фрагмента будет успешным
        for (auto it = currentBlock.begin(); !it.atEnd(); ++it) {
            fragment = it.fragment();

            // Если фрагмент содержит изображение, для которого редактировались свойства
            if (fragment.isValid() &&
                fragment.charFormat().isImageFormat() &&
                fragment.charFormat().toImageFormat().name() == imageName) {

                int fragmentStart = fragment.position();
                int fragmentEnd = fragmentStart + fragment.length();
                int selectionStart = textArea->textCursor().selectionStart();
                int selectionEnd = textArea->textCursor().selectionEnd();

                // Если начало и конец фрагмента совпадает с координатами выделения
                // Проверяется и случай, когда выделение было в обратную сторону
                if ((fragmentStart == selectionStart && fragmentEnd == selectionEnd) ||
                    (fragmentStart == selectionEnd && fragmentEnd == selectionStart)) {
                    break; // Переменная fragment содержит только картинку, для которой редактируются свойства
                }
            }
        }

        auto helpCursor = textArea->textCursor();

        helpCursor.setPosition(fragment.position());
        helpCursor.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
        helpCursor.setCharFormat(imageFormat);
    }

    // Если изображение не выбрано, но курсор находится в позиции изображения
    if (editor->cursorPositionDetector->isCursorOnImage()) {
        int cursorPosition = textArea->textCursor().position();

        auto helper = textArea->textCursor();

        helper.setPosition(cursorPosition);
        helper.movePosition(textArea->textCursor().atBlockStart() ? QTextCursor::NextCharacter : QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
        helper.setCharFormat(imageFormat);
    }
}

// Обработка клавиши добавления картинки
void ImageFormatter::onInsertImageFromFileClicked() {

    // Если выделена картинка
    if (editor->cursorPositionDetector->isImageSelect() ||
        editor->cursorPositionDetector->isCursorOnImage()) {
        qDebug() << "Image selected";

        editImageProperties();
    } else {
        // картинка не выделена, срабатывает режим добавления картинки из файла

        // Диалог выбора файлов катинок
        QFileDialog imageSelectDialog(editor);
        imageSelectDialog.setFileMode(QFileDialog::ExistingFiles); // QFileDialog::ExistingFile
        imageSelectDialog.setNameFilter("*.png *.jpg *.gif");
        imageSelectDialog.setWindowTitle(tr("Insert image"));
        imageSelectDialog.setDirectory(QDir::homePath());

        // Если не было нажато "Ок"
        if (imageSelectDialog.exec() != QDialog::Accepted)
            return;

        // Выясняется список выбранных файлов
        auto files = imageSelectDialog.selectedFiles();

        // Если ни один файл не выбран
        if (files.empty())
            return;

        // Перебираются файлы выбранных картинок
        for (auto file : files) {
            // Картинка загружается из файла
            auto image = QImageReader(file).read();

            // Внутреннее имя картинки
            auto imageName = getUniqueImageName();

            // Картинка добавляется в хранилище документа
            textArea->document()->addResource(QTextDocument::ImageResource, QUrl(imageName), image);

            // Картинка вставляется в текст
            auto cursor = textArea->textCursor();
            cursor.insertImage(imageName);
        }
    }     // Завершилось условие что картинка не выбрана и нужно добавлять из файла
}

// Вызов окна настройки свойств изображения
void ImageFormatter::onContextMenuEditImageProperties() {
    auto pos = editor->cursorPositionDetector;

    // Для картинки с формулой свойства изображения редактироваться не должны
    if (!pos->isMathExpressionSelect() && !pos->isCursorOnMathExpression() && (pos->isImageSelect() || pos->isCursorOnImage()))
        editImageProperties();
}

void ImageFormatter::onDownloadImages(const QString html) {
    // qDebug() << "HTML for download images: " << html;

    // Создается временный документ на основе HTML (именно документ, так как у QTextDocumentFragment нет методов перебора блоков текста)
    QTextDocument textDocument;
    QTextCursor textCursor(&textDocument);
    textCursor.insertHtml(html);

    QStringList downloadReferences; // Список ссылок на изображения, которые надо загрузить

    QMap<QString, QString> referencesAndInternalNames; // Соответствие ссылок на изображения и внутренних имен изображений

    QMessageBox msgBox;

    QTextBlock textBlock = textDocument.begin();
    while (textBlock.isValid()) {
        bool resetBlock = false;

        for (auto it = textBlock.begin(); !it.atEnd(); ++it) {
            auto currentFragment = it.fragment();
            if (!currentFragment.isValid() || !currentFragment.charFormat().isImageFormat())
                continue;

            // Выясняется формат картинки
            auto imgFmt = currentFragment.charFormat().toImageFormat();

            // Из формата выясняется имя картинки
            auto imageName = imgFmt.name();
            qDebug() << "Find " << imageName << "\n"; // имя файла

            // Если имя файла не является "внутренним", значит картинка еще не добавлена
            static const QRegularExpression re("^image\\d{10}[a-z0-9]+.png$");
            if (!imageName.contains(re)) {
                if (msgBox.text().length() == 0) {
                    msgBox.setText(tr("Images download initiating..."));
                    msgBox.setStandardButtons(QMessageBox::NoButton);
                    msgBox.setWindowModality(Qt::WindowModal);
                    msgBox.show();
                    qApp->processEvents();
                }

                qDebug() << "Set file for download" << imageName;

                QString internalImageName;

                // Если ссылки на картинку еще нет в списке загружаемых
                if (!downloadReferences.contains(imageName)) {
                    // Ссылка на картинку добавляется в массив скачиваемых файлов
                    downloadReferences << imageName;

                    // Ссылке ставится в соответствие уникальное внутреннее имя картинки
                    internalImageName = getUniqueImageName();
                    referencesAndInternalNames[imageName] = internalImageName;
                } else {
                    // Иначе картинка повторяется

                    // Внутреннее имя картинки берется из уже существующих
                    internalImageName = referencesAndInternalNames[imageName];
                }

                // Символ внешней картинки заменяется на символ внутренней
                textCursor.setPosition(currentFragment.position());
                textCursor.deleteChar();                   // Удаляется символ - внешняя картинка (попробовать deletePreviousChar)
                textCursor.insertImage(internalImageName); // Вставляется символ - внутренняя картинка

                // Выход из цикла перебора фрагмента, т. к. документ изменился
                resetBlock = true;
                break;
            }
        }

        if (resetBlock)
            textBlock = textDocument.begin(); // Документ изменился, его надо обрабатывать с самого начала
        else
            textBlock = textBlock.next();
    }

    // Если есть изображения, которые надо скачать
    if (downloadReferences.count() > 0) {
        msgBox.hide();

        // Виджет скачивания файлов
        Downloader downloader;
        downloader.setAboutText(tr("Download images"));
        downloader.setDownloadMode(Downloader::memory);
        downloader.setReferencesList(downloadReferences);

        // Установка ширины для виджета скачивания файлов
        int dialogWidth = 0.8 * textArea->width();
        downloader.setMinimumWidth(dialogWidth);
        downloader.resize(downloader.size());

        // На передний план
        downloader.raise();

        // Запуск виджета скачивания файлов
        downloader.run();

        // Если при скачивании изображений появились какие-то ошибки
        if (!downloader.isSuccess()) {
            QMessageBox msgBox;
            msgBox.setText(tr("Probably error in a process of images download."));

            if (downloader.getErrorLog().length() > 0)
                msgBox.setDetailedText(downloader.getErrorLog());

            msgBox.exec();
        }

        emit downloadImagesSuccessfull(textDocument.toHtml(), downloader.getReferencesAndMemoryFiles(), referencesAndInternalNames);
        return;
    }

    emit downloadImagesSuccessfull(html, QMap<QString, QByteArray>(), QMap<QString, QString>());
}
