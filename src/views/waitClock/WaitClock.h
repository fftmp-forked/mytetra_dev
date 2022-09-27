#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QTimer>
#include <QLabel>
#include <QPixmap>


class WaitClock : public QWidget
{
 Q_OBJECT

public:
 WaitClock(QWidget *parent=nullptr);
 ~WaitClock();

private slots:

 void iconUpdate();

private:

 QHBoxLayout *centralLayout;
 
 QLabel iconLabel;

 QPixmap kadr_00;
 QPixmap kadr_01;
 QPixmap kadr_02;

 int currentIconNum;

 QTimer timer;

};

