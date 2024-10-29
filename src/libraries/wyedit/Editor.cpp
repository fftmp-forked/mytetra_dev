#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDir>
#include <QLayout>
#include <QMessageBox>
#include <QStyle>
#include <QTextCursor>

#include "Editor.h"
#include "EditorConfigDialog.h"
#include "EditorContextMenu.h"
#include "EditorCursorPositionDetector.h"
#include "EditorIndentSliderAssistant.h"
#include "EditorShowTextDispatcher.h"
#include "formatters/Formatter.h"

#include "../../views/mainWindow/MainWindow.h"
#include "../helpers/DebugHelper.h"
#include "../helpers/MultiLineInputDialog.h"

// Максимально возможная длина выделения текста (в символах) при которой
// происходит проверка, есть ли в выделенном тексте различные шрифты, размеры текста, BUI форматирование
// с соответствующем изменением панели кнопок и других элементов форматирования текста
// Если выделено текста больше, такая проверка не происходит
#define WYEDIT_MAX_SELECTION_SIZE_WHILE_CHECK_TOOLBAR_ITEM 4096

Editor::Editor(QWidget *parent) : QWidget(parent) {
    isInit = false;

    initDataEnableAssembly = true;
    initDataConfigFileName = "";
    initDataDisableToolList.clear();

    dirFileEmptyReaction = DIRFILEEMPTY_REACTION_SHOW_ERROR;
}

Editor::~Editor() {
    delete cursorPositionDetector;

    delete typefaceFormatter;
    delete placementFormatter;
    delete listFormatter;
    delete tableFormatter;
    delete imageFormatter;
    delete mathExpressionFormatter;
    delete referenceFormatter;
}

// Настроечные методы, вызываемые перед init()

void Editor::initEnableAssembly(bool flag) {
    if (isInit)
        criticalError("Method " + QString(__FUNCTION__) + " running before init() only.");

    initDataEnableAssembly = flag;
}

void Editor::initConfigFileName(QString name) {
    if (isInit)
        criticalError("Method " + QString(__FUNCTION__) + " running before init() only.");

    initDataConfigFileName = name;
}

void Editor::initDisableToolList(QStringList toolNames) {
    if (isInit)
        criticalError("Method " + QString(__FUNCTION__) + " running before init() only.");

    initDataDisableToolList = toolNames;
}


void Editor::init() {
    // Информационный флаг, что был запущен метод init()
    isInit = true;

    // Создается объект поддержки конфигурирования редактора
    editorConfig = new EditorConfig(initDataConfigFileName, this);
    editorConfig->setObjectName("editorconfig");

    // Создается виджет поиска; обязательно нужно указать parent, чтобы могли применяться флаги окна.
    // Виджет будет постоянно включен параллельно с работой редактора.
    // Только будет либо виден, либо не виден.
    findDialog = new EditorFindDialog(this);
    findDialog->hide();

    // Создаётся контекстное меню
    editorContextMenu = new EditorContextMenu(this);
    this->addActions(editorContextMenu->getActionsList());

    setupEditorTextArea();
    setupCursorPositionDetector();
    setupIndentSliderAssistant(); // Инициализируется обязательно только после TextArea
    setupEditorToolBarAssistant(textArea, initDataDisableToolList);
    setupFormatters();

    setupSignals();

    if (initDataEnableAssembly)
        assembly();

    // Вначале редактор находится в обычном нераспахнутом состоянии
    expandEditAreaFlag = false;

    emit updateIndentSliderGeometry();
}

// Создание и настройка панели инструментов редактора
void Editor::setupEditorToolBarAssistant(EditorTextArea *textArea, QStringList disableToolList) {
    // Установлен this в качестве родителя, поэтому вручную удалять объект в деструкторе не нужно
    editorToolBarAssistant = new EditorToolBarAssistant(this, textArea, disableToolList);
    editorToolBarAssistant->setObjectName("toolBarAssistant");
}

// Создание и настройка ассистента линейки отступов
void Editor::setupIndentSliderAssistant() {
    indentSliderAssistant = new EditorIndentSliderAssistant(this, textArea);
    indentSliderAssistant->setObjectName("indentSliderAssistant");
}

// Создание и настройка области редактирования
void Editor::setupEditorTextArea() {
    // Создается область редактирования
    textArea = new EditorTextArea(this);
    textArea->setObjectName("textArea");
    textArea->setContextMenuPolicy(Qt::CustomContextMenu); // Меню определяется в программе

    QFont font;
    font.fromString(editorConfig->get_default_font());
    font.setPointSize(editorConfig->get_default_font_size());
    textArea->selectAll();
    textArea->setCurrentFont(font);
    textArea->setFont(font);

    // Устанавка размера табуляции для клавиши Tab
    setTabSize();
}

void Editor::setupCursorPositionDetector() {
    cursorPositionDetector = new EditorCursorPositionDetector();
    cursorPositionDetector->setTextArea(textArea);
}

