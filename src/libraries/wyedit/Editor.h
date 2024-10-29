#pragma once

#include <QBoxLayout>
#include <QComboBox>
#include <QFontComboBox>
#include <QLabel>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>

#include "EditorConfig.h"
#include "EditorFindDialog.h"
#include "EditorTextArea.h"
#include "EditorToolBarAssistant.h"
#include "formatters/Formatter.h"
#include "formatters/ImageFormatter.h"
#include "formatters/ListFormatter.h"
#include "formatters/MathExpressionFormatter.h"
#include "formatters/PlacementFormatter.h"
#include "formatters/ReferenceFormatter.h"
#include "formatters/TableFormatter.h"
#include "formatters/TypefaceFormatter.h"

// ----------------------------------------------------------
// WyEdit - визуальный редактор для MyTetra
// Волгодонск, 2010 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензией GPL v.3
// © Степанов С. М. 2010
// ----------------------------------------------------------


// Описание заголовка математического выражения в поле Description PNG-файла
// Математическое выражение состоит из текстового идентификатора приложения, типа хранимого значения
// и номера версии формата хранимого значения, выглядит следующим образом:
// "mytetra:mathExpression:v0001:математическое выражение в формате TeX"
const QLatin1String mathExpDescriptionType("mathExpression");
const int mathExpVersion = 1;          // Текущая максимальная версия формата хранения формулы
const int mathExpVersionNumberLen = 4; // Сколько символов занимает номер версии (только цифры)
const int mathExpHeaderLen = 29;       // Сколько символов занимает весь заголовок (префикс, номер версии, три двоеточия)

class EditorTextEdit;
class EditorContextMenu;
class EditorIndentSliderAssistant;
class Formatter;
class MetaEditor;
class EditorCursorPositionDetector;

class Editor : public QWidget {
    Q_OBJECT

    friend class MetaEditor;

    friend class Formatter;
    friend class PlacementFormatter;
    friend class TypefaceFormatter;
    friend class ListFormatter;
    friend class TableFormatter;
    friend class ImageFormatter;
    friend class MathExpressionFormatter;
    friend class ReferenceFormatter;

    friend class EditorContextMenu;

  public:
    Editor(QWidget *parent = nullptr);
    ~Editor();

    // Объект, хранящий настройки редактора
    EditorConfig *editorConfig = nullptr;

    // Ассистент панели кнопок
    EditorToolBarAssistant *editorToolBarAssistant = nullptr; // todo: Переименовать в toolBarAssistant?

    // Ассистент виджета горизонтальной линейки отступов
    EditorIndentSliderAssistant *indentSliderAssistant = nullptr;

    // Контекстное меню редактора
    EditorContextMenu *editorContextMenu = nullptr;

    EditorCursorPositionDetector *cursorPositionDetector = nullptr;

    void initEnableAssembly(bool flag);
    void initConfigFileName(QString name);
    void initDisableToolList(QStringList toolNames);
    void init();

    // Методы работы с textarea
    void setTextarea(QString text);
    void setTextareaEditable(bool editable) { textArea->setReadOnly(!editable); };
    QString getTextarea() const { return textArea->document()->toHtml(); }; // Получение текста области редактирования в формате HTML
    QTextDocument *getTextareaDocument() const { return textArea->document(); };
    void setTextareaModified(bool modify) { textArea->document()->setModified(modify); };
    bool getTextareaModified() const { return textArea->document()->isModified(); };

    // Абсолютный или относительный путь (т.е. директория),
    // куда будет сохраняться текст. Без завершающего слеша
    bool setWorkDirectory(QString dirName);
    QString getWorkDirectory(void) const { return workDirectory; }

    // Имя файла, куда должен сохраняться текст
    // Без пути, только имя
    void setFileName(QString fileName) { workFileName = fileName; };
    QString getFileName() const { return workFileName; };

    void saveTextarea();
    bool saveTextareaText();
    bool saveTextareaImages(int mode);
    bool loadTextarea();

    // Методы установки нестандартных процедур чтения и сохранения текста
    void setSaveCallback(void (*func)(QObject *editor, QString saveString)) { saveCallbackFunc = func; };
    void setLoadCallback(void (*func)(QObject *editor, QString &loadString)) { loadCallbackFunc = func; };

    // Метод установки функции нажатия на кнопку Attach
    void setAttachCallback(void (*func)(void)) { attachCallbackFunc = func; };

