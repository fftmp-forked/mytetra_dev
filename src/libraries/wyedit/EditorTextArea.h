#pragma once

#include <QEvent>
#include <QMimeData>
#include <QPaintEvent>
#include <QTextEdit>

class EditorTextArea : public QTextEdit {
    Q_OBJECT

  public:
    EditorTextArea(QWidget *parent = nullptr);
    ~EditorTextArea(){};

    // Константы, используемые для определения формата данных в буфере обмена
    enum MimeDataFormat { MimeDataText = 1,
                          MimeDataHtml = 2,
                          MimeDataImage = 3 };

    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

    virtual bool canInsertFromMimeData(const QMimeData *source) const;
    virtual void insertFromMimeData(const QMimeData *source);
    int detectMimeDataFormat(const QMimeData *source);

    bool getShowFormatting() const { return flagShowFormatting; }
    void setShowFormatting(bool i);

    int getIndentStartedLeft(void);
    int getIndentStartedRight(void);

  signals:
    void updateIndentlineGeometry(void);
    void clickedOnReference(QString href);

    void downloadImages(const QString href);

    void doubleClickOnImage();

  public slots:
    void showIndentEdge(bool i);
    void setIndentEdgePos(int i);
    void onChangeFontcolor(const QColor &selectedColor);
    void onChangeBackgroundColor(const QColor &selectedColor);
    void onChangeFontFamily(QString fontFamily);
    void onChangeFontPointSize(int n);

    void onGlobalPressKey(int key);
    void onGlobalReleaseKey(int key);

    void onDownloadImagesSuccessfull(const QString html,
                                     const QMap<QString, QByteArray> referencesAndMemoryFiles,
                                     const QMap<QString, QString> referencesAndInternalNames);

  private:
    bool flagShowFormatting; // Рисовать ли символы форматирования

    bool flagShowIndentEdge; // Рисовать ли линию отступа
    int posIndentEdge;       // По какой координате рисовать линию отступа

    bool mouseCursorOverriden;

    QPoint currentMousePosition;

    virtual bool eventFilter(QObject *o, QEvent *e);

    // Переопределение событий обработки клавиш
    // Нужны для отслеживания нажатия клавиши Ctrl чтобы при ней запускался переход по ссылке при клике на ссылку
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    // Переопределение событий обработки мышки
    // Нужны для обработки кликов по ссылкам в тексте записи
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void switchReferenceClickMode(bool flag);
};
