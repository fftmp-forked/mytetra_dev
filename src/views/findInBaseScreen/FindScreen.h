#pragma once

#include <QComboBox>
#include <QMap>
#include <QWidget>

class QLineEdit;
class QPushButton;
class QToolButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class QCheckBox;
class QProgressDialog;

class KnowTreeModel;
class TreeItem;

class FindTableWidget;

/// @brief Виджет поиска по базе
class FindScreen : public QWidget {
    Q_OBJECT

  public:
    FindScreen(QWidget *parent = nullptr);
    virtual ~FindScreen(void) {}

  public slots:

    void widgetShow(void);
    void widgetHide(void);
    void findClicked(void);
    void setFindText(QString text);

  private slots:

    void enableFindButton(const QString &text);
    void toolsExpandClicked(void);

    void changedWordRegard(int pos);
    void changedHowExtract(int pos);
    void changedTreeSearchArea(int pos);

    void changedFindInName(int state) {changedFindInField("name", state);}
    void changedFindInAuthor(int state) {changedFindInField("author", state);}
    void changedFindInUrl(int state) {changedFindInField("url", state);}
    void changedFindInTags(int state) {changedFindInField("tags", state);}
    void changedFindInText(int state) {changedFindInField("text", state);}
    void changedFindInNameItem(int state) {changedFindInField("nameItem", state);}

  signals:

    // Сигнал вырабатывается, когда обнаружено что в слоте setFindText() был изменен текст для поиска
    void textChangedFromAnother(const QString &);

    void findClickedAfterAnotherTextChanged(void);

  private:
    QHBoxLayout *toolsAreaFindTextAndButton;
    QLineEdit *findText;
    QPushButton *findStartButton;
    QToolButton *toolsExpand;

    QVBoxLayout *toolsAreaCloseButton;
    QToolButton *closeButton;

    QHBoxLayout *toolsAreaComboOption;
    QComboBox *wordRegard;
    QComboBox *howExtract;
    QComboBox *treeSearchArea;

    QHBoxLayout *whereFindLine;
    QLabel *whereFindLabel;
    QCheckBox *findInName;
    QCheckBox *findInAuthor;
    QCheckBox *findInUrl;
    QCheckBox *findInTags;
    QCheckBox *findInText;
    QCheckBox *findInNameItem;

    QHBoxLayout *toolsLine;
    QGridLayout *toolsGrid;

    QVBoxLayout *centralDesktopLayout;

    FindTableWidget *findTable;

    QProgressDialog *progress;

    void setupFindTextAndButton(void);
    void assemblyFindTextAndButton(void);

    void setupComboOption(void);
    void assemblyComboOption(void);

    void setupCloseButton(void);
    void assemblyCloseButton(void);

    void setupWhereFindLine(void);
    void assemblyWhereFindLine(void);

    void setupUI(void);
    void assembly(void);

    void setupSignals(void);

    void changedFindInField(QString fieldname, int state);

    void findStart(void);
    void findRecurse(TreeItem *curritem);
    bool findInTextProcess(const QString &text);

    void switchToolsExpand(bool flag);

    QStringList textDelimiterDecompose(QString text);

    QStringList scanFields;

    // Список слов, которые нужно искать
    QStringList searchWordList;

    int totalProgressCounter;

    bool cancelFlag;
};
