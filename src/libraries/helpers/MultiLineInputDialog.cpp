#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "../ShortcutManager/ShortcutManager.h"
#include "MultiLineInputDialog.h"

MultiLineInputDialog::MultiLineInputDialog(QWidget *parent) : QDialog(parent) {
    sizeCoefficient = 1.0;

    setupUi();
    setupShortcuts();
    setupSignals();
    assembly();
}

MultiLineInputDialog::~MultiLineInputDialog() {
    if (textArea != NULL)
        delete textArea;

    if (buttonBox != NULL)
        delete buttonBox;
}

void MultiLineInputDialog::setupUi() {
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    textArea = new QTextEdit(this);
    textArea->setAcceptRichText(false);
    textArea->setSizePolicy(sizePolicy);

    buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::NoButton | QDialogButtonBox::Cancel);

    // На кнопку OK назначается комбинация клавиш Ctrl+Enter и она устанавливается как кнопка по умолчанию
    QPushButton *OkButton = buttonBox->button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
    OkButton->setAutoDefault(true);
    OkButton->setDefault(true);

    // С кнопки Cancel снимается флаг срабатывания по умолчанию,
    // это нужно для тулкитов основанных на GTK и прочих подобных библиотеках
    QPushButton *CancelButton = buttonBox->button(QDialogButtonBox::Cancel); // Выясняется указатель на кнопку Cancel
    CancelButton->setAutoDefault(false);
    CancelButton->setDefault(false);

    // Устанавливается размер окна, основанный на размере виджета, из которого этот виджет был вызван
    if (this->parent() != nullptr)
        if (this->parent()->isWidgetType())
            updateSize();
}

void MultiLineInputDialog::setupShortcuts(void) {
    qDebug() << "Setup shortcut for" << staticMetaObject.className();

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);                                                 // Выясняется указатель на кнопку OK
    okButton->setShortcut(ShortcutManager::get().getKeySequence(ShortcutManager::SECTION_MISC, "editConfirm"));      // Устанавливается шорткат
    okButton->setToolTip(ShortcutManager::get().getKeySequenceAsText(ShortcutManager::SECTION_MISC, "editConfirm")); // ToolTip зависит от шортката
}

void MultiLineInputDialog::setupSignals() {
    connect(buttonBox, &QDialogButtonBox::accepted, this, &MultiLineInputDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MultiLineInputDialog::reject);

    // Обновление горячих клавиш, если они были изменены
    connect(&ShortcutManager::get(), &ShortcutManager::updateWidgetShortcut, this, &MultiLineInputDialog::setupShortcuts);
}

void MultiLineInputDialog::assembly() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Добавляется область текста
    mainLayout->addWidget(textArea);

    // Добавляется линейка кнопок OK и Cancel
    mainLayout->addWidget(buttonBox);
}

void MultiLineInputDialog::updateSize() {
    QWidget *parentWidget = qobject_cast<QWidget *>(this->parent());
    QRect geom(parentWidget->pos(), parentWidget->size());

    geom.setHeight((int)(geom.height() * sizeCoefficient));
    geom.setWidth((int)(geom.width() * sizeCoefficient));

    qDebug() << "Parent window geometry " << geom.x() << geom.y() << geom.width() << geom.height();

    setMinimumSize(geom.width(), geom.height());
}
