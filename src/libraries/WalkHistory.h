#pragma once

#include <QMap>
#include <QStringList>

#include "../libraries/Singleton.h"

#define WALK_HISTORY_MAX 100

enum class WalkHistoryMode { GO_NONE, GO_PREVIOUS, GO_NEXT };

struct WALK_HISTORY_DATA {
    int cursorPosition;
    int scrollBarPosition;
};

class WalkHistory : public Singleton<WalkHistory> {
    friend class Singleton<WalkHistory>;
  public:
    void add(QString id, int cursorPosition, int scrollBarPosition, WalkHistoryMode mode = WalkHistoryMode::GO_NONE);

    QString getId() const;
    int getCursorPosition(QString id) const;
    int getScrollBarPosition(QString id) const;
    void removeHistoryData(QString id);

    void setDrop(bool flag) { dropFlag = flag; }

  protected:
    bool dropFlag = false;

    // Указатель в списке истории посещения записей, обычно указывает на последнюю в списке запись
    // но если идет движение назад-вперед по истории, он соответственно смещается
    int historyPoint = -1;

    // Указатель на точку в истории, в которой началось движение назад
    // Если пользователь отъехал назад, а потом вперед, то при достижении historyPoint этой точки,
    // указатель historyPoint сразу перемещается на последнюю запись, а leaveMarkPoint становится неиспользуемым
    // (устанавливается в значение -1)
    int leaveMarkPoint = -1;

    // Список идентификаторов посещаемых записей
    QStringList historyId;

    // Для каждого идентификатора запоминается позиция курсора и прокрутки
    QMap<QString, WALK_HISTORY_DATA> data;
};
