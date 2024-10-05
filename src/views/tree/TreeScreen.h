#pragma once

#include <QDateTime>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

class KnowTreeModel;
class KnowTreeView;
class ClipboardBranch;

class TreeScreen : public QWidget {
    Q_OBJECT

  public:
    TreeScreen(QWidget *parent = nullptr);

    KnowTreeModel *knowTreeModel;

    void saveKnowTree();
    bool reloadKnowTree();

    void updateSelectedBranch();

    int getFirstSelectedItemIndex();
    QModelIndex getCurrentItemIndex();

    QItemSelectionModel *getSelectionModel();

    void exportBranchToDirectory(QString exportDir);
    void importBranchFromDirectory(QString importDir);

    // Установка курсора на указанный элемент
    void setCursorToIndex(QModelIndex index);
    void setCursorToId(QString nodeId);

    void updateBranchOnScreen(const QModelIndex &index);

    void setFocusToBaseWidget();

  signals:

    void treeScreenFindInBaseClicked();

  public slots:

    void setupShortcuts();

  private slots:

    void expandAllSubbranch();
    void collapseAllSubbranch();
    void expandOrCollapseRecurse(QModelIndex modelIndex, bool mode);
    void insSubbranch();
    void insBranch();
    void editBranch();
    void setIcon();

    void delBranch(QString mode = "delete");

    void moveUpBranch();
    void moveDownBranch();
    void cutBranch();
    bool copyBranch();
    void pasteBranch();
    void pasteSubbranch();

    // Действия при клике на ветку дерева
    void onKnowtreeClicked(const QModelIndex &index);

    // Открытие контекстного меню
    void onCustomContextMenuRequested(const QPoint &pos);

  private:
    QMap<QString, QAction *> actionList;

    QToolBar *toolsLine;

    KnowTreeView *knowTreeView;

    QDateTime lastKnowTreeModifyDateTime;
    qint64 lastKnowTreeSize;

    void setupUI();
    void setupModels();
    void setupSignals();
    void setupActions();
    void assembly();

    void moveUpDownBranch(int direction);
    bool moveCheckEnable();

    void insBranchSmart(bool is_branch);
    void insBranchProcess(QModelIndex index, QString name, bool is_branch);

    void addBranchToClipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root);

    void pasteBranchSmart(bool is_branch);

    void treeEmptyControl();

    void updateLastKnowTreeData(QFileInfo fileInfo, bool fileInfoValid);
};