void Editor::setupFormatters() {
    // Форматирование начертания текста
    typefaceFormatter = new TypefaceFormatter();
    typefaceFormatter->setEditor(this);
    typefaceFormatter->setTextArea(textArea);

    // Форматирование размещения текста
    placementFormatter = new PlacementFormatter();
    placementFormatter->setEditor(this);
    placementFormatter->setTextArea(textArea);

    // Форматирование текста как списка
    listFormatter = new ListFormatter();
    listFormatter->setEditor(this);
    listFormatter->setTextArea(textArea);

    // Форматирование таблиц в тексте
    tableFormatter = new TableFormatter();
    tableFormatter->setEditor(this);
    tableFormatter->setTextArea(textArea);

    // Форматирование картинок в тексте
    imageFormatter = new ImageFormatter();
    imageFormatter->setEditor(this);
    imageFormatter->setTextArea(textArea);

    // Редактирование формул в тексте
    mathExpressionFormatter = new MathExpressionFormatter();
    mathExpressionFormatter->setEditor(this);
    mathExpressionFormatter->setTextArea(textArea);

    // Форматирование ссылки
    referenceFormatter = new ReferenceFormatter();
    referenceFormatter->setEditor(this);
    referenceFormatter->setTextArea(textArea);
}

void Editor::setupSignals() {
    setupToolsSignals();

    // Обратка для typefaceFormatter
    /// @todo: подумать, а надо ли
    connect(typefaceFormatter, &TypefaceFormatter::updateOutlineButtonHiglight,
            editorToolBarAssistant, &EditorToolBarAssistant::onUpdateOutlineButtonHiglight,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeFontselectOnDisplay,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeFontselectOnDisplay,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeFontsizeOnDisplay,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeFontsizeOnDisplay,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeFontFamily,
            textArea, &EditorTextArea::onChangeFontFamily,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeFontFamily,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeFontFamily,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeFontPointSize,
            textArea, &EditorTextArea::onChangeFontPointSize,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeFontPointSize,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeFontPointSize,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeFontcolor,
            textArea, &EditorTextArea::onChangeFontcolor,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeFontcolor,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeFontcolor,
            Qt::DirectConnection);
    connect(textArea, &EditorTextArea::currentCharFormatChanged,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeIconFontColor,
            Qt::DirectConnection);

    // Соединение сигналов и слотов обрабортки для цвета выделения текста
    connect(typefaceFormatter, &TypefaceFormatter::changeBackgroundcolor,
            textArea, &EditorTextArea::onChangeBackgroundColor,
            Qt::DirectConnection);
    connect(typefaceFormatter, &TypefaceFormatter::changeBackgroundcolor,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeBackgroundColor,
            Qt::DirectConnection);
    connect(textArea, &EditorTextArea::currentCharFormatChanged,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeIconBackgroundColor,
            Qt::DirectConnection);
    connect(textArea, &EditorTextArea::cursorPositionChanged,
            editorToolBarAssistant, &EditorToolBarAssistant::onCursorPositionChanged,
            Qt::DirectConnection);

    connect(this, &Editor::changeFontselectOnDisplay,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeFontselectOnDisplay,
            Qt::DirectConnection);
    connect(this, &Editor::changeFontsizeOnDisplay,
            editorToolBarAssistant, &EditorToolBarAssistant::onChangeFontsizeOnDisplay,
            Qt::DirectConnection);

    connect(placementFormatter, &PlacementFormatter::updateIndentsliderToActualFormat,
            indentSliderAssistant, &EditorIndentSliderAssistant::updateToActualFormat,
            Qt::DirectConnection);
    connect(this, &Editor::updateIndentsliderToActualFormat,
            indentSliderAssistant, &EditorIndentSliderAssistant::updateToActualFormat,
            Qt::DirectConnection);

    connect(this, &Editor::updateAlignButtonHiglight,
            editorToolBarAssistant, &EditorToolBarAssistant::onUpdateAlignButtonHiglight,
            Qt::DirectConnection);
    connect(placementFormatter, &PlacementFormatter::updateAlignButtonHiglight,
            editorToolBarAssistant, &EditorToolBarAssistant::onUpdateAlignButtonHiglight,
            Qt::DirectConnection);

    // Область редактирования текста
    connect(textArea, &EditorTextArea::cursorPositionChanged,
            this, &Editor::onCursorPositionChanged,
            Qt::DirectConnection);
    connect(textArea, &EditorTextArea::selectionChanged,
            this, &Editor::onSelectionChanged,
            Qt::DirectConnection);

    // Соединение сигнал-слот чтобы показать контекстное меню по правому клику в редакторе
    connect(textArea, &EditorTextArea::customContextMenuRequested,
            this, &Editor::onCustomContextMenuRequested,
            Qt::DirectConnection);

    connect(textArea, &EditorTextArea::textChanged, referenceFormatter, &ReferenceFormatter::onTextChanged,
            Qt::DirectConnection);

    // Вызов загрузки картинок
    connect(textArea, &EditorTextArea::downloadImages,
            imageFormatter, &ImageFormatter::onDownloadImages,
            Qt::DirectConnection);

    // Вставка загруженных картинок в текст
    connect(imageFormatter, &ImageFormatter::downloadImagesSuccessfull,
            textArea, &EditorTextArea::onDownloadImagesSuccessfull,
            Qt::DirectConnection);

    // Клик по ссылке при зажатой клавише Ctrl
    connect(textArea, &EditorTextArea::clickedOnReference,
            referenceFormatter, &ReferenceFormatter::onClickedGotoReference,
            Qt::DirectConnection);

    // Двойной клик по картинке, для ImageFormatter
    connect(textArea, &EditorTextArea::doubleClickOnImage,
            imageFormatter, &ImageFormatter::onDoubleClickOnImage,
            Qt::DirectConnection);

    // Двойной клик по картинке, для MathExpressionFormatter
    connect(textArea, &EditorTextArea::doubleClickOnImage,
            mathExpressionFormatter, &MathExpressionFormatter::onDoubleClickOnImage,
            Qt::DirectConnection);

    // Глобальное нажатие клавиш
    connect(&MainWindow::get(), &MainWindow::globalPressKey,
            textArea, &EditorTextArea::onGlobalPressKey,
            Qt::DirectConnection);

    // Глобальное отжатие клавиш
    connect(&MainWindow::get(), &MainWindow::globalReleaseKey,
            textArea, &EditorTextArea::onGlobalReleaseKey,
            Qt::DirectConnection);

    // Сигналы контекстного меню
    connect(textArea->document(), &QTextDocument::undoAvailable,
            editorContextMenu->getActionUndo(), &QAction::setEnabled);
    connect(textArea->document(), &QTextDocument::redoAvailable,
            editorContextMenu->getActionRedo(), &QAction::setEnabled);
    connect(editorContextMenu, &EditorContextMenu::undo,
            this, &Editor::onUndo,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::redo,
            this, &Editor::onRedo,
            Qt::DirectConnection);

    connect(editorContextMenu, &EditorContextMenu::cut,
            this, &Editor::onCut,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::copy,
            this, &Editor::onCopy,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::paste,
            this, &Editor::onPaste,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::pasteAsPlainText,
            this, &Editor::onPasteAsPlainText,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::selectAll,
            this, &Editor::onSelectAll,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::contextMenuEditImageProperties,
            imageFormatter, &ImageFormatter::onContextMenuEditImageProperties,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::contextMenuEditMathExpression,
            mathExpressionFormatter, &MathExpressionFormatter::onContextMenuEditMathExpression,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::contextMenuGotoReference,
            referenceFormatter, &ReferenceFormatter::onContextMenuGotoReference,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::lowercase,
            typefaceFormatter, &TypefaceFormatter::onLowerCase,
            Qt::DirectConnection);
    connect(editorContextMenu, &EditorContextMenu::uppercase,
            typefaceFormatter, &TypefaceFormatter::onUpperCase,
            Qt::DirectConnection);

    // Вызов диалога поиска в тексте
    connect(findDialog, &EditorFindDialog::find_text,
            this, &Editor::onFindtextSignalDetect,
            Qt::DirectConnection);

    connect(textArea, &EditorTextArea::updateIndentlineGeometry,
            indentSliderAssistant, &EditorIndentSliderAssistant::onUpdateGeometry,
            Qt::DirectConnection);

    connect(this, &Editor::updateIndentSliderGeometry,
            indentSliderAssistant, &EditorIndentSliderAssistant::onUpdateGeometry,
            Qt::DirectConnection);
}

