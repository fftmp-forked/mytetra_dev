#pragma once

#include "TimerMonitoring.h"


class PeriodicSynchro : public TimerMonitoring
{
  Q_OBJECT

protected:

  bool isStartEnabled();
  void timerEvent(QTimerEvent *event);

};


