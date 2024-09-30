#pragma once

#include <QCloseEvent>
#include <QDialog>
#include <QEvent>
#include <QTextEdit>
#include "EditorShowTextContextMenu.h"


class EditorShowText : public QDialog {
    Q_OBJECT
  public:
    explicit EditorShowText(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~EditorShowText();

    void setNoteId(const QString &noteId);
    void setHtml(QString text);
    void setDocument(QSharedPointer<QTextDocument> pDocument);

    bool hasTextSelection();

    int getTextVerticalScroll();
    void setTextVerticalScroll(int vScroll);

    // Действия при закрытии диалога
    void closeEvent(QCloseEvent *event);

  signals:

    void editorShowTextClose(const QString &noteId);

  protected slots:

    void onCustomContextMenuRequested(const QPoint &pos);
    void onGotoNote();

  private:
    QString mNoteId;
    QSharedPointer<QTextEdit> mTextArea;
    QSharedPointer<QTextDocument> mTextDocument;
    QSharedPointer<EditorShowTextContextMenu> mContextMenu;

    int mGeomX = 0;
    int mGeomY = 0;
    int mGeomW = 0;
    int mGeomH = 0;

    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
};