void Editor::setupToolsSignals() {
    // Создание сигналов, генерируемых кнопками Undo / Redo
    connect(textArea->document(), &QTextDocument::undoAvailable,
            editorToolBarAssistant->undo, &QAction::setEnabled);
    connect(textArea->document(), &QTextDocument::redoAvailable,
            editorToolBarAssistant->redo, &QAction::setEnabled);
    connect(editorToolBarAssistant->undo, &QAction::triggered,
            this, &Editor::onUndo,
            Qt::DirectConnection);
    connect(editorToolBarAssistant->redo, &QAction::triggered,
            this, &Editor::onRedo,
            Qt::DirectConnection);

    // Создание сигналов, генерируемых кнопками форматирования текста
    connect(editorToolBarAssistant->bold, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onBoldClicked);

    connect(editorToolBarAssistant->italic, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onItalicClicked);

    connect(editorToolBarAssistant->underline, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onUnderlineClicked);

    connect(editorToolBarAssistant->strikeout, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onStrikeOutClicked);

    connect(editorToolBarAssistant->superscript, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onSuperScriptClicked);

    connect(editorToolBarAssistant->subscript, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onSubScriptClicked);

    connect(editorToolBarAssistant->monospace, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onMonospaceClicked);

    connect(editorToolBarAssistant->code, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onCodeClicked);

    connect(editorToolBarAssistant->clear, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onClearClicked);

    connect(editorToolBarAssistant->textOnly, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onTextOnlyClicked);

    connect(editorToolBarAssistant->fixBreakSymbol, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onFixBreakSymbolClicked);

    connect(editorToolBarAssistant->fontSelect, &EditorFontFamilyComboBox::currentFontChanged,
            typefaceFormatter, &TypefaceFormatter::onFontselectChanged);

    connect(editorToolBarAssistant->fontSize, qOverload<int>(&EditorFontSizeComboBox::currentIndexChanged),
            typefaceFormatter, &TypefaceFormatter::onFontsizeChanged);

    connect(editorToolBarAssistant->fontColor, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onFontcolorClicked);

    // Цвет фона текста
    connect(editorToolBarAssistant->backgroundColor, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onBackgroundcolorClicked);

    connect(editorToolBarAssistant->indentPlus, &QAction::triggered,
            placementFormatter, &PlacementFormatter::onIndentplusClicked);

    connect(editorToolBarAssistant->indentMinus, &QAction::triggered,
            placementFormatter, &PlacementFormatter::onIndentminusClicked);

    connect(editorToolBarAssistant->alignLeft, &QAction::triggered,
            placementFormatter, &PlacementFormatter::onAlignleftClicked);

    connect(editorToolBarAssistant->alignCenter, &QAction::triggered,
            placementFormatter, &PlacementFormatter::onAligncenterClicked);

    connect(editorToolBarAssistant->alignRight, &QAction::triggered,
            placementFormatter, &PlacementFormatter::onAlignrightClicked);

    connect(editorToolBarAssistant->alignWidth, &QAction::triggered,
            placementFormatter, &PlacementFormatter::onAlignwidthClicked);

    connect(editorToolBarAssistant->numericList, &QAction::triggered,
            listFormatter, &ListFormatter::onNumericlistClicked);

    connect(editorToolBarAssistant->dotList, &QAction::triggered,
            listFormatter, &ListFormatter::onDotlistClicked);

    // Кнопки работы с таблицами
    connect(editorToolBarAssistant->createTable, &QAction::triggered,
            tableFormatter, &TableFormatter::onCreatetableClicked);

    connect(editorToolBarAssistant->tableRemoveRow, &QAction::triggered,
            tableFormatter, &TableFormatter::onTableRemoveRowClicked);

    connect(editorToolBarAssistant->tableRemoveCol, &QAction::triggered,
            tableFormatter, &TableFormatter::onTableRemoveColClicked);

    connect(editorToolBarAssistant->tableAddRow, &QAction::triggered,
            tableFormatter, &TableFormatter::onTableAddRowClicked);

    connect(editorToolBarAssistant->tableAddCol, &QAction::triggered,
            tableFormatter, &TableFormatter::onTableAddColClicked);

    connect(editorToolBarAssistant->tableMergeCells, &QAction::triggered,
            tableFormatter, &TableFormatter::onTableMergeCellsClicked);

    connect(editorToolBarAssistant->tableSplitCell, &QAction::triggered,
            tableFormatter, &TableFormatter::onTableSplitCellClicked);

    connect(editorToolBarAssistant->tableProperties, &QAction::triggered,
            tableFormatter, &TableFormatter::onTablePropertiesClicked);

    connect(editorToolBarAssistant->reference, &QAction::triggered,
            referenceFormatter, &ReferenceFormatter::onReferenceClicked);

    connect(editorToolBarAssistant->showHtml, &QAction::triggered, this, &Editor::onShowhtmlClicked);
    connect(editorToolBarAssistant->findText, &QAction::triggered, this, &Editor::onFindtextClicked);
    connect(editorToolBarAssistant->settings, &QAction::triggered, this, &Editor::onSettingsClicked);

    connect(editorToolBarAssistant->showFormatting, &QAction::toggled, // Это не клик а переключение
            this, &Editor::onShowformattingClicked);

    // Прочие кнопки
    connect(editorToolBarAssistant->insertImageFromFile, &QAction::triggered,
            imageFormatter, &ImageFormatter::onInsertImageFromFileClicked);

    connect(editorToolBarAssistant->insertHorizontalLine, &QAction::triggered,
            typefaceFormatter, &TypefaceFormatter::onInsertHorizontalLineClicked);

    connect(editorToolBarAssistant->mathExpression, &QAction::triggered,
            mathExpressionFormatter, &MathExpressionFormatter::onMathExpressionClicked);

    connect(editorToolBarAssistant->expandEditArea, &QAction::triggered, this, &Editor::onExpandEditAreaClicked);
    connect(editorToolBarAssistant->save, &QAction::triggered, this, &Editor::onSaveClicked);
    connect(editorToolBarAssistant->showText, &QAction::triggered, this, &Editor::onShowTextClicked);
    connect(editorToolBarAssistant->toAttach, &QAction::triggered, this, &Editor::onToAttachClicked);
}

