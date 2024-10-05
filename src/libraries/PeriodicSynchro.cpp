#include <QDateTime>

#include "../models/appConfig/AppConfig.h"
#include "../views/mainWindow/MainWindow.h"
#include "PeriodicSynchro.h"

bool PeriodicSynchro::isStartEnabled() const {
    return AppConfig::get().getEnablePeriodicSynchro();
}

void PeriodicSynchro::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event)

    qDebug() << "In timer PeriodicSynchro working method";

    // Если команда синхронизации пуста, нечего выполнять
    if (AppConfig::get().get_synchrocommand().trimmed().length() == 0)
        return;

    // Запуск синхронизации в скрытом режиме
    MainWindow::get().synchronization(false);
}
