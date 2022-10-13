#pragma once

#include "TimerMonitoring.h"

class PeriodicCheckBase : public TimerMonitoring {
    Q_OBJECT

  public:
    void init();

  signals:
    void doUpdateDetachedWindows();

  protected:
    bool isStartEnabled() const;
    void timerEvent(QTimerEvent *event);
};
