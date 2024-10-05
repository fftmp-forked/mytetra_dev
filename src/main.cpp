#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QTextOption>
#include <QToolButton>
#include <QTranslator>

#include <QScrollBar>
#include <QScroller>
#include <QScrollerProperties>

#include "main.h"
#include "views/mainWindow/MainWindow.h"

#include "../thirdParty/qtSingleApplication/qtsingleapplication.h"

#include "libraries/GlobalParameters.h"
#include "libraries/IconSelectDialog.h"
#include "libraries/PeriodicCheckBase.h"
#include "libraries/PeriodicSynchro.h"
#include "libraries/ShortcutManager/ShortcutManager.h"
#include "libraries/helpers/CssHelper.h"
#include "libraries/helpers/DebugHelper.h"
#include "models/appConfig/AppConfig.h"

/// @todo: Разгрести объекты глобальной области, переделать все
/// на синглтоны, сделать объект ядра, поместить объекты глобальной
/// области в ядро как в единую центральную точку доступа

// Различные периодические проверки
PeriodicCheckBase periodicCheckBase;
PeriodicSynchro periodicSynchro;

QObject *pMainWindow;
WalkHistory *walkHistory;


static void process_control_options(QtSingleApplication & app, const QString & cmd) {
    QStringList simple_commands = {"show", "hide", "quit", "reload", "addNoteDialog"};
    if (simple_commands.contains(cmd)) {
        app.sendMessage(cmd);
        return;
    }

    const QStringList commands_with_arg = {"openNote", "openTreeItem"};
    for (auto & str : commands_with_arg) {
        if(cmd.startsWith(str)) {
            app.sendMessage(cmd);
            return;
        }
    }
    qInfo() << "Unknown control option " << cmd;
}


int main(int argc, char **argv) {
    Q_INIT_RESOURCE(mytetra);

    QtSingleApplication app(argc, argv);
    app.setApplicationVersion(APPLICATION_VERSION);

    // parse CLI options
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        {"control", "Send command to already running instance of MyTetra. Available commands:\n"
                    "show - Show and activate MyTetra window\n"
                    "hide - Hide MyTetra window\n"
                    "quit - Quit from MyTetra\n"
                    "reload - Reload database\n"
                    "addNoteDialog - Show dialod for create new note in current tree item\n"
                    "openNote=<noteId> - Jump to note with <noteId>\n"
                    "openTreeItem=<treeItemId> - Jump to tree item with <treeItemId>", "cmd"},
        {{"c", "cfg_dir"}, "specify directory with conf.ini instead of default location", "dir", QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)}
    });

    parser.process(app);

    auto control_cmd = parser.value("control");
    auto cfg_dir = parser.value("cfg_dir");

    if (app.isRunning()) {
        if(control_cmd.isEmpty()) {
            QMessageBox msgBox(QMessageBox::Warning, "Info", "Another MyTetra instance is running.");
            msgBox.exec();
            exit(1);
        } else {
            process_control_options(app, control_cmd);
            exit(0);
        }
    } else {
        if (!control_cmd.isEmpty()) {
            qInfo() << "MyTetra instance for control is not running. Run MyTetra before running your command.";
            exit(2);
        }
    }

    if (!GlobalParameters::get().init(cfg_dir)) {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "Can't use config from " + cfg_dir);
        msgBox.exec();
        exit(3);
    }

    // Инициализация основных конфигурирующих программу переменных
    auto &cfg = AppConfig::get();
    cfg.init();

    setupDebug(cfg.get_printdebugmessages());
    // Проверяется наличие коллекции прикрепляемых к веткам иконок (и иконки создаются если они отсутствуют)
    IconSelectDialog::iconsCollectionCheck();

    CssHelper::setCssStyle(cfg_dir);

    // Подключение перевода интерфейса
    QString langFileName = ":/resource/translations/mytetra_" + cfg.get_interfacelanguage() + ".qm";
    qDebug() << "Use language file " << langFileName;
    QTranslator langTranslator;
    if (langTranslator.load(langFileName))
        app.installTranslator(&langTranslator);
    else
        qDebug() << "Can't install translations";

    //Загрузка переводов стандартных диалогов и кнопок
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + cfg.get_interfacelanguage(), QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        if (!app.installTranslator(&qtTranslator))
            qDebug() << "Can't install QT translations";
    }

    // Инициализация менеджера горячих клавиш должна происходить после инициализации переводов,
    // чтобы были переведены все действия по горячим клавишам
    ShortcutManager::get().init(cfg_dir + "/shortcut.ini");

    // История перехода очищается, так как в нее может попасть первая запись в востаналиваемой ветке и сама восстанавливаемая запись
    WalkHistory wk;
    walkHistory = &wk;

    // Создание объекта главного окна
    MainWindow win;
    pMainWindow = &win;
    win.init();

    srand(time(NULL));

    // Сразу восстанавливается вид окна в предыдущий запуск
    // Эти действия нельзя делать в конструкторе главного окна, т.к. окно еще не создано
    // Эти действия надо делать до установки заголовка. В некоторых оконных средах,
    // если сделать после setWindowTitle(), геометрия восстановится некорректно, окно съедет вверх на толщину заголовка
    win.restoreAllWindowState();

    // Настройка объекта главного окна
    win.setWindowTitle("MyTetra");
    if (!cfg.get_runinminimizedwindow())
        win.show();
    else
        win.hide();

    // Восстанавливаются открепляемые окна
    win.restoreDockableWindowsState();

    qDebug() << "Restore session succesfull";

    if (cfg.get_synchroonstartup() && cfg.get_synchrocommand().trimmed().length() > 0)
        win.synchronization();

    // Инициалиация периодической проверки изменения базы сторонними программами
    periodicCheckBase.init();
    periodicCheckBase.setDelay(cfg.getCheckBasePeriod());
    periodicCheckBase.start();

    // Инициалиация периодической синхронизации
    periodicSynchro.init();
    periodicSynchro.setDelay(cfg.getPeriodicSynchroPeriod());
    periodicSynchro.start();

    // При закрытии окна не выходить из программы.
    // Окно программы может быть снова открыто из трея
    QApplication::setQuitOnLastWindowClosed(false);

    app.connect(&app, &QtSingleApplication::lastWindowClosed, &app, &QtSingleApplication::quit);

    // Прием сообщений, испускаемых другим экземпляром MyTetra с помощью консольных команд "--control"
    app.connect(&app, &QtSingleApplication::messageReceived, &win, &MainWindow::messageHandler);

    return app.exec();
}
