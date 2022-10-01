#pragma once

#include <QString>


QString getUniqueId(void);
inline QString getUniqueImageName(void) { return "image" + getUniqueId() + ".png"; }

