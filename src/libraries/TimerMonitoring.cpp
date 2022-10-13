#include <QtDebug>

#include "TimerMonitoring.h"

TimerMonitoring::TimerMonitoring(void) {
    // По сути, это синглтон. Синглтон назначает себе имя сам
    setObjectName("timerMonitoring");

    isFirstStart = true;
}

/// @details Инициализация класса должна происходить после инициализации KnowTreeView
void TimerMonitoring::init() {
    setDelay(0);
    timerId = 0;
}

void TimerMonitoring::setDelay(int sec) {
    delay = sec;

    // Если таймер работает, он перезапускается с новой задержкой
    if (timerId != 0) {
        stop();
        start();
    }
}

void TimerMonitoring::start() {
    if (!isStartEnabled())
        return;
    if (isFirstStart) {
        timerEvent(NULL);
        isFirstStart = false;
    }

    // Установка автоматического повтора действий
    timerId = startTimer(delay * 1000); // Периодичность таймера должна задаваться в миллисекундах
    qDebug() << "Start timer with delay: " << delay << " ID: " << timerId;
}

void TimerMonitoring::stop() {
    killTimer(timerId);
    timerId = 0;
}
