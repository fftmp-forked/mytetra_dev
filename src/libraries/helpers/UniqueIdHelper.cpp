#include <ctime>

#include "UniqueIdHelper.h"

/// @return уникальный идентификатор из 10 цифр количества секунд с эпохи UNIX и 10 случайных символов 0-9 a-z
QString getUniqueId(void) {

    const char *const symbols = "0123456789abcdefghijklmnopqrstuvwxyz";

    QString line = QString::number(time(nullptr), 10);
    line = line.rightJustified(10, '0');

    for (int i = 0; i < 10; ++i)
        line += symbols[rand() % strlen(symbols)];

    return line;
}
