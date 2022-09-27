#pragma once

#include <QString>

class TraceLogger
{
public:
  TraceLogger(const char* iFileName,
              const char* iFuncName,
              int iLineNumber);
  ~TraceLogger();

private:
    const char* fileName;
    const char* funcName;

    static QString indent;
};

