#pragma once

#include <QObject>
#include <QString>

[[noreturn]] void criticalError(QString message);

void printObjectTree(const QObject *obj);

inline void setupDebug(bool enable) { qSetMessagePattern(enable ? " [${time} %{type}] %{message}" : ""); }
