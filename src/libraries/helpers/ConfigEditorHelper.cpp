#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include "ConfigEditorHelper.h"
#include "DebugHelper.h"
#include "MultiLineInputDialog.h"
#include "../../views/mainWindow/MainWindow.h"

/// @brief Редактирование произвольного конфиг-файла (конфиг программы, конфиг редактора)
void ConfigEditorHelper::editConfigFile(QString fileName, double sizeCoefficient) {
    // Окно диалога для редактирования файла конфига
    MultiLineInputDialog dialog(&MainWindow::get());
    dialog.setWordWrapMode(QTextOption::NoWrap);
    dialog.setWindowTitle(QObject::tr("Edit config file (Be careful!)"));
    dialog.setSizeCoefficient(sizeCoefficient);

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        criticalError("Can't open config file " + fileName);
    }

    // Установка в окне текста файла
    dialog.setText(QString::fromUtf8(file.readAll()));

    // Текст из файла получен, файл больше не требуется
    file.close();

    // Если была нажата отмена
    if (dialog.exec() != QDialog::Accepted)
        return;

    // Если текст в окне не менялся
    if (!dialog.isModified())
        return;

    // Файл конфига открывается на запись
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        criticalError("Can't open config file for write: " + fileName);
    }

    // Измененный текст записывается в файл
    QTextStream out(&file);
    out << dialog.getText();

    // Текст записан, файл больше не требуется
    file.close();

    // Для принятия изменений требуется перезапустить программу
    QMessageBox::warning(&MainWindow::get(),
                         QObject::tr("Warning"),
                         QObject::tr("Program will be restarted to apply changes."),
                         QMessageBox::Ok);
    exit(0);
}
