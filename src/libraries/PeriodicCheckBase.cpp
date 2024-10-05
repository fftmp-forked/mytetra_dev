#include <QDateTime>
#include <QFileInfo>

#include "../models/appConfig/AppConfig.h"
#include "../models/tree/KnowTreeModel.h"
#include "../views/mainWindow/MainWindow.h"
#include "../views/tree/KnowTreeView.h"
#include "PeriodicCheckBase.h"
#include "helpers/MessageHelper.h"
#include "helpers/ObjectHelper.h"
#include "wyedit/EditorShowTextDispatcher.h"

void PeriodicCheckBase::init() {
    TimerMonitoring::init();

    connect(this, &PeriodicCheckBase::doUpdateDetachedWindows,
            EditorShowTextDispatcher::instance(), &EditorShowTextDispatcher::closeWindowForNonExistentRecords,
            Qt::QueuedConnection);
}

bool PeriodicCheckBase::isStartEnabled() const {
    return AppConfig::get().getEnablePeriodicCheckBase();
}

/// @brief  Действия, происходящие по таймеру
void PeriodicCheckBase::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event)

    auto knowTreeView = find_object<KnowTreeView>("knowTreeView");
    auto knowTreeModel = qobject_cast<KnowTreeModel *>(knowTreeView->model());

    auto lastAccess = knowTreeModel->getLastAccess();
    if (lastAccess.isNull())
        return;

    // Время последнего изменения файла дерева
    auto modifyDateTime = QFileInfo(AppConfig::get().get_tetradir() + "/mytetra.xml").lastModified();

    if (modifyDateTime > lastAccess) {
        MainWindow::get().reload();

        emit doUpdateDetachedWindows();

        // Если разрешена выдача сообщения о том, что база данных была изменена
        if (AppConfig::get().getEnablePeriodicCheckMessage())
            showMessageBox(tr("The database was changed by external application or services.\nMyTetra reload the database tree to keep data consistency."));
    }
}
