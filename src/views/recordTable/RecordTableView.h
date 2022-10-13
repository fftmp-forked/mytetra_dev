#pragma once

#include <QEvent>
#include <QItemSelection>
#include <QMenu>
#include <QTableView>
#include <QWidget>

class ClipboardRecords;
class RecordTableController;

/// @brief Виджет, отображащий список записей в ветке
class RecordTableView : public QTableView {
    Q_OBJECT

  public:
    RecordTableView(QWidget *parent = nullptr);
    virtual ~RecordTableView(){};

    void setController(RecordTableController *pController) { controller = pController; };

    void init(void);

    void restoreHeaderState(void);
    void restoreColumnWidth(void);

    int getFirstSelectionPos(void);
    QString getFirstSelectionId(void);

    void setSelectionToPos(int pos);

    QModelIndex getFirstSelectionProxyIndex(void);
    QModelIndex getFirstSelectionSourceIndex(void);

    bool isSelectedSetToTop(void);
    bool isSelectedSetToBottom(void);

    ClipboardRecords *getSelectedRecords(void);

    void moveCursorToNewRecord(int mode, int pos);

    void switchSelectionMode(void);

    void updateRow(int rowNum);

  public slots:

    // Открытие контекстного меню
    void onCustomContextMenuRequested(const QPoint &mousePos);

    // Слот, срабатывающий после перетаскивания колонки
    void onSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void onSectionResized(int logicalIndex, int oldSize, int newSize);

    // Вызов действий из контекстного меню или из контроллера для редактирования инфополей записи
    void editFieldContext(void);

  protected slots:

    // Реакия на сдвиг засветки клавишами или мышкой
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    // Слот, который автоматически срабатыват при изменении selection в списке
    // Этот слот нигде не надо прописывать через connect(), так как он
    // является переопределенным, так как его тип virtual protected slot
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void onPressToRecord(const QModelIndex &index);

    void onDropEventHandleCatch() { isDragHappeningNow = false; } // Слот вызывается из KnowTreeView если произошол Drop

  protected:
    QMenu *contextMenu;
    RecordTableController *controller;

    void setupSignals(void);

    void assemblyContextMenu(void);

    void editField(int pos, QString name, QString author, QString url, QString tags);

    void deleteRecords(void);

    // Реакция на выбор записи мышкой или клавишами
    void clickToRecord(const QModelIndex &index);

    QPoint mouseStartPos;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void saveColumnWidth(void);

    bool enableMoveSection;

  private:
    void startDrag(Qt::DropActions supportedActions);
    QModelIndex startDragIndex;
    bool isDragHappeningNow = false;
};
