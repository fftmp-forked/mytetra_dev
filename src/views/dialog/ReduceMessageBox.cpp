#include <QVBoxLayout>

#include "ReduceMessageBox.h"
#include "views/mainWindow/MainWindow.h"

ReduceMessageBox::ReduceMessageBox(QWidget *parent) : QDialog(parent) {
    setupUI();
    setupSignals();
    assembly();
}

void ReduceMessageBox::setupUI(void) {
    this->resize(MainWindow::get().width() * 2 / 3, MainWindow::get().height() / 3);
}

void ReduceMessageBox::setupSignals(void) {
    connect(&buttonBox, &QDialogButtonBox::accepted, this, &ReduceMessageBox::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, this, &ReduceMessageBox::reject);
}

void ReduceMessageBox::assembly(void) {
    // Размещалка элементов
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(&text);
    layout->addWidget(&detailedText);
    layout->addWidget(&buttonBox);
}