// Сборка редактора и его активизация
void Editor::assembly() {
    // Создается вертикальная область, которая должна объединить
    // линейку кнопок редактирования и область редактирования текста
    auto buttonsAndEditLayout = new QVBoxLayout(this);
    buttonsAndEditLayout->setObjectName("buttons_and_edit_layout");

    // Добавляется виджет с кнопками редактора
    buttonsAndEditLayout->addWidget(editorToolBarAssistant);

    // Добавляется виджет линейки отступов
    indentSliderAssistant->setVisible(true);
    buttonsAndEditLayout->addWidget(indentSliderAssistant->getIndentSlider());


    // Добавляется область редактирования
    buttonsAndEditLayout->addWidget(textArea);

    // Полученый набор элементов устанавливается для текущего окна
    setLayout(buttonsAndEditLayout);

    // Границы убираются, так как данный объект будет использоваться как виджет
    layout()->setContentsMargins(0, 2, 0, 0);
}

// Установка текста области редактирования
void Editor::setTextarea(QString text) {
    textArea->setHtml(text);

    // Очищается URL документа, так как документ создается "из ничего"
    textArea->document()->setMetaInformation(QTextDocument::DocumentUrl, "");
}

bool Editor::setWorkDirectory(QString dirName) {
    QDir directory(dirName);

    if (directory.exists()) {
        workDirectory = dirName;
        return true;
    } else {
        criticalError("WyEdit: Can not set work directory to " + dirName + ". Directory not exists.");
        return false;
    }
}

