#pragma once

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "HotKeyGrabber.h"
#include "controllers/shortcutSettings/ShortcutSettingsController.h"

class ShortcutSettingsScreen : public QDialog {
    Q_OBJECT
  public:
    ShortcutSettingsScreen(QWidget *parent = nullptr);
    ~ShortcutSettingsScreen() {}

  protected slots:

    void onShortcutSelect(const QModelIndex &index);
    void onShortcutKeysChange(const QString &text);
    void onGrabShortcutClick();
    void onGrabShortcutEditingFinished();
    void onInfoClick();
    void onResetShortcutToDefaultClick();
    void onResetAllShortcutsToDefaultClick();

  protected:
    ShortcutSettingsController *shortcutSettingsController;

    QPushButton *buttonGrabShortcut;
    QPushButton *buttonResetShortcutToDefault;
    QPushButton *buttonResetAllShortcutsToDefault;

    QDialogButtonBox *dialogButtonBox; // Кнопки Ok и Cancel

    QGroupBox *shortcutBox;
    QLabel *commandLabel;
    QLabel *commandValueLabel;
    QLabel *desctiptionLabel;
    QLabel *desctiptionValueLabel;
    QLabel *shortcutLabel;
    QLineEdit *shortcutValueLineEdit;
    QHBoxLayout *shortcutLineLayout; // Слой с полем сочетания клавиш, кнопками Grab и Reset to default
    QToolButton *buttonInfo;
    QGridLayout *shortcutLayout; // Слой внутри объединяющего прямоугольника настройки шортката

    QVBoxLayout *screenLayout;

    HotKeyGrabber hotKeyGrabber;

    // Данные о текущем выбранном шорткате
    // Раздел, команда, описание, клавиши
    ShortcutSettingsController::ShortcutData shortcutData;

    void setupUI(void);
    void setupSignals(void);
    void assembly(void);
};
