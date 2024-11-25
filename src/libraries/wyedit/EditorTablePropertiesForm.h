#pragma once

#include <QColor>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton> // Почему-то не работает предварительное определение класса (этап линковки), приходится включать инклюдом

class EditorTablePropertiesForm : public QDialog {
    Q_OBJECT

  public:
    enum TableAlign { Left,
                      Center,
                      Right };

    EditorTablePropertiesForm();
    virtual ~EditorTablePropertiesForm() {}

    void setTableWidth(int iWidth) { spinTableWidth.setValue(iWidth); }
    int getTableWidth() { return spinTableWidth.value(); }

    void setBorderWidth(int iWidth) { spinBorderWidth.setValue(iWidth); }
    int getBorderWidth() { return spinBorderWidth.value(); }

    void setBackgroundColor(QColor iColor) { setColorForButtonBackgroundColor(iColor); }
    QColor getBackgroundColor() { return backgroundColor; }

    void setTableAlign(TableAlign iAlign);
    int getTableAlign() { return tableAlign; }

  protected slots:

    void onClickedButtonBackgroundColor();

    void onToggleButtonAlignLeft() { if (!directSetAlign) setTableAlign(Left); }
    void onToggleButtonAlignCenter() { if (!directSetAlign) setTableAlign(Center); }
    void onToggleButtonAlignRight() { if (!directSetAlign) setTableAlign(Right); }

  private:
    // Ширина таблицы
    QLabel labelTableWidth;
    QLabel labelTablePercent;
    QSpinBox spinTableWidth;

    // Толщина линий таблицы
    QLabel labelBorderWidth;
    QLabel labelBorderPix;
    QSpinBox spinBorderWidth;

    // Цвет фона таблицы
    QLabel labelBackgroundColor;
    QToolButton buttonBackgroundColor;
    QColor backgroundColor;

    QLabel labelAlign;
    QToolButton buttonAlignLeft;
    QToolButton buttonAlignCenter;
    QToolButton buttonAlignRight;
    TableAlign tableAlign;
    bool directSetAlign; // Флаг, включаемый при программной установке нажатой кнопки. Он нужен, чтобы исключить рекурсию

    // Кнопки Ok и Concel
    QDialogButtonBox buttonBox;

    void setupUi();
    void setupSignals();
    void assembly();

    void setColorForButtonBackgroundColor(QColor iColor);
};