// Сохранение HTML кода документа в файл
bool Editor::saveTextareaText() {
    if (workFileName.length() == 0) {
        criticalError("WyEdit: Save function. Not setted work file name for editor.");
        return false;
    }

    QString fullFileName = workDirectory + "/" + workFileName;

    // Создание нового файла записи
    QFile wfile(fullFileName);

    if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        criticalError("WyEdit: Can't open file " + fullFileName + " for write.");
        return false;
    }

    qDebug() << "Write to file " << workFileName;

    QTextStream out(&wfile);
    out << textArea->document()->toHtml();

    return true;
}

/// @brief Сохранение картинок, которые присутствуют в документе, в указанную директорию
bool Editor::saveTextareaImages(int mode = SAVE_IMAGES_SIMPLE) {
    qDebug() << "Save images...\n";
    qDebug() << "Block count" << textArea->document()->blockCount() << "\n";

    if (workDirectory.length() == 0) {
        criticalError("WyEdit: Save images function. Not setted work directory.");
        return false;
    }

    // Перебираются блоки документа и находятся блоки с картинками
    QStringList imagesNames; // В список сохраняются имена найденных картинок
    QTextBlock textBlock = textArea->document()->begin();
    while (textBlock.isValid()) {
        QTextBlock::iterator it;

        for (it = textBlock.begin(); !(it.atEnd()); ++it) {
            QTextFragment currentFragment = it.fragment();
            if (currentFragment.isValid()) {
                if (currentFragment.charFormat().isImageFormat()) {
                    // Найден блок с картинкой

                    // Выясняется формат картинки
                    QTextImageFormat imgFmt = currentFragment.charFormat().toImageFormat();

                    // Из формата выясняется имя картинки
                    QString imageName = imgFmt.name();
                    imagesNames << imageName;
                    qDebug() << "Image " << imageName << "\n"; // имя файла

                    // Если файла картинки не существует
                    QString imageFileName = workDirectory + "/" + imageName;
                    QFileInfo tryFile(imageFileName);
                    if (tryFile.exists() == false) {
                        qDebug() << "Save image data to file " << imageFileName;

                        // Из ресурсов вытягивается картинка
                        QVariant imageData = textArea->document()->resource(QTextDocument::ImageResource, QUrl(imageName));

                        QImage picture = imageData.value<QImage>();

                        picture.save(imageFileName, "png");
                    }
                }
            }
        }
        textBlock = textBlock.next();
    }

    // Если при сохранении все лишние картинки должны удалиться в каталоге
    if (mode == SAVE_IMAGES_REMOVE_UNUSED) {
        // Выясняется список файлов в директории
        QDir directory(workDirectory);
        const auto imageInDirectory = directory.entryList(QDir::Files);

        // Перебираются файлы в директории
        static const QRegularExpression re("\\.png$");
        for(auto & fileName : imageInDirectory)
            if (fileName.contains(re) && !imagesNames.contains(fileName) && !miscFields["attachFileNameList"].contains(fileName))
                // удаляются *.png файлы, не встречающиеся ни в тексте записи, ни в прикрепленных файлах
                directory.remove(fileName);
    }

    qDebug() << "Save images finish";

    return true;
}

void Editor::saveTextarea() {
    qDebug() << "Save textarea...";

    // Если запись была открыта на просмотр и изменена
    if (getWorkDirectory().length() != 0 && getFileName().length() != 0 && getTextareaModified()) {
        // Перенос текущего файла записи в корзину
        qDebug() << "Try remove file " << getFileName() << " from directory " << getWorkDirectory();
        if (!QDir(getWorkDirectory()).remove(getFileName()))
            qDebug() << "Can't remove file.";

        // Если происходит прямая работа с файлом текста
        if (loadCallbackFunc == nullptr) {
            // Сохранение текста записи в файл
            saveTextareaText();

            // Сохранение картинок, которые есть в записи
            saveTextareaImages(Editor::SAVE_IMAGES_REMOVE_UNUSED);
        } else {
            // Иначе задана функция обратного вызова для записи текста и картинок

            QString content = textArea->document()->toHtml();
            saveCallbackFunc(qobject_cast<QObject *>(this), content);
        }

        // Обновляется открепляемое окно с текстом записи, если таковое открыто
        EditorShowTextDispatcher::instance()->updateWindow(this->getMiscField("id"));

        // Так как произошло сохранение,
        // отмечается что новый текст небыл еще изменен
        setTextareaModified(false);
    }
}

