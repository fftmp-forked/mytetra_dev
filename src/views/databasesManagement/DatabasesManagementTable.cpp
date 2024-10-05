#include <QApplication>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QPainter>

#include "DatabasesManagementScreen.h"
#include "DatabasesManagementTable.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"

void DatabasesManagementTable::init() {
    this->horizontalHeader()->setStretchLastSection(true);      // Растягивание последней секции до размеров виджета
    this->setSelectionBehavior(QAbstractItemView::SelectRows);  // Выделяется вся строка
    this->horizontalHeader()->setHighlightSections(false);      // Заголовки не должны выглядеть нажатыми
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключается горизонтальная прокрутка

    // Установка ширины и высоты виджета
    int dialogWidth = MainWindow::get().width() * 4 / 5;
    int dialogHeight = MainWindow::get().height() * 4 / 5;
    setMinimumWidth(dialogWidth);
    setMinimumHeight(dialogHeight);
    resize(size());

    // Установка ширины столбцов
    // this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive); // Interactive, ResizeToContents
    this->resizeColumnToContents(0);

    assemblyContextMenu();
    setContextMenuPolicy(Qt::CustomContextMenu);

    setupSignals();
}

void DatabasesManagementTable::setupSignals(void) {
    // Сигнал чтобы показать контекстное меню по правому клику на списке записей
    connect(this, &DatabasesManagementTable::customContextMenuRequested,
            this, &DatabasesManagementTable::onCustomContextMenuRequested);
}

void DatabasesManagementTable::resizeEvent(QResizeEvent *event) {
    // Отрисовка родительского класса
    QTableView::resizeEvent(event);
}

void DatabasesManagementTable::paintEvent(QPaintEvent *event) {
    QTableView::paintEvent(event);

    // Если нет записей лога
    if (model() != nullptr)
        if (model()->rowCount() == 0 && AppConfig::get().getEnableLogging() == false) {
            QPainter painter(viewport());
            painter.setPen(QApplication::palette().color(QPalette::ToolTipText)); // Qt::gray
            // painter.setFont(QFont("Arial", 14));
            painter.drawText(rect(), Qt::AlignCenter, tr("Action log is empty\nPlease enable action logging in Tools -> Preferences -> Misc"));
        }
}

void DatabasesManagementTable::assemblyContextMenu() {
    DatabasesManagementScreen *parentPointer = qobject_cast<DatabasesManagementScreen *>(parent());

    contextMenu.addAction(parentPointer->actionCopy);
}

// Открытие контекстного меню в таблице записей лога
void DatabasesManagementTable::onCustomContextMenuRequested(const QPoint &pos) {
    qDebug() << "In DatabasesManagementTable::on_customContextMenuRequested";

    // Включение отображения меню на экране
    // menu.exec(event->globalPos());
    contextMenu.exec(viewport()->mapToGlobal(pos));
}
