#include <QAction>
#include <QToolBar>
#include <QToolButton>

#include "ActionHelper.h"

void insertActionAsButton(QToolBar *tools_line, QAction *action, bool isVisible) {
    // Действие добавляется в виде кнопки
    tools_line->addAction(action);

    if (!isVisible) {
        // Выясняется кнопка, которая была создана и обрабатвает данное действие
        auto currentButton = tools_line->widgetForAction(action);
        currentButton->setFixedHeight(0);
        currentButton->setFixedWidth(0);
    }
}
