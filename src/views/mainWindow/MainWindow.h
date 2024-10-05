#pragma once

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QSessionManager>
#include <QSplitter>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTextList>
#include <QWidget>

#include "../../libraries/Singleton.h"
#include "../../models/appConfig/AppConfig.h"
#include "../record/MetaEditor.h"
#include "../recordTable/RecordTableScreen.h"
#include "../tree/TreeScreen.h"

class FindScreen;
class CommandRun;

class MainWindow : public QMainWindow, public Singleton<MainWindow> {
    Q_OBJECT
    friend class Singleton<MainWindow>;
  public:
    virtual ~MainWindow();
    void init();

    TreeScreen *treeScreen = nullptr;
    RecordTableScreen *recordTableScreen = nullptr;
    MetaEditor *editorScreen = nullptr;
    FindScreen *findScreenDisp = nullptr;
    QStatusBar *statusBar = nullptr;

    void restoreWindowGeometry();
    void restoreTreePosition();
    void restoreRecordTablePosition();
    void restoreEditorCursorPosition() { editorScreen->setCursorPosition(AppConfig::get().getEditorCursorPosition()); };
    void restoreEditorScrollBarPosition() { editorScreen->setScrollBarPosition(AppConfig::get().getEditorScrollBarPosition()); };
    void restoreFindOnBaseVisible();
    void restoreAllWindowState();

    void restoreDockableWindowsState();

    void setTreePosition(QStringList path);

    void setRecordtablePositionById(QString id) { recordTableScreen->setSelectionToId(id); };

    void synchronization(bool visible = true);

    void goWalkHistoryPrevious();
    void goWalkHistoryNext();

    void saveTextarea();
    void saveAllState();
    void reload();

  signals:

    void globalPressKey(int key);
    void globalReleaseKey(int key);

    void doUpdateDetachedWindows();

  public slots:
    void applicationExit();
    void applicationFastExit();
    void commitData(QSessionManager &manager);
    void messageHandler(QString message);
    void toolsFindInBase();
    void setupShortcuts();

  private slots:

    void showWindow();

    /// @todo
    //bool fileSave(void); // Сохранить текущую статью
    //bool fileSaveAs(void); // Сохранить текущую статью как файл

    void fileDatabasesManagement();
    void fileExportBranch();
    void fileImportBranch();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

    void toolsPreferences();

    void onExpandEditArea(bool flag);

    void onClickHelpAboutMyTetra();
    void onClickHelpAboutQt() { QMessageBox(this).aboutQt(this); };
    void onClickFocusTree() { treeScreen->setFocusToBaseWidget(); };
    void onClickFocusNoteTable() { recordTableScreen->setFocusToBaseWidget(); };
    void onClickFocusEditor() { editorScreen->setFocusToBaseWidget(); };

    void onSynchroCommandFinishWork();

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

  private:
    void setupUI();
    void setupSignals();
    void assembly();

    void initFileMenu();
    void initToolsMenu();
    void initHelpMenu();
    void initHiddenActions();

    void createTrayIcon();
    void setIcon();

    void saveWindowGeometry();
    void saveTreePosition();
    void saveRecordTablePosition() { AppConfig::get().set_recordtable_selected_record_id(recordTableScreen->getFirstSelectionId()); };
    void saveEditorCursorPosition() { AppConfig::get().setEditorCursorPosition(editorScreen->getCursorPosition()); };
    void saveEditorScrollBarPosition() { AppConfig::get().setEditorScrollBarPosition(editorScreen->getScrollBarPosition()); };

    void reloadSaveStage();
    void reloadLoadStage(bool isLongTimeReload);

    QAction *actionFileMenuPrint;
    QAction *actionFileMenuExportPdf;
    QAction *actionFileMenuQuit;

    QAction *actionToolsMenuFindInBase;

    QAction *actionFocusTree;
    QAction *actionFocusNoteTable;
    QAction *actionFocusEditor;

    QSystemTrayIcon *trayIcon;

    QSplitter *vSplitter;
    QSplitter *hSplitter;
    QSplitter *findSplitter;

    CommandRun *synchroCommandRun = nullptr;

  protected:
    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *o, QEvent *e); // Отслеживание прочих событий

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void goWalkHistory();

    bool enableRealClose;
    int exitCounter = 0;
};