    // Методы установки и чтения произвольных нестандартных данных
    // которые может хранить объект редактора
    void setMiscField(QString name, QString value);
    QString getMiscField(QString name);
    void clearAllMiscField();

    void setDirFileEmptyReaction(int mode);
    int getDirFileEmptyReaction() const { return dirFileEmptyReaction; };

    int getCursorPosition() const { return textArea->textCursor().position(); };
    void setCursorPosition(int n);

    int getScrollBarPosition() const { return textArea->verticalScrollBar()->value(); };
    void setScrollBarPosition(int n) { textArea->verticalScrollBar()->setValue(n); };

    QString smartFontFamily(QString fontName); // Умное преобразование имени шрифта
    int smartFontSize(int fontSize) { return fontSize ? editorConfig->get_default_font_size() : fontSize; };

    void switchAttachIconExists(bool isExists) { editorToolBarAssistant->switchAttachIconExists(isExists); };

    enum {
        SAVE_IMAGES_SIMPLE = 0,       // Простое сохранение картинок, встречающихся в тексте
        SAVE_IMAGES_REMOVE_UNUSED = 1 // Сохранение картинок, встречающихся в тексте, с удалением из каталога записи тех, которых в тексте нет
    };

    enum {
        DIRFILEEMPTY_REACTION_SHOW_ERROR,
        DIRFILEEMPTY_REACTION_SUPPRESS_ERROR
    };

  signals:

    void send_expand_edit_area(bool flag);

    void updateIndentsliderToActualFormat();
    void updateIndentSliderGeometry();

    void updateAlignButtonHiglight(bool);

    void changeFontselectOnDisplay(QString fontName);
    void changeFontsizeOnDisplay(int n);

  private slots:

    void onShowhtmlClicked();
    void onFindtextClicked();
    void onSettingsClicked();
    void onShowformattingClicked(bool);

    void onExpandEditAreaClicked();
    void onSaveClicked();
    void onShowTextClicked();
    void onToAttachClicked();

    void onCursorPositionChanged(); // Слот, контролирущий перемещение курсора
    void onSelectionChanged();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onPasteAsPlainText();
    void onSelectAll();

    void onFindtextSignalDetect(const QString &text, QTextDocument::FindFlags flags);

    // Открытие контекстного меню
    void onCustomContextMenuRequested(const QPoint &pos);

  private:
    void setupSignals();
    void setupToolsSignals();
    void setupEditorToolBarAssistant(EditorTextArea *textArea, QStringList disableToolList);
    void setupIndentSliderAssistant();
    void setupEditorTextArea();
    void setupCursorPositionDetector();
    void setupFormatters();
    void assembly();

    // Устанавка размера табуляции для клавиши Tab
    void setTabSize();

    // Переопределение событий обработки клавиш
    // нужны для определения момента undo/redo
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    // Область редактирования текста
    EditorTextArea *textArea = nullptr;

    // Форматировщики текста
    TypefaceFormatter *typefaceFormatter = nullptr;
    PlacementFormatter *placementFormatter = nullptr;
    ListFormatter *listFormatter = nullptr;
    TableFormatter *tableFormatter = nullptr;
    ImageFormatter *imageFormatter = nullptr;
    MathExpressionFormatter *mathExpressionFormatter = nullptr;
    ReferenceFormatter *referenceFormatter = nullptr;

    bool isInit;

    bool initDataEnableAssembly;
    QString initDataConfigFileName;
    QStringList initDataDisableToolList;

    // Рабочая директория редактора и файл текста
    // Используется при сохранении текста на диск
    QString workDirectory;
    QString workFileName;

    EditorFindDialog *findDialog; // Виджет поиска

    bool expandEditAreaFlag; // Распахнуто ли на максимум окно редактора

    // Указатели на переопределенные функции записи и чтения редактируемого текста
    void (*saveCallbackFunc)(QObject *editor, QString saveString) = nullptr;
    void (*loadCallbackFunc)(QObject *editor, QString &loadString) = nullptr;

    // Указатель на функцию открытия присоединенных файлов
    void (*attachCallbackFunc)(void) = nullptr;

    // Поля для хранения произвольных данных
    // Обычно используются для запоминания нестандартного набора данных
    // в объекте редактора, и считываются из функции обратного вызова
    QMap<QString, QString> miscFields;

    int dirFileEmptyReaction;
};
