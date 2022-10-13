#include <QApplication>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QPainter>

#include "AttachTableScreen.h"
#include "AttachTableView.h"
#include "controllers/attachTable/AttachTableController.h"

AttachTableView::AttachTableView(QWidget *parent) : QTableView(parent) {
    this->horizontalHeader()->setStretchLastSection(true);
    this->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделяется вся строка
}

AttachTableView::~AttachTableView() {
}

void AttachTableView::init(void) {
    setupSignals();

    assemblyContextMenu();
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void AttachTableView::setupSignals(void) {
    // Сигнал чтобы показать контекстное меню по правому клику на списке записей
    connect(this, &AttachTableView::customContextMenuRequested,
            this, &AttachTableView::onCustomContextMenuRequested);

    // Сигнал чтобы открыть на просмотр/редактирование файл по двойному клику
    connect(this, &AttachTableView::doubleClicked,
            controller, &AttachTableController::onOpenAttach);
}

void AttachTableView::assemblyContextMenu(void) {
    // Конструирование меню
    contextMenu = new QMenu(this);

    // find_object() не работает, потому что при инициализации еще не вызван метод assembly() у attachTableScreen, и этому объекту не задан родитель layout
    // AttachTableScreen *screenPointer=find_object<AttachTableScreen>("attachTableScreen");
    AttachTableScreen *screenPointer = qobject_cast<AttachTableScreen *>(controller->parent());

    contextMenu->addAction(screenPointer->actionAddAttach);
    contextMenu->addAction(screenPointer->actionAddAttachFromUrl);
    contextMenu->addAction(screenPointer->actionEditFileName);
    contextMenu->addAction(screenPointer->actionDeleteAttach);
    contextMenu->addAction(screenPointer->actionSaveAsAttach);
    contextMenu->addSeparator();
    contextMenu->addAction(screenPointer->actionOpenAttach);
    contextMenu->addAction(screenPointer->actionShowAttachInfo);
}

void AttachTableView::resizeEvent(QResizeEvent *event) {
    // Первый столбец должен занимать 80% ширины при любом размере таблицы
    QRect viewRect = this->rect();
    float columnWidth = (float)viewRect.width() * 0.8;
    this->setColumnWidth(0, columnWidth);

    // Отрисовка родительского класса
    QTableView::resizeEvent(event);
}

void AttachTableView::paintEvent(QPaintEvent *event) {
    QTableView::paintEvent(event);

    // Если список приаттаченных файлов пуст
    if (model() && model()->rowCount() == 0) {
        QPainter painter(viewport());
        painter.setPen(QApplication::palette().color(QPalette::ToolTipText));
        painter.setFont(QFont("Arial", 14));
        painter.drawText(rect(), Qt::AlignCenter, tr("No attach files"));
    }
}

// Открытие контекстного меню в таблице приаттаченных файлов
void AttachTableView::onCustomContextMenuRequested(const QPoint &pos) {
    // Включение отображения меню на экране
    contextMenu->exec(viewport()->mapToGlobal(pos));
}

// Получение номера первого выделенного элемента
int AttachTableView::getFirstSelectionPos(void) {
    // Получение списка выделенных Item-элементов
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if (selectItems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectItems.at(0)).row(); // Индекс первого выделенного элемента
}