bool Editor::loadTextarea() {
    // Если не заданы директория или файл
    // Но установлен режим подавления ошибки
    if (workFileName.length() == 0 || workDirectory.length() == 0)
        if (dirFileEmptyReaction == DIRFILEEMPTY_REACTION_SUPPRESS_ERROR) {
            // Никаких действий с областью редактирования не делается
            // скорее всего она останется просто пустой
            return true;
        }

    if (workFileName.length() == 0) {
        criticalError("WyEdit: Load function. Not setted work file name for editor.");
        return false;
    }

    if (workDirectory.length() == 0) {
        criticalError("WyEdit: Not setted work directory for editor.");
        return false;
    }

    QString fileName = workDirectory + "/" + workFileName;

    QString content;

    // Если происходит прямая работа с файлом текста
    if (loadCallbackFunc == nullptr) {
        // Данная часть кода в MyTetra не используется, т. к. всегда задана callback-функция
        // для считывания текста записи

        // Создается объект файла с нужным именем
        QFile f(fileName);

        // Если нужный файл не существует
        if (!f.exists()) {
            criticalError("WyEdit: File " + fileName + " not found");
            return false;
        }

        // Если нужный файл недоступен для чтения
        if (!f.open(QIODevice::ReadOnly)) {
            criticalError("WyEdit: File " + fileName + " not readable. Check permission.");
            return false;
        }

        content = QString::fromUtf8(f.readAll());
    } else {
        // Иначе задана функция обратного вызова для установки начального текста
        QString resultString;
        loadCallbackFunc(qobject_cast<QObject *>(this), resultString);
        content = resultString;
    }

    // Устанавливается URL документа, с помощью него будут высчитываться
    // относительные ссылки при загрузке картинок
    textArea->document()->setMetaInformation(QTextDocument::DocumentUrl, "file:" + fileName);

    qDebug() << "Editor::load_textarea() Set document URL to " << fileName;

    // Текст из файла вставляется в область редактирования
    // QString content=QString::fromUtf8(f.readAll());
    // qDebug() << "Load content:";
    // qDebug() << content;
    textArea->setHtml(content);

    // qDebug() << "Set content:";
    // qDebug() << textArea->toHtml();

    return true;
}

// Умное преобразование имени шрифта
QString Editor::smartFontFamily(QString fontName) {
    // Коррекция имен шрифтов
    if (fontName == "") { // Если имени шрифта просто нет
        // Имя шрифта берется из конфига до первой запятой (потому что после запятой идут еще параметры шрифта)
        fontName = editorConfig->get_default_font();
        int firstCommaPos = fontName.indexOf(",");
        if (firstCommaPos > 0)
            fontName = fontName.left(firstCommaPos);
    } else if (fontName == "Sans" && editorToolBarAssistant->fontSelect->findText(fontName) == -1)
        fontName = "Sans Serif";

    return fontName;
}

/////////////////////////////////////////////////
// Форматирование текста
/////////////////////////////////////////////////

