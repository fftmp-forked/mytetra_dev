#pragma once

#include <QComboBox>
#include "EditorFontToolFocus.h"

#define MINIMUM_ALLOWED_FONT_SIZE 5
#define MAXIMUM_ALLOWED_FONT_SIZE 100


class EditorFontSizeComboBox : public QComboBox
{
    Q_OBJECT

public:
    EditorFontSizeComboBox(QWidget *parent=nullptr);
    virtual ~EditorFontSizeComboBox();

    void setIsProgrammChanged(bool flag);

    EditorFontToolFocus toolFocus;

protected slots:
    void onCurrentIndexChanged(int index);

protected:

    bool isProgrammChanged;

    int previousIndex;
};



