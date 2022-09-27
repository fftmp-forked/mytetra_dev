#pragma once

class QString;


class HtmlHelper
{
public:
    HtmlHelper();

    static QString htmlSpecialChars(QString line);
    static QString htmlSpecialCharsDecode(QString line);

};

