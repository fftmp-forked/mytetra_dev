#pragma once

#include <QTextDocumentFragment>
#include <QTextImageFormat>

#include "Formatter.h"

/// @brief Класс для работы с картинками в тексте
class ImageFormatter : public Formatter {
    Q_OBJECT

  public:
    ImageFormatter() {};

    QTextImageFormat imageFormatOnSelect();
    QTextImageFormat imageFormatOnCursor();

    void editImageProperties();

  signals:

    void downloadImagesSuccessfull(const QString html,
                                   const QMap<QString, QByteArray> referencesAndMemoryFiles,
                                   const QMap<QString, QString> referencesAndInternalNames);

  public slots:

    void onInsertImageFromFileClicked();
    void onContextMenuEditImageProperties();

    void onDownloadImages(const QString html);

    void onDoubleClickOnImage() { onContextMenuEditImageProperties(); };
};
