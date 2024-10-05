#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

class RecordTableController;

/// @brief Виджет, который отображает список записей в ветке c кнопками управления
class RecordTableScreen : public QWidget {
    Q_OBJECT

  public:
    RecordTableScreen(QWidget *parent = nullptr);
    virtual ~RecordTableScreen() {}

    int getFirstSelectionPos(void);
    QString getFirstSelectionId(void);
    void setSelectionToPos(int pos);
    void setSelectionToId(QString id);

    void setTreePath(QString path);
    QString getTreePath(void);

    void disableAllActions(void);

    void setFocusToBaseWidget();

    // Действия, используемые как на тулбаре, так и в контекстном меню списка записей
    QAction *actionAddNewToEnd;  // Добавление записи
    QAction *actionAddNewBefore; // Добавление записи до
    QAction *actionAddNewAfter;  // Добавление записи после
    QAction *actionEditField;    // Редактирование свойств записи
    QAction *actionBlock;        // Блокировка записи
    QAction *actionDelete;       // Удаление записи
    QAction *actionCut;          // Удаление записи с копированием в буфер обмена
    QAction *actionCopy;         // Копирование записи (записей) в буфер обмена
    QAction *actionPaste;        // Вставка записи из буфера обмена
    QAction *actionSettings;     // Настройка внешнего вида таблицы конечных записей (горячая кнопка не требуется)
    QAction *actionFindInBase;   // Поиск по базе (клик связывается с действием в MainWindow)
    QAction *actionSort;         // Действия по сортировке (горячая кнопка не требуется)
    QAction *actionPrint;        // Кнопка вызова печати таблицы конечных записей
    QAction *actionCopyRecordReference; // Кнопка копирования ссылки на запись
    QAction *actionSwitchSelectionMode; // Кнопка переключения режима одинарного выбора и мультивыбора (горячая кнопка не требуется)

    // Действие, которое может быть вызвано из MainWindows
    QAction *actionSynchro;

  public slots:

    // Обновление состояния экранных элементов согласно состоянию записи и положении зиписи в таблице
    void toolsUpdate(void);

    void onSynchroCommandFinishWork(void);

    void setupShortcuts(void);

  private slots:

    void onSynchroClick(void);
    void onWalkHistoryPreviousClick(void);
    void onWalkHistoryNextClick(void);
    void onCopyRecordReference(void);

  private:
    QToolBar *toolsLine;
    QToolBar *extraToolsLine;

    QLabel *treePathLabel;
    QString treePath;

    RecordTableController *recordTableController;

    QHBoxLayout *recordTableToolsLayout;
    QVBoxLayout *recordTableScreenLayout;

    QAction *actionMoveUp; // Перемещение записи вверх
    QAction *actionMoveDn; // Перемещение записи вниз

    QAction *actionWalkHistoryPrevious; // Перемещение по истории посещаемых записей назад
    QAction *actionWalkHistoryNext;     // Перемещение по истории посещаемых записей вперед

    void setupUI(void);
    void setupSignals(void);
    void setupActions(void);
    void assembly(void);

    void toolsWidgetsUpdate();
    void editorModesUpdate();
};
