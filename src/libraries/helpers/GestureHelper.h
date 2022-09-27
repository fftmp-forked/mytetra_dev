#pragma once

class QAbstractItemView;

class GestureHelper
{
public:
    GestureHelper();

    static void setKineticScrollArea(QAbstractItemView *object);
};

