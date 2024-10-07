#pragma once

#include "EditorToolBar.h"
#include <QWidget>
#include <qtextformat.h>


class Editor;
class EditorTextArea;
class QTextCharFormat;


/// @brief Виджет для отрисовки кнопок форматирования текста
/// Управляет поведением и внешним видом кнопок, и действиями, совершаемыми при кликах на кнопки
class EditorToolBarAssistant : public EditorToolBar {
    Q_OBJECT

  public:
    EditorToolBarAssistant(QWidget *parent, EditorTextArea *iTextArea, QStringList iDisableToolList);

    void reload();

    // Метод, переключающий состояние видимости развернутой/свернутой панели инструментов
    // Если вызывается без параметра, метод сам переключает
    // Параметр 1 - включить полную видимость
    // Параметр -1 - выключить полную видимость
    void switchExpandToolsLines(int flag = 0);

    void updateToActualFormat();
    void setOutlineButtonHiglight(int button, bool active);
    void setShowFormattingButtonHiglight(bool active);
    bool isKeyForToolLineUpdate(QKeyEvent *event);

    bool getFlagSetFontParametersEnabled() const {return flagSetFontParametersEnabled;}

    int getFontSizeByNum(int n) const {return fontSize->itemData(n).toInt();}

    enum {
        BT_BOLD,
        BT_ITALIC,
        BT_UNDERLINE,
        BT_STRIKEOUT,
        BT_SUPERSCRIPT,
        BT_SUBSCRIPT,
        BT_ALIGN_LEFT,
        BT_ALIGN_CENTER,
        BT_ALIGN_RIGHT,
        BT_ALIGN_WIDTH
    };

  signals:

    void updateIndentSliderGeometry();
    void changeIndentSliderVisible(bool);

  public slots:

    void onExpandToolsLinesClicked() { switchExpandToolsLines(); };
    void onUpdateAlignButtonHiglight(bool activate);
    void onUpdateOutlineButtonHiglight();
    void onChangeFontselectOnDisplay(QString fontName);
    void onChangeFontsizeOnDisplay(int n);
    void onChangeFontFamily(QString fontFamily) {currentFontFamily = fontFamily; };
    void onChangeFontPointSize(int n) { currentFontSize = n; };
    void onChangeFontcolor(const QColor &color);
    void onChangeIconFontColor(const QTextCharFormat &format) { onChangeFontcolor(format.foreground().color()); };
    void onChangeBackgroundColor(const QColor &color);
    void onChangeIconBackgroundColor(const QTextCharFormat &format) { onChangeBackgroundColor(format.background().color()); };
    void onCursorPositionChanged(); // Слот, вызываемый при изменение позиции курсора

  protected:
    void init(const QStringList &iDisableToolList);
    void initToolsLists(const QStringList &iDisableToolList);
    void setupSignals();

    QPixmap drawIconOverColor(const QColor &fillColor, const QIcon &icon) const;

    Editor *editor;           // Указатель на объект редактора, с которым работает эта панель кнопок
    EditorTextArea *textArea; // Указатель на объект области редактирования, с которой работает эта панель кнопок

    QString currentFontFamily;
    int currentFontSize;

    bool flagSetFontParametersEnabled; // Флаг разрешения/запрета срабатывания слотов установки параметров шрифта
};
