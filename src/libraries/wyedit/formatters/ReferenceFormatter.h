#pragma once

#include "Formatter.h"


/// @brief Класс форматирования ссылок
class ReferenceFormatter : public Formatter
{
    Q_OBJECT

public:
    ReferenceFormatter();

signals:

public slots:

    void onReferenceClicked(void);
    void onContextMenuGotoReference(void);
    void onClickedGotoReference(QString href);
    void onTextChanged(void);
protected:

    bool isHrefInternal(QString href);
    QString getIdFromInternalHref(QString href);
};


