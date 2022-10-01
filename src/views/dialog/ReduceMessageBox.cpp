#include <QVBoxLayout>

#include "ReduceMessageBox.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/helpers/ObjectHelper.h"


ReduceMessageBox::ReduceMessageBox(QWidget *parent) : QDialog(parent) {
  setupUI();
  setupSignals();
  assembly();
}


void ReduceMessageBox::setupUI(void) {
  int w=find_object<MainWindow>("mainwindow")->width();
  int h=find_object<MainWindow>("mainwindow")->height();

  this->resize(w*2/3, h/3);
}


void ReduceMessageBox::setupSignals(void) {
  connect(&buttonBox, &QDialogButtonBox::accepted, this, &ReduceMessageBox::accept);
  connect(&buttonBox, &QDialogButtonBox::rejected, this, &ReduceMessageBox::reject);
}


void ReduceMessageBox::assembly(void) {
  // Размещалка элементов
  QVBoxLayout *layout=new QVBoxLayout(this);

  layout->addWidget(&text);
  layout->addWidget(&detailedText);
  layout->addWidget(&buttonBox);
}

