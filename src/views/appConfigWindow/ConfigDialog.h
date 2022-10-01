#pragma once

#include <QDialog>
#include <QListWidget>

class QDialogButtonBox;
class QStackedWidget;


class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog(QWidget * parent = nullptr);
    virtual ~ConfigDialog() {};

    QListWidgetItem *addWidget(QWidget *inswidget, QString name);

    void updateListWidth(void);
    void externalChangePage(QListWidgetItem *item) {contentsWidget->setCurrentItem(item);}
    void setMenuListVisible(bool visible) {contentsWidget->setVisible(visible);}

private slots:    
    void applyChanges(void);
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    QListWidgetItem *createItems(QString name);


    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
    QDialogButtonBox *confirmButtons;
};

