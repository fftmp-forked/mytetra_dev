#ifndef GLOBALPARAMETERS_H
#define	GLOBALPARAMETERS_H

#include <QObject>
#include <QStatusBar>

#include "FixedParameters.h"

class TreeScreen;
class MetaEditor;
class RecordTableScreen;
class FindScreen;
class WindowSwitcher;
class CommandRun;

class GlobalParameters : public QObject
{
    Q_OBJECT

public:
    GlobalParameters(QObject *pobj=nullptr);
    virtual ~GlobalParameters();

    void setMainProgramFile(QString file);
    QString getMainProgramFile(void);

    void init(void);

    // Получение рабочей директории. Рабочая директория - это та, где лежит файл conf.ini
    QString getWorkDirectory(void);

    QString getActionLogFileName();
    QString getActionLogPrevFileName();

    enum class OS_type {Android, Desktop};
    static constexpr OS_type getOs(void) {
        // see also QOperatingSystemVersionBase::currentType()
        #if defined(Q_OS_ANDROID)
            return GlobalParameters::OS_type::Android;
        #else
            return GlobalParameters::OS_type::Desktop;
        #endif
    }

    static QString getOsStr(void) { return (getOs() == OS_type::Android ? "android" : "any"); }


    static QString getApplicationName(void) {
        if(getOs() == OS_type::Desktop)
            return FixedParameters::appTextId;
        else
            return QString("ru.webhamster.") + FixedParameters::appTextId;
    }


    void setTreeScreen(TreeScreen *point);
    TreeScreen *getTreeScreen();

    void setRecordTableScreen(RecordTableScreen *point);
    RecordTableScreen *getRecordTableScreen();

    void setFindScreen(FindScreen *point);
    FindScreen *getFindScreen();

    void setMetaEditor(MetaEditor *point);
    MetaEditor *getMetaEditor();

    void setStatusBar(QStatusBar *point);
    QStatusBar *getStatusBar();

    void setWindowSwitcher(WindowSwitcher *point);
    WindowSwitcher *getWindowSwitcher();

    QString getInstallAutodetectLang();

    // Файл стилей может создаваться и после развертывания начальных файлов MyTetra
    // Так как в более старых версиях MyTetra его еще не было
    void createStyleSheetFile(QString dirName);

public:
    // Указание на обрабатываемую панель инструментов редактора текста
    enum EditorToolbar {
        First = 0,
        Second
    };

    // Возможные режимы добавления записей в таблицу конечных записей
    enum AddNewRecordBehavior
    {
        ADD_TO_END=0,
        ADD_BEFORE,
        ADD_AFTER
    };

private:

    void initWorkDirectory(void);
    bool findWorkDirectory(void);
    bool isMytetraIniConfig(QString fileName);
    void createStandartProgramFiles(void);
    void createPortableProgramFiles(void);
    void createFirstProgramFiles(QString dirName);

    TreeScreen *pointTreeScreen=nullptr;
    RecordTableScreen *pointRecordTableScreen=nullptr;
    FindScreen *pointFindScreen=nullptr;
    MetaEditor *pointMetaEditor=nullptr;
    QStatusBar *pointStatusBar=nullptr;
    WindowSwitcher *windowSwitcher=nullptr;

    QString mainProgramFile;
    QString workDirectory;

    //! Язык, который был автоопределен если запускалась инсталляция базы знаний.
    //! Если автоинсталляция не запускалась, это значение будет пустой строкой
    QString installAutodetectLang;
};

#endif	/* GLOBALPARAMETERS_H */

