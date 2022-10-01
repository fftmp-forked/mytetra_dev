#pragma once

#include <QObject>

class KnowTreeModel;

class TimerMonitoring : public QObject
{
  Q_OBJECT

public:
  TimerMonitoring(void);
  virtual ~TimerMonitoring(void) {}

  virtual void init();

  void setDelay(int sec);
  void start();
  void stop();

 
protected:

  virtual bool isStartEnabled() const =0;
  virtual void timerEvent(QTimerEvent *)=0;

  bool isFirstStart;

  int delay;
  int timerId;
};