// Слот вызывается при каждом движении курсора в момент выделения текста
void Editor::onSelectionChanged() {
    // Если выделения нет
    if (!textArea->textCursor().hasSelection())
        return;

    // qDebug() << "Selection text " << textarea->textCursor().selection().toHtml();

    // Позиции начала и конца выделения
    int start = textArea->textCursor().selectionStart();
    int stop = textArea->textCursor().selectionEnd();
    // qDebug() << "Selection start " << start << " stop " << stop;

    // Создается дополнительный курсор, который будет пробегать по выделенному тексту
    QTextCursor cursor = textArea->textCursor();
    cursor.setPosition(start);

    // Выравнивание относится к форматированию строк, начальное состояние
    // берётся из начального положения курсора
    Qt::Alignment startAlign = cursor.blockFormat().alignment();

    // Курсор сдвигается на одну позицию вперёд
    cursor.movePosition(QTextCursor::NextCharacter);

    // Для анализа форматирования символов надо начинать
    // с позиции, следующей справа от начала выделения
    QString startFontFamily = smartFontFamily(cursor.charFormat().font().family()); // Шрифт
    qreal startSize = smartFontSize(cursor.charFormat().fontPointSize());           // Размер шрифта
    bool startBold = false;
    if (cursor.charFormat().fontWeight() == QFont::Bold)
        startBold = true;                                      // Толщина
    bool startItalic = cursor.charFormat().fontItalic();       // Наклон
    bool startUnderline = cursor.charFormat().fontUnderline(); // Подчеркивание
    bool startStrikeOut = cursor.charFormat().fontStrikeOut(); // Зачеркивание

    bool differentFontFlag = false;
    bool differentSizeFlag = false;
    bool differentBoldFlag = false;
    bool differentItalicFlag = false;
    bool differentUnderlineFlag = false;
    bool differentStrikeOutFlag = false;
    bool differentAlignFlag = false;

    // Слишком большие выделения текста нельзя обрабатывать, так как выделение становится слишком медленным
    if (abs(stop - start) > WYEDIT_MAX_SELECTION_SIZE_WHILE_CHECK_TOOLBAR_ITEM) {
        differentFontFlag = true;
        differentSizeFlag = true;
        differentBoldFlag = true;
        differentItalicFlag = true;
        differentUnderlineFlag = true;
        differentStrikeOutFlag = true;
        differentAlignFlag = true;
    } else {
        // Программа пробегает по выделенному куску текста дополнительным курсором
        // и выясняет, есть ли разные шрифты, разные размеры,
        // разные начертания символов, разное выравнивание в выделенном тексте
        while (cursor.position() <= stop) {
            if (differentFontFlag == false && startFontFamily != smartFontFamily(cursor.charFormat().font().family()))
                differentFontFlag = true;

            if (differentSizeFlag == false && startSize != smartFontSize(cursor.charFormat().fontPointSize()))
                differentSizeFlag = true;

            if (differentBoldFlag == false) {
                int b = cursor.charFormat().fontWeight();
                if (startBold == false && b == QFont::Bold)
                    differentBoldFlag = true;
                else if (startBold == true && b == QFont::Normal)
                    differentBoldFlag = true;
            }

            if (differentItalicFlag == false && startItalic != cursor.charFormat().fontItalic())
                differentItalicFlag = true;

            if (differentUnderlineFlag == false && startUnderline != cursor.charFormat().fontUnderline())
                differentUnderlineFlag = true;

            if (differentStrikeOutFlag == false && startStrikeOut != cursor.charFormat().fontStrikeOut())
                differentStrikeOutFlag = true;

            if (differentAlignFlag == false && startAlign != cursor.blockFormat().alignment())
                differentAlignFlag = true;

            // Курсор передвигается на одну позицию вперед
            // Если дальше двигаться некуда (конец документа) цикл досрочно завершается
            if (cursor.movePosition(QTextCursor::NextCharacter) == false)
                break;
        }
    }

    // Список выбора шрифта начинает указывать на нужный шрифт
    if (differentFontFlag == false)
        emit changeFontselectOnDisplay(startFontFamily); // Если всё выделение одним шрифтом
    else
        emit changeFontselectOnDisplay("");

    // Список выбора размера начинает указывать на нужный размер
    if (differentSizeFlag == false) { // Если всё отформатировано одним размером
        int size = (int)startSize;
        if (size == 0) { // Если размер шрифта не определен (т.е. размер шрифта для текста просто не установлен)
            size = editorConfig->get_default_font_size();
        }
        emit changeFontsizeOnDisplay(size);
    } else
        emit changeFontsizeOnDisplay(0); // В выделении есть разные размеры

    // Кнопка Bold выключается, если есть разное Bold форматирование
    // и включается, если форматирование одинаковое,
    // и выделение начиналось с Bold
    if (differentBoldFlag == true)
        editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_BOLD, false);
    else if (startBold == true)
        editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_BOLD, true);

    // Кнопка Italic
    if (differentItalicFlag == true)
        editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_ITALIC, false);
    else if (startItalic == true)
        editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_ITALIC, true);

    // Кнопка Underline
    if (differentUnderlineFlag == true)
        editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_UNDERLINE, false);
    else if (startUnderline == true)
        editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_UNDERLINE, true);

    // Кнопка StrikeOut
    if (differentStrikeOutFlag == true)
        editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_STRIKEOUT, false);
    else if (startStrikeOut == true)
        editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_STRIKEOUT, true);

    // Кнопки выравнивания
    emit updateAlignButtonHiglight(!differentAlignFlag);
}

// Слот вызывается при каждом перемещении курсора
void Editor::onCursorPositionChanged() {
    // Если одновременно идет режим выделения
    // то обслуживание текущего шрифта и размера идет в on_selection_changed()
    if (textArea->textCursor().hasSelection())
        return;

    editorToolBarAssistant->updateToActualFormat();

    emit updateIndentsliderToActualFormat();
}

// Событие отлавливает нажатия клавиш
void Editor::keyPressEvent(QKeyEvent *event) {
    if (editorToolBarAssistant->isKeyForToolLineUpdate(event))
        editorToolBarAssistant->updateToActualFormat();

    QWidget::keyPressEvent(event);
}

// Событие отлавливает отжатия клавиш
void Editor::keyReleaseEvent(QKeyEvent *event) {
    if (editorToolBarAssistant->isKeyForToolLineUpdate(event))
        editorToolBarAssistant->updateToActualFormat();

    QWidget::keyReleaseEvent(event);
}

void Editor::onUndo() {
    qDebug() << "Undo slot normal running ";
    textArea->undo();
    editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}

void Editor::onRedo() {
    qDebug() << "Redo slot normal running ";
    textArea->redo();
    editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}

void Editor::onCut() {
    textArea->cut();
    editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}

void Editor::onCopy() {
    // Если текущий виджет не основной виджет редактирования текста
    if (!textArea->hasFocus()) {

        auto focusWidget = qApp->focusWidget(); // Выясняется, какой виджет выбран

        // Если это надпись QLabel
        // Для виджетов, существующих рядом с textArea в рамках виджета Editor,
        // сочетание клавиш копирования не перекрывается самим виджетом.
        // Поэтому нужно вручную заполнить буфер обмена.
        // Это поведения стало необходимо после введения подсистемы горячих клавиш
        if (QString(focusWidget->metaObject()->className()) == "QLabel") {
            QLabel *label = static_cast<QLabel *>(focusWidget);

            if (label->selectedText().size() > 0) {
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(label->selectedText());
            }
        }

        return; // Закончилась обработка что текущий виджет не основной виджет редактирования текста
    }

    // Если выбрана только картинка или курсор стоит на позиции картинки
    if (cursorPositionDetector->isImageSelect() || cursorPositionDetector->isCursorOnImage()) {
        QTextImageFormat imageFormat;

        if (cursorPositionDetector->isImageSelect())
            imageFormat = imageFormatter->imageFormatOnSelect();

        if (cursorPositionDetector->isCursorOnImage())
            imageFormat = imageFormatter->imageFormatOnCursor();

        // Из формата выясняется имя картинки
        QString imageName = imageFormat.name();

        // Из ресурсов вытягивается картинка
        QVariant imageData = textArea->document()->resource(QTextDocument::ImageResource, QUrl(imageName));
        QImage image = imageData.value<QImage>();

        // Создается ссылка на буфер обмена
        QClipboard *clipboard = QApplication::clipboard();

        // Копирование картинки в буфер обмена
        clipboard->setImage(image);
    } else
        textArea->copy(); // Обычное копирование

    editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}

