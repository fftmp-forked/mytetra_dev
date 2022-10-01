#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QTextList>
#include <QCloseEvent>
#include <QSessionManager>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QStatusBar>

class TreeScreen;
class MetaEditor;
class RecordTableScreen;
class FindScreen;
class CommandRun;


class MainWindow : public QMainWindow
{
Q_OBJECT

public:
 MainWindow(): QMainWindow() {}
 virtual ~MainWindow() {saveAllState();}
 void init();

 TreeScreen *treeScreen=nullptr;
 RecordTableScreen *recordTableScreen=nullptr;
 MetaEditor *editorScreen=nullptr;
 FindScreen *findScreenDisp=nullptr;
 QStatusBar *statusBar=nullptr;

 void restoreWindowGeometry(void);
 void restoreTreePosition(void);
 void restoreRecordTablePosition(void);
 void restoreEditorCursorPosition(void);
 void restoreEditorScrollBarPosition(void);
 void restoreFindOnBaseVisible(void);
 void restoreAllWindowState(void);

 void restoreDockableWindowsState(void);

 void setTreePosition(QStringList path);

 void setRecordtablePositionById(QString id);

 void synchronization(bool visible=true);

 void goWalkHistoryPrevious(void);
 void goWalkHistoryNext(void);

 void saveTextarea(void);

 void saveAllState(void);

 void reload(void);

signals:

 void globalPressKey(int key);
 void globalReleaseKey(int key);

 void doUpdateDetachedWindows();

public slots:
 void applicationExit(void);
 void applicationFastExit(void);
 void commitData(QSessionManager& manager);
 void messageHandler(QString message);

 void toolsFindInBase(void);

 void setupShortcuts(void);

private slots:

 void showWindow();

 bool fileSave(void);
 bool fileSaveAs(void);

 void fileDatabasesManagement(void);
 void fileExportBranch(void);
 void fileImportBranch(void);

 void filePrint(void);
 void filePrintPreview(void);
 void filePrintPdf(void);
 
 void toolsPreferences(void);

 void onExpandEditArea(bool flag);

 void onClickHelpAboutMyTetra(void);
 void onClickHelpAboutQt(void);
 void onClickHelpTechnicalInfo(void);

 void onClickFocusTree(void);
 void onClickFocusNoteTable(void);
 void onClickFocusEditor(void);

 void runDirectPreferences(QAction *action);

 void onSynchroCommandFinishWork(void);

 void iconActivated(QSystemTrayIcon::ActivationReason reason);

 void onFocusChanged(QWidget *, QWidget *);

private:

 void setupUI(void);
 void setupSignals(void);
 void assembly(void);

 void initFileMenu(void);
 void initToolsMenu(void);
 void initPreferencesMenu(QMenu *menu);
 void initHelpMenu(void);
 void initHiddenActions(void);

 void initRecordTableActions(void);

 void setupIconActions(void);
 void createTrayIcon(void);
 void setIcon(void);

 void saveWindowGeometry(void);
 void saveTreePosition(void);
 void saveRecordTablePosition(void);
 void saveEditorCursorPosition(void);
 void saveEditorScrollBarPosition(void);

 void reloadSaveStage(void);
 void reloadLoadStage(bool isLongTimeReload);


 QAction *actionFileMenuDatabasesManagement;
 QAction *actionFileMenuExportTreeItem;
 QAction *actionFileMenuImportTreeItem;
 QAction *actionFileMenuPrint;
 QAction *actionFileMenuPrintPreview;
 QAction *actionFileMenuExportPdf;
 QAction *actionFileMenuQuit;

 QAction *actionToolsMenuFindInBase;
 QAction *actionToolsMenuPreferences; // Вызов окна настроек, используется в десктопе

 // Напрямую вызываемые настройки, используются в мобильном интерфейсе
 QAction *actionDirectPreferencesMain       =nullptr;
 QAction *actionDirectPreferencesAppearance =nullptr;
 QAction *actionDirectPreferencesSyncro     =nullptr;
 QAction *actionDirectPreferencesRecordTable=nullptr;
 QAction *actionDirectPreferencesAttach     =nullptr;
 QAction *actionDirectPreferencesKeyboard   =nullptr;
 QAction *actionDirectPreferencesHistory    =nullptr;
 QAction *actionDirectPreferencesMisc       =nullptr;

 QAction *actionHelpMenuAboutMyTetra;
 QAction *actionHelpMenuAboutQt;
 QAction *actionHelpMenuTechnicalInfo;

 QAction *actionTrayRestore;
 QAction *actionTrayMaximize;
 QAction *actionTrayMinimize;
 QAction *actionTrayQuit;

 QAction *actionFocusTree;
 QAction *actionFocusNoteTable;
 QAction *actionFocusEditor;

 QSystemTrayIcon *trayIcon;
 QMenu           *trayIconMenu;

 QSplitter *vSplitter;
 QSplitter *hSplitter;
 QSplitter *findSplitter;

 CommandRun *synchroCommandRun=nullptr;

 
protected:
 
 void closeEvent(QCloseEvent *event);

 bool eventFilter( QObject * o, QEvent * e ); // Отслеживание прочих событий

 void keyPressEvent(QKeyEvent *event);
 void keyReleaseEvent(QKeyEvent *event);

 void goWalkHistory(void);

 bool enableRealClose;
 int exitCounter=0;
 
};
