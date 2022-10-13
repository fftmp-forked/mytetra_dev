#pragma once

#include <QCheckBox>
#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>
#include <QWidget>

#include "../../views/appConfigWindow/ConfigPage.h"

class EditorConfig;

class EditorConfigFont : public ConfigPage {
    Q_OBJECT

  public:
    EditorConfigFont(QWidget *parent = nullptr);

    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);
    int applyChanges(void);

  private slots:
    void on_monospace_applysize_state_changed(int i);

    void on_code_applysize_state_changed(int i);
    void on_code_applyindent_state_changed(int i);

    void on_code_select_color_button_click();

  private:
    QLabel *defaultFontselectLabel;
    QFontComboBox *defaultFontselect;
    QSpinBox *defaultFontsize;

    QLabel *monospaceFontselectLabel;
    QFontComboBox *monospaceFontselect;
    QCheckBox *monospaceFontsizeapply;
    QSpinBox *monospaceFontsize;

    QLabel *codeFontselectLabel;
    QFontComboBox *codeFontselect;
    QCheckBox *codeFontsizeapply;
    QSpinBox *codeFontsize;
    QCheckBox *codeIndentsizeapply;
    QSpinBox *codeIndentsize;
    QLabel *codeSelectColorLabel;
    QToolButton *codeSelectColorButton;
    QColor *codeColor;

    EditorConfig *conf;
};
