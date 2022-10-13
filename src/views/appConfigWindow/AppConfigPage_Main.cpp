#include <QAbstractSpinBox>
#include <QBoxLayout>
#include <QCommonStyle>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QToolButton>
#include <QWidget>

#include "AppConfigPage_Main.h"
#include "models/appConfig/AppConfig.h"

AppConfigPage_Main::AppConfigPage_Main(QWidget *parent) : ConfigPage(parent) {
    setupUi();
    setupSignals();
    assembly();
}

void AppConfigPage_Main::setupUi(void) {
    qDebug() << "Create main config page";

    // Блок работы с путем до каталога данных
    tetradirLabel = new QLabel(this);
    tetradirLabel->setText(tr("Data directory"));

    tetradirInput = new QLineEdit(this);
    tetradirInput->setMinimumWidth(50);
    tetradirInput->setText(AppConfig::get().get_tetradir());

    tetradirButton = new QToolButton(this);
    tetradirButton->setText(tr("..."));

    // Блок работы с выбором языка интерфейса
    interfaceLanguageLabel = new QLabel(this);
    interfaceLanguageLabel->setText(tr("Language"));

    interfaceLanguage = new QComboBox(this);
    interfaceLanguage->setMinimumContentsLength(2);
    interfaceLanguage->addItem(QIcon(":/resource/pic/countryFlags/United-Kingdom.png"), "en");
    interfaceLanguage->addItem(QIcon(":/resource/pic/countryFlags/France.png"), "fr");
    interfaceLanguage->addItem(QIcon(":/resource/pic/countryFlags/Russia.png"), "ru");
    interfaceLanguage->setCurrentIndex(interfaceLanguage->findText(AppConfig::get().get_interfacelanguage(), Qt::MatchCaseSensitive));

    // Блок работы с отображением даты и времени
    dateTimeFormatBox = new QGroupBox(this);
    dateTimeFormatBox->setTitle(tr("Displaying date and time format"));

    disableCustomDateTimeFormat = new QRadioButton(tr("Locale settings"), this);
    enableCustomDateTimeFormat = new QRadioButton(tr("Custom format"), this);
    customDateTimeFormat = new QLineEdit(this);
    dateTimeFormatHelpButton = new QToolButton(this);
    QCommonStyle styleHelp;
    dateTimeFormatHelpButton->setIcon(styleHelp.standardIcon(QStyle::SP_MessageBoxQuestion));

    // Точка устанавливается возле того пункта, который настроен в конфиге
    if (AppConfig::get().getEnableCustomDateTimeFormat() == false) {
        disableCustomDateTimeFormat->setChecked(true);
        enableCustomDateTimeFormat->setChecked(false);
        customDateTimeFormat->setEnabled(false);
    } else {
        // Если разрешен пользовательский формат даты и времени
        disableCustomDateTimeFormat->setChecked(false);
        enableCustomDateTimeFormat->setChecked(true);
        customDateTimeFormat->setEnabled(true);
        customDateTimeFormat->setText(AppConfig::get().getCustomDateTimeFormat());
    }
}

AppConfigPage_Main::~AppConfigPage_Main(void) {
}

void AppConfigPage_Main::setupSignals(void) {
    connect(tetradirButton, &QToolButton::clicked, this, &AppConfigPage_Main::onClickedTetradirSelectDialog);

    connect(disableCustomDateTimeFormat, &QRadioButton::toggled, this, &AppConfigPage_Main::onDisableCustomDateTimeFormatToggle);
    connect(enableCustomDateTimeFormat, &QRadioButton::toggled, this, &AppConfigPage_Main::onEnableCustomDateTimeFormatToggle);

    connect(dateTimeFormatHelpButton, &QToolButton::clicked, this, &AppConfigPage_Main::onDateTimeFormatHelpButton);
}

