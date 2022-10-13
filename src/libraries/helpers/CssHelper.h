#pragma once

#include <QtGlobal>

class CssHelper {
  public:
    CssHelper(){};
    static void setCssStyle(QString dir);
    static void createStyleSheetFile(QString dir);

  private:
    static QString replaceCssMetaIconSize(QString styleText);
    static qreal getCalculateIconSizePx(void);
};