// Обработка команды Paste контекстного меню
void Editor::onPaste() {
    // В Qt обнаружен баг, заключающийся в том, что при установке ReadOnly на область редактирования,
    // все равно можно сделать вставку текста (и замену выделенного текста на вставляемый) через контекстное меню.
    // Здесь происходит блокировка этого действия
    if (textArea->isReadOnly())
        return;

    textArea->paste();
    editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}

// Обработка команды PasteAsPlainText контекстного меню
void Editor::onPasteAsPlainText() {
    // В Qt обнаружен баг, заключающийся в том, что при установке ReadOnly на область редактирования,
    // все равно можно сделать вставку текста (и замену выделенного текста на вставляемый) через контекстное меню.
    // Здесь происходит блокировка этого действия
    if (textArea->isReadOnly())
        return;

    textArea->insertPlainText(QGuiApplication::clipboard()->text());
    editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}

void Editor::onSelectAll() {
    textArea->selectAll();
    editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}

// Показывание окна с исходным текстом HTML
void Editor::onShowhtmlClicked() {
    MultiLineInputDialog dialog(this);

    dialog.setText(textArea->toHtml());
    dialog.setWindowTitle(tr("Edit HTML source"));

    if (dialog.exec() != QDialog::Accepted)
        return;

    textArea->setHtml(dialog.getText());

    // Если в диалоговом окне был иземен HTML код
    if (dialog.isModified())
        textArea->document()->setModified(true);
}

void Editor::onFindtextClicked() {
    findDialog->show();
    findDialog->activateWindow();
}

// Слот, принимающий данные от окна поиска текста
void Editor::onFindtextSignalDetect(const QString &text, QTextDocument::FindFlags flags) {
    qDebug() << "Find text " << text << " with flags " << flags;

    if (!textArea->find(text, flags)) {
        findDialog->hide();
        QMessageBox::information(this,
                                 tr("Search result"),
                                 tr("String '<b>") + text + tr("</b>' not found"),
                                 QMessageBox::Close);
    }
}

// Открытие контекстного меню в области редактирования
void Editor::onCustomContextMenuRequested(const QPoint &pos) {
    qDebug() << "In Editor on_customContextMenuRequested";

    // В контекстном меню выставляются допустимые пункты
    editorContextMenu->update();

    // Контекстное меню запускается
    editorContextMenu->exec(textArea->viewport()->mapToGlobal(pos));
}

void Editor::onSettingsClicked() {
    // Создается окно настроек, после выхода из этой функции окно удалится
    EditorConfigDialog dialog;
    dialog.show();

    // Устанавка размера табуляции для клавиши Tab
    setTabSize();
}

void Editor::onShowformattingClicked(bool ok) {
    textArea->setShowFormatting(ok);
}

void Editor::onExpandEditAreaClicked() {
    expandEditAreaFlag = !expandEditAreaFlag;
    emit send_expand_edit_area(expandEditAreaFlag);
}

void Editor::onSaveClicked() {
    saveTextarea();
}

void Editor::onToAttachClicked() {
    attachCallbackFunc();
}

// Слот нажатия кнопки показа текста в отдельном открепляемом окне
void Editor::onShowTextClicked() {
    if (textArea->toPlainText().length() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Can't show empty text in detached window");
        msgBox.exec();

        return;
    }

    EditorShowTextDispatcher::instance()->createWindow(this->getMiscField("id"));
}

void Editor::setTabSize() {
    // Устанавка размера табуляции для клавиши Tab
    // Учитываем среднюю ширину глифов в шрифте
    textArea->setTabStopDistance(
        QFontMetrics(textArea->currentCharFormat().font()).averageCharWidth() *
        editorConfig->get_tab_size());
}

void Editor::setMiscField(QString name, QString value) {
    miscFields[name] = value;
}

QString Editor::getMiscField(QString name) {
    if (miscFields.contains(name))
        return miscFields[name];
    else
        return QString();
}

void Editor::clearAllMiscField() {
    miscFields.clear();
}

void Editor::setDirFileEmptyReaction(int mode) {
    // Проверяется допустимость переданного значения
    if (mode == DIRFILEEMPTY_REACTION_SHOW_ERROR || mode == DIRFILEEMPTY_REACTION_SUPPRESS_ERROR)
        dirFileEmptyReaction = mode;
    else
        criticalError("Editor::setDirFileEmptyReaction() : Unsupport mode " + QString::number(mode));
}

void Editor::setCursorPosition(int n) {
    QTextCursor cursor = textArea->textCursor();

    cursor.setPosition(n);

    textArea->setTextCursor(cursor);
}
