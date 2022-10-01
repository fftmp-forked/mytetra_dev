#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QString>
#include <QLabel>

class RecordTableController;


/// @brief Виджет, который отображает список записей в ветке c кнопками управления
class RecordTableScreen : public QWidget
{
 Q_OBJECT

public:
 RecordTableScreen(QWidget *parent=nullptr);
 virtual ~RecordTableScreen(){}

 int     getFirstSelectionPos(void);
 QString getFirstSelectionId(void);
 void    setSelectionToPos(int pos);
 void    setSelectionToId(QString id);

 void setTreePath(QString path);
 QString getTreePath(void);

 void disableAllActions(void);

 void setFocusToBaseWidget();

 // Действия, используемые как на тулбаре, так и в контекстном меню списка записей
 QAction *actionAddNewToEnd;
 QAction *actionAddNewBefore;
 QAction *actionAddNewAfter;
 QAction *actionEditField;
 QAction *actionBlock;
 QAction *actionDelete;
 QAction *actionCut;
 QAction *actionCopy;
 QAction *actionPaste;
 QAction *actionSettings;
 QAction *actionBack;
 QAction *actionFindInBase;
 QAction *actionSort;
 QAction *actionPrint;
 QAction *actionCopyRecordReference;
 QAction *actionSwitchSelectionMode;

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

 QAction *actionMoveUp;
 QAction *actionMoveDn;
 QAction *actionWalkHistoryPrevious;
 QAction *actionWalkHistoryNext;
  
 void setupUI(void);
 void setupSignals(void);
 void setupActions(void);
 void assembly(void);

 void toolsWidgetsUpdate();
 void editorModesUpdate();

};

