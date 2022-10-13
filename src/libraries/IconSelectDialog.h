#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>

class IconSelectDialog : public QDialog {
    Q_OBJECT

  public:
    IconSelectDialog();
    ~IconSelectDialog(){};

    enum resultCode { RemoveIconCode = 2 };

    void setPath(QString iPath);
    void setDefaultSection(QString iSectionName);
    QString getCurrentSection() const { return sectionComboBox.itemText(currentSectionIndex); };

    QString getSelectFileName(void) const { return currentFileName; };

    int exec();

    // Проверка наличия коллекции иконок
    static void iconsCollectionCheck();

  protected slots:

    void onRemoveIconClick();
    void onOkClick();
    void onCancelClick();
    void onSectionCurrentIndexChanged(int idx);
    void updateIcons();

    void onIconItemSelectionChanged();

  protected:
    void setupUI();
    void setupSignals();
    void assembly();

    QLabel sectionLabel;
    QComboBox sectionComboBox;
    QListWidget iconList;
    QPushButton buttonRemoveIcon;
    QPushButton buttonOk;
    QPushButton buttonCancel;
    QProgressBar progressBar;

    QString path;
    int defaultSectionIndex = -1;
    int currentSectionIndex;

    bool enableIconUpdate;

    QString currentFileName;
};