void AppConfigPage_Main::assembly(void) {
    // Блок работы с путем до каталога данных
    QHBoxLayout *tetradirLayout = new QHBoxLayout();
    tetradirLayout->addWidget(tetradirInput);
    tetradirLayout->addWidget(tetradirButton);

    // Блок работы с выбором языка интерфейса
    QHBoxLayout *interfaceLanguageLayout = new QHBoxLayout();
    interfaceLanguageLayout->addWidget(interfaceLanguage);

    // Блок работы с отображением даты и времени
    QVBoxLayout *dateTimeFormatVBox = new QVBoxLayout();
    QHBoxLayout *dateTimeFormatHBox = new QHBoxLayout();
    dateTimeFormatVBox->addWidget(disableCustomDateTimeFormat);
    dateTimeFormatVBox->addWidget(enableCustomDateTimeFormat);

    dateTimeFormatHBox->addWidget(customDateTimeFormat);
    dateTimeFormatHBox->addWidget(dateTimeFormatHelpButton);

    dateTimeFormatVBox->addLayout(dateTimeFormatHBox);
    dateTimeFormatBox->setLayout(dateTimeFormatVBox);

    // Сборка всех блоков
    QGridLayout *dirLayout = new QGridLayout();
    dirLayout->addWidget(tetradirLabel, 0, 0);
    dirLayout->addLayout(tetradirLayout, 0, 1);

    QGridLayout *otherSettingLayout = new QGridLayout();

    otherSettingLayout->addWidget(interfaceLanguageLabel, 2, 0);
    otherSettingLayout->addLayout(interfaceLanguageLayout, 2, 1);

    otherSettingLayout->setColumnStretch(1, 100);

    QVBoxLayout *centralLayout = new QVBoxLayout();
    centralLayout->addLayout(dirLayout);
    centralLayout->addLayout(otherSettingLayout);
    centralLayout->addWidget(dateTimeFormatBox);
    centralLayout->addStretch();

    setLayout(centralLayout);
}

// Действия при нажатии кнопки выбора директории данных
void AppConfigPage_Main::onClickedTetradirSelectDialog(void) {
    QFileDialog tetradirSelectDialog(this);
    tetradirSelectDialog.setFileMode(QFileDialog::Directory);
    tetradirSelectDialog.setWindowTitle(tr("Select data directory"));
    tetradirSelectDialog.setDirectory(tetradirInput->text());

    if (tetradirSelectDialog.exec())
        if (!tetradirSelectDialog.directory().absolutePath().isEmpty())
            tetradirInput->setText(tetradirSelectDialog.directory().absolutePath());
}

// Действия при нажатии на радиокнопку системных установок даты и времени
void AppConfigPage_Main::onDisableCustomDateTimeFormatToggle(bool checked) {
    qDebug() << "In onDisableCustomDateTimeFormat(): " << checked;
    if (checked == true) {
        customDateTimeFormat->setEnabled(false);
        customDateTimeFormat->setText("");
    }
}

// Действия при нажатии на радиокнопку пользовательских установок даты и времени
void AppConfigPage_Main::onEnableCustomDateTimeFormatToggle(bool checked) {
    qDebug() << "In onEnableCustomDateTimeFormat(): " << checked;

    if (checked == true)
        customDateTimeFormat->setEnabled(true);
}

void AppConfigPage_Main::onDateTimeFormatHelpButton(void) {
    QString text = "Format symbols:\n\
\n\
d: day 1-31, dd: day 01-31,\n\
ddd: day short name, dddd: day long name,\n\
M: month 1-12, MM: month 01-12,\n\
MMM: month short name, MMMM: month long name,\n\
yy: year 00-99, yyyy: year 0000-9999,\n\
h: hour 0-23, hh: hour 00-23,\n\
H: hour 0-23 AM/PM, HH: hour 00-23 AM/PM,\n\
n: minute 0-59, mm: minute 00-59,\n\
s: second 0-59, ss: second 00-59,\n\
AP: use AM/PM display, ap: use am/pm display,\n\
t: timezone\n\
\n\
Sample: dd.MM.yyyy - hh:mm:ss";

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Main::applyChanges(void) {
    qDebug() << "Apply changes main";

    int difficultChanges = 0;

    // Если был изменен путь к базе, он запоминается в конфигфайл
    if (AppConfig::get().get_tetradir() != tetradirInput->text()) {
        QDir dir(tetradirInput->text());

        // Проверяется, допустимо ли имя директории
        if (dir.isReadable() == false || dir.exists() == false)
            QMessageBox::warning(this, tr("Warning"),
                                 tr("The data directory does not exists or unavailable for reading."),
                                 QMessageBox::Ok);
        else {
            // Новое имя запоминается в конфиг
            AppConfig::get().set_tetradir(tetradirInput->text());
            difficultChanges = 1;
        }
    }

    // Если было изменено использование пользовательского формата даты и времени
    if (AppConfig::get().getEnableCustomDateTimeFormat() != enableCustomDateTimeFormat->isChecked())
        AppConfig::get().setEnableCustomDateTimeFormat(enableCustomDateTimeFormat->isChecked());

    // Если было изменен пользовательский формат даты и времени
    if (AppConfig::get().getCustomDateTimeFormat() != customDateTimeFormat->text())
        AppConfig::get().setCustomDateTimeFormat(customDateTimeFormat->text());

    // Если был изменен язык
    if (AppConfig::get().get_interfacelanguage() != interfaceLanguage->currentText()) {
        AppConfig::get().set_interfacelanguage(interfaceLanguage->currentText());
        difficultChanges = 1;
    }

    return difficultChanges;
}
