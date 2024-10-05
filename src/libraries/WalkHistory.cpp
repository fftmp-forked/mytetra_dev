#include "WalkHistory.h"

#include <QtDebug>


void WalkHistory::add(QString id, int cursorPosition, int scrollBarPosition, WalkHistoryMode mode) {
    if (id.isEmpty() == 0)
        return;

    if (dropFlag)
        return;

    data[id].cursorPosition = cursorPosition;
    data[id].scrollBarPosition = scrollBarPosition;

    // Если история слишком большая
    if (historyId.length() > WALK_HISTORY_MAX) {
        qDebug() << "WalkHistory::add() : history too big, trim";

        historyId.removeFirst();

        if (historyPoint > 0)
            historyPoint -= 1;

        if (leaveMarkPoint > 0)
            leaveMarkPoint -= 1;
    }

    // Если происходит просто запоминание нового ID без движения по истории
    if (mode == WalkHistoryMode::GO_NONE) {
        qDebug() << "WalkHistory::add() : mode GO_NONE";

        // Повторяющийся идентификатор не запоминается
        if (historyId.size() > 0 && historyId.last() == id) {
            qDebug() << "WalkHistory::add() : Repeate ID, dont save";
            return;
        }

        // Идентификатор добавляется в историю
        historyId << id;
        historyPoint = historyId.length() - 1;

        // Без этой команды работало правильно, но она вроде должна быть в этом месте
        leaveMarkPoint = -1;

        return;
    } // Закончилось условие что происходит запоминание без движения по истории

    ///////////////////////////////////////////////////////////////////////////////
    // В этом месте остались режимы WALK_HISTORY_GO_PREVIOUS и WALK_HISTORY_GO_NEXT
    ///////////////////////////////////////////////////////////////////////////////

    // Если история была пуста
    if (historyPoint == -1) {
        // Идентификатор просто добавляется в историю
        historyId << id;
        historyPoint = 0;

        qDebug() << "WalkHistory::add() : history is empty, only add ID";
        return;
    }

    // Если добавляемый идентификатор не равен последнему в истории, его нужно добавить (даже если идет движение назад или вперед по истории)
    if (id != historyId.last()) {
        qDebug() << "WalkHistory::add() : new ID not equal new ID, adding";

        // Если указатель находится в конце истории
        if (historyPoint == (historyId.length() - 1))
            historyPoint = historyId.length(); // Указатель перемещается на ячейку, которая добавится следующей командой

        // Идентификатор добавляется в историю
        historyId << id;

        qDebug() << "WalkHistory add id: " << id;
        qDebug() << "WalkHistory pointer: " << historyPoint;
        qDebug() << "WalkHistory lenght: " << historyId.length();
    }

    // Если происходит запоминание с движением назад по истории
    if (mode == WalkHistoryMode:: GO_PREVIOUS) {
        qDebug() << "WalkHistory::add() : mode GO_PREVIOUS";

        // Если указатель находится в конце истории
        if (historyPoint == (historyId.length() - 1)) {
            qDebug() << "WalkHistory::add() : pointer on end";

            // Запоминается момент, когда началось движение назад по истории
            leaveMarkPoint = historyPoint;

            historyPoint = historyId.length() - 1;
        }

        // Происходит перемещение указателя назад
        if (historyPoint > 0)
            historyPoint--;
    } // Закончилось условие что происходит запоминание с движением назад по истории

    // Если происходит запоминание с движением вперед по истории
    if (mode == WalkHistoryMode::GO_NEXT) {
        qDebug() << "WalkHistory::add() : mode GO_NEXT";

        // Если указатель находится в конце истории
        if (historyPoint == (historyId.length() - 1)) {
            qDebug() << "WalkHistory::add() : pointer on end, no move to next";
            return;
        } else {
            // Иначе указатель находится где-то в середине истории
            qDebug() << "WalkHistory::add() : pointer on middle history";

            // Происходит перемещение указателя вперед
            if (historyPoint < (historyId.length() - 1))
                historyPoint++;

            // Если указатель при движении вперед достиг точки, в которой начинал движение назад
            if (historyPoint > leaveMarkPoint) {
                historyPoint = historyId.length() - 1; // Прыжок на самую последнюю запись
                leaveMarkPoint = -1;                   // Точка начала движения назад становится неактивной
            }
        }

    } // Закончилось условие что происходит запоминание с движением вперед по истории
}

QString WalkHistory::getId() const {
    if (historyPoint >= 0 && historyPoint <= (historyId.length() - 1))
        return historyId[historyPoint];
    else
        return QString();
}

int WalkHistory::getCursorPosition(QString id) const {
    if (data.contains(id))
        return data[id].cursorPosition;
    else
        return 0;
}

int WalkHistory::getScrollBarPosition(QString id) const {
    if (data.contains(id))
        return data[id].scrollBarPosition;
    else
        return 0;
}

void WalkHistory::removeHistoryData(QString id) {
    if (data.contains(id))
        data.remove(id);
}
