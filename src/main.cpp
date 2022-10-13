#include <QLibraryInfo>
#include <QMessageBox>
#include <QSplashScreen>
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

void printHelp() {
    printf("\n");
    printf("MyTetra %s\n", APPLICATION_VERSION);
    printf("For use control mode, run by standard way MyTetra for show GUI interface, and next use command:\n");
    printf("./mytetra --control --show - Show and activate MyTetra window\n");
    printf("./mytetra --control --hide - Hide MyTetra window\n");
    printf("./mytetra --control --quit - Quit from MyTetra\n");
    printf("./mytetra --control --reload - Reload database\n");
    printf("./mytetra --control --openNote <noteId> - Jump to note with <noteId>\n");
    printf("./mytetra --control --addNoteDialog - Show dialod for create new note in current tree item\n");
    printf("./mytetra --control --openTreeItem <treeItemId> - Jump to tree item with <treeItemId>\n");
    printf("\n");
}

static void parseConsoleOption(QtSingleApplication &app) {
    // Если запрашивается помощь по опциям
    if (app.arguments().contains("-h") || app.arguments().contains("--help")) {
        printHelp();
        exit(0);
    }

    // Если MyTetra запущена с какой-то опцией, но нет опции --control
    if (app.arguments().count() > 1 && !app.arguments().contains("--control")) {
        QString message = "Bad options. May be you lost \"--control\"?\n";
        printf("%s", message.toLocal8Bit().data());
        exit(1);
    }

    // Если MyTetra запущена в режиме управления, а другого экземпляра (которым надо управлять) нет
    if (app.arguments().contains("--control") && !app.isRunning()) {
        QString message = "MyTetra exemplar for control is not running.\nPlease, run MyTetra before running your command.\n";
        printf("%s", message.toLocal8Bit().data());
        exit(2);
    }

    // Если MyTetra запущена в обычном режиме, но уже запущен другой экземпляр
    if (!app.arguments().contains("--control") && app.isRunning()) {
        QString message = "Another MyTetra exemplar is running.\n";

        printf("%s", message.toLocal8Bit().data());

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning); // Сообщение со значком предупреждения
        msgBox.setText(message);
        msgBox.exec();

        exit(3);
    }

    // Если MyTetra запущена в режиме управления, и есть другой экземпляр, которым нужно управлять
    if (app.arguments().contains("--control") && app.isRunning()) {
        if (app.arguments().contains("--show")) {
            app.sendMessage("show");
            exit(0);
        } else if (app.arguments().contains("--hide")) {
            app.sendMessage("hide");
            exit(0);
        } else if (app.arguments().contains("--quit")) {
            app.sendMessage("quit");
            exit(0);
        } else if (app.arguments().contains("--reload")) {
            app.sendMessage("reload");
            exit(0);
        } else if (app.arguments().contains("--openNote")) {
            int openNoteIndex = app.arguments().indexOf("--openNote");
            app.sendMessage("openNote " + app.arguments().at(openNoteIndex + 1));
            exit(0);
        } else if (app.arguments().contains("--addNoteDialog")) {
            app.sendMessage("addNoteDialog");
            exit(0);
        } else if (app.arguments().contains("--openBranch")) // Устаревшая опция
        {
            int openBranchIndex = app.arguments().indexOf("--openBranch");
            app.sendMessage("openTreeItem " + app.arguments().at(openBranchIndex + 1)); // Аналог сигнала openTreeItem
            exit(0);
        } else if (app.arguments().contains("--openTreeItem")) {
            int openTreeItemIndex = app.arguments().indexOf("--openTreeItem");
            app.sendMessage("openTreeItem " + app.arguments().at(openTreeItemIndex + 1));
            exit(0);
        } else {
            QString message = "Unknown control option.\n";
            printf("%s", message.toLocal8Bit().data());
            exit(4);
        }
    }
}

int main(int argc, char **argv) {
    printf("\nStart MyTetra %s\n", APPLICATION_VERSION);

    Q_INIT_RESOURCE(mytetra);

    // Начальные инициализации основных объектов

    // Запоминается имя файла запущенного бинарника
    // Файл запущенной программы (нулевой аргумент функции main)
    // Метод fromLocal8Bit корректно работает для 8-ми битных кодировок локали (Win) и для UTF-8 (Lin)
    // даже если в имени файла встречаются национальные символы, а кодек еще не установлен
    QString mainProgramFile = QString::fromLocal8Bit(argv[0]); // Данные запоминаются в сыром виде и никак не интерпретируются до использования

    // Создание объекта приложения
    QtSingleApplication app(argc, argv);

    // Обработка консольных опций
    parseConsoleOption(app);

    // Инициализация глобальных параметров, внутри происходит установка рабочей директории, настройка кодеков для локали и консоли
    GlobalParameters::get().init(mainProgramFile);

    // Инициализация основных конфигурирующих программу переменных
    AppConfig::get().init(GlobalParameters::get().getWorkDirectory() + "/conf.ini");
    auto &cfg = AppConfig::get();

    setupDebug(cfg.get_printdebugmessages());
    // Проверяется наличие коллекции прикрепляемых к веткам иконок (и иконки создаются если они отсутствуют)
    IconSelectDialog::iconsCollectionCheck();

    CssHelper::setCssStyle(GlobalParameters::get().getWorkDirectory());

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
    ShortcutManager::get().init(GlobalParameters::get().getWorkDirectory() + "/shortcut.ini");

    // История перехода очищается, так как в нее может попасть первая запись в востаналиваемой ветке и сама восстанавливаемая запись
    WalkHistory wk;
    walkHistory = &wk;

    // Создание объекта главного окна
    MainWindow win;
    pMainWindow = &win;
    win.init();

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
