#include <QDir>
#include <QIcon>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QTextDocumentFragment>
#include <QTextEdit>
#include <QWidget>
#include <QtDebug>

#include "AddNewRecord.h"
#include "libraries/GlobalParameters.h"
#include "libraries/ShortcutManager/ShortcutManager.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/DiskHelper.h"

AddNewRecord::AddNewRecord(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {
    setupUI();
    setupShortcuts();
    setupSignals();
    assembly();

    setupEventFilter();
}

void AddNewRecord::setupUI(void) {
    this->setWindowTitle(tr("Enter a new note"));

    // Редактор текста записи
    recordTextEditor.initEnableAssembly(true);
    recordTextEditor.initConfigFileName(GlobalParameters::get().get_cfg_dir() + "/editorconf.ini");
    recordTextEditor.initDisableToolList(QStringList() << "save"
                                                       << "showText"
                                                       << "toAttach"
                                                       << "settings"
                                                       << "expandEditArea");
    recordTextEditor.init();

    // В редакторе текста записи устанавливается пустой текст, что активирует правильное
    // начальное положение различных кнопок редактора, например кнопок цвета текста
    recordTextEditor.setTextarea("");

    // Кнопки OK и Cancel
    buttonBox.setOrientation(Qt::Horizontal);
    buttonBox.setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::NoButton | QDialogButtonBox::Cancel);

    // На кнопку OK назначается комбинация клавиш Ctrl+Enter и она устанавливается как кнопка по умолчанию
    QPushButton *OkButton = buttonBox.button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
    OkButton->setAutoDefault(true);
    OkButton->setDefault(true);

    // С кнопки Cancel снимается флаг срабатывания по умолчанию,
    // это нужно для тулкитов основанных на GTK и прочих подобных библиотеках
    QPushButton *CancelButton = buttonBox.button(QDialogButtonBox::Cancel); // Выясняется указатель на кнопку Cancel
    CancelButton->setAutoDefault(false);
    CancelButton->setDefault(false);
}

void AddNewRecord::setupShortcuts(void) {
    qDebug() << "Setup shortcut for" << staticMetaObject.className();

    // На кнопку OK назначается комбинация клавиш Ctrl+Enter
    QPushButton *okButton = buttonBox.button(QDialogButtonBox::Ok);                                                  // Выясняется указатель на кнопку OK
    okButton->setShortcut(ShortcutManager::get().getKeySequence(ShortcutManager::SECTION_MISC, "editConfirm"));      // Устанавливается шорткат
    okButton->setToolTip(ShortcutManager::get().getKeySequenceAsText(ShortcutManager::SECTION_MISC, "editConfirm")); // ToolTip зависит от шортката
}

void AddNewRecord::setupSignals(void) {
    connect(&buttonBox, &QDialogButtonBox::accepted, this, &AddNewRecord::okClick);
    connect(&buttonBox, &QDialogButtonBox::rejected, this, &AddNewRecord::reject);

    // Обновление горячих клавиш, если они были изменены
    connect(&ShortcutManager::get(), &ShortcutManager::updateWidgetShortcut, this, &AddNewRecord::setupShortcuts);
}

void AddNewRecord::assembly(void) {
    // Размещалка элементов
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(10);

    // Добавление элементов в размещалку
    layout->addWidget(&infoField);
    layout->addWidget(&recordTextEditor);
    layout->addWidget(&buttonBox, 0, Qt::AlignRight);

    setLayout(layout);

    // Фокус устанавливается на поле ввода названия записи
    infoField.setFocusToStart();
}

void AddNewRecord::setupEventFilter(void) {
    // Для области редактирования задается eventFilter (используется для отлова нажатия на ESC)
    recordTextEditor.installEventFilter(this);
}

bool AddNewRecord::eventFilter(QObject *object, QEvent *event) {
    // qDebug() << "Editor::eventFilter()";

    // Отслеживание нажатия ESC в области редактирования текста
    if (object == &recordTextEditor) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape) {
                // qDebug() << "Press ESC key";
                close();
                return true;
            }
        }
    }

    return QDialog::eventFilter(object, event); /// @todo: тут подумать, возможно здесь надо возвращать false
}

void AddNewRecord::okClick(void) {
    QString message = "";

    // Проверка наличия названия записи
    if (infoField.getField("name").length() == 0)
        message += tr("Please enter the note's <b>title</b>. ");

    // Если что-то не заполнено, выдается предупреждение
    if (message.length() > 0) {
        QMessageBox::warning(this, tr("A new note cannot be added"), message, QMessageBox::Close);
        return;
    }

    // Картинки сохраняются
    imagesDirName = DiskHelper::createTempDirectory();
    recordTextEditor.setWorkDirectory(imagesDirName);
    recordTextEditor.saveTextareaImages(Editor::SAVE_IMAGES_SIMPLE);

    emit(accept());
}

QString AddNewRecord::getImagesDirectory(void) {
    if (imagesDirName.length() == 0) {
        criticalError("In add new record function can not generate temp directory with saved images.");
        return "";
    }

    return imagesDirName;
}

/// @brief Получение полей, заполненных в окне добавления записи
QString AddNewRecord::getField(QString name) {
    if (name == "name" || name == "author" || name == "url" || name == "tags")
        return infoField.getField(name);

    if (name == "text")
        return recordTextEditor.getTextarea();

    // Если запрашиваемого поля нет, возвращается пустая строка
    return QString();
}
