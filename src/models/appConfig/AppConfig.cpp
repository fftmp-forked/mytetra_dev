#include <QColor>
#include <QDir>
#include <QFile>
#include <QRect>
#include <functional>

#include "AppConfig.h"
#include "libraries/GlobalParameters.h"

#include "libraries/helpers/DebugHelper.h"


void AppConfig::init() {
    auto confName = GlobalParameters::get().getWorkDirectory() + "/conf.ini";
    QFile confFile(confName);
    if (!confFile.exists()) {
        GlobalParameters::createStandartProgramFiles();
        confName = GlobalParameters::get().getWorkDirectory() + "/conf.ini";
    }

    // Создается указатель на объект хранилища конфигурации
    conf = new QSettings(confName, QSettings::IniFormat);

    update_version_process();

    sync();
}

// Получение параметра по имени в виде строки с проверкой его существования
QString AppConfig::get_parameter(QString name) const {
    if (conf->contains(name))
        return conf->value(name).toString();
    else
        criticalError("In config not found parameter " + name);
}

QString AppConfig::get_tetradir(void) const {
    auto d = get_parameter("tetradir");
    if (d[0] == '~')
        d.replace (0, 1, QDir::homePath());
    return d;
}

/// @brief Установка имени директории с данными (в которой находится mytetra.xml)
bool AppConfig::set_tetradir(QString dirName) {
    QDir directory(dirName);

    if (directory.exists() && directory.isReadable()) {
        conf->setValue("tetradir", dirName);
        return true;
    } else {
        return false;
    }
}

void AppConfig::set_addnewrecord_expand_info(QString state) {
    if (state == "0" || state == "1") {
        conf->setValue("addnewrecord_expand_info", state);
    } else {
        criticalError("Set unavailable value for addnewrecord_expand_info " + state);
    }
}

QList<int> AppConfig::get_splitter_size_list(QString name) const {
    QList<int> list;

    auto line_list = (conf->value(name + "_size_list", "100,100")).toString().split(",");

    for (const auto &l : line_list)
        list.append(l.toInt());

    return list;
}

void AppConfig::set_splitter_size_list(QString name, QList<int> list) {
    QStringList line_list;

    for (const auto &i : list)
        line_list.append(QString::number(i));

    conf->setValue(name + "_size_list", line_list.join(","));
}

void AppConfig::setRecordTableFieldsWidth(QStringList fields) {
    // В списке с одним элементом должна стоять стандартная величина (все равно она не влияет, и начнет влиять только после появления второй колонки)
    if (fields.size() == 1)
        fields[0] = "256";

    // Не должно быть так, чтобы был пустой список
    if (fields.size() == 0)
        fields << "256";

    conf->setValue("recordTableFieldsWidth", fields.join(","));
}

void AppConfig::setRecordWithAttachHighlightColor(QString color) {
    QColor saveColor(color);

    // Если сохраняемая строка действительно содержит закодированный цвет
    if (saveColor.isValid()) {
        conf->setValue("recordWithAttachHighlightColor", color);
    }
}

void AppConfig::setDockableWindowsBehavior(QString mode) {
    if (mode == "single" || mode == "together")
        conf->setValue("dockableWindowsBehavior", mode);
    else
        criticalError("Set unavailable value for dockableWindowsBehavior " + mode);
}

// ------------------------------------
// Методы для обновления версии конфига
// ------------------------------------

void AppConfig::add_default_param(const QString name, const QVariant value) {
    if (!conf->contains(name))
        conf->setValue(name, value);
}

void AppConfig::set_version_1_default_params() {
    add_default_param("addnewrecord_expand_info", 1);
    add_default_param("findscreen_find_inname", true);
    add_default_param("findscreen_find_intags", true);
    add_default_param("findscreen_find_intext", true);
    add_default_param("findscreen_howextract", 1);
    add_default_param("findscreen_show", false);
    add_default_param("findscreen_wordregard", 1);
    add_default_param("findsplitter_size_list", "517,141");
    add_default_param("hspl_size_list", "237,621");
    add_default_param("lastidnum", 3537);
    add_default_param("lastnotenum", 3119);
    add_default_param("lastprefixnum", 7540);
    add_default_param("mainwingeometry", "155,24,864,711)");
    add_default_param("recordtable_position", 0);
    add_default_param("tetradir", "/opt/mytetra/data");
    add_default_param("tree_position", "0,1818,1819");
    add_default_param("vspl_size_list", "171,487");
    add_default_param("findscreen_find_inauthor", true);
    add_default_param("findscreen_find_inurl", false);
}

void AppConfig::update_version_process() {
    switch (conf->value("version").toInt()) {
    case 1:
        set_version_1_default_params();
        [[fallthrough]];
    case 2:
        add_default_param("cutbranchconfirm", true);
        [[fallthrough]];
    case 3:
        add_default_param("printdebugmessages", false);
        [[fallthrough]];
    case 4:
        add_default_param("interfacelanguage", "en");
        add_default_param("programm", "mytetra");
        [[fallthrough]];
    case 5:
        conf->remove("lastidnum");
        conf->remove("lastnotenum");
        conf->remove("lastprefixnum");
        [[fallthrough]];
    case 6:
    case 7:
        add_default_param("runinminimizedwindow", false);
        [[fallthrough]];
    case 8:
        add_default_param("synchrocommand", "");
        add_default_param("synchroonstartup", false);
        add_default_param("synchroonexit", false);
        [[fallthrough]];
    case 9:
        add_default_param("syncroConsoleDetails", false);
        [[fallthrough]];
    case 10:
    case 11:
        add_default_param("editorCursorPosition", 0);
        add_default_param("editorScrollBarPosition", 0);
        [[fallthrough]];
    case 12:
    case 13:
        add_default_param("rememberCursorAtHistoryNavigation", true);
        add_default_param("rememberCursorAtOrdinarySelection", true);
        [[fallthrough]];
    case 14:
        add_default_param("findScreenTreeSearchArea", 0);
        [[fallthrough]];
    case 15: // Так как не все параметры можно стилизовать через QSS, здесь задается высота ячейки таблицы
        add_default_param("uglyQssReplaceHeightForTableView", 0);
        [[fallthrough]];
    case 16:
        add_default_param("recordTableShowFields", "name,tags");
        add_default_param("recordTableFieldsWidth", "256,128");
        add_default_param("recordTableShowHorizontalHeaders", true);
        add_default_param("recordTableShowVerticalHeaders", false);
        [[fallthrough]];
    case 17:
    case 18:
    case 19:
        add_default_param("focusWidget", "");
        [[fallthrough]];
    case 20:
    case 21:
        add_default_param("findInBaseExpand", true);
        [[fallthrough]];
    case 22:
        add_default_param("recordtableSelectedRecordId", "");
        [[fallthrough]];
    case 23:
        conf->remove("recordtable_position");
        [[fallthrough]];
    case 24:
        add_default_param("enableCustomDateTimeFormat", false);
        add_default_param("customDateTimeFormat", "");
        [[fallthrough]];
    case 25:
        add_default_param("attachAppendDir", "");
        add_default_param("attachSaveAsDir", "");
        [[fallthrough]];
    case 26:
    case 27:
    case 28:
        add_default_param("enableLogging", false);
        [[fallthrough]];
    case 29:
        add_default_param("enableRecordWithAttachHighlight", true);
        add_default_param("recordWithAttachHighlightColor", "#d1e3c5");
        [[fallthrough]];
    case 30:
        add_default_param("enablePeriodicCheckBase", false);
        add_default_param("checkBasePeriod", 20);
        add_default_param("enablePeriodicCheckMessage", false);
        [[fallthrough]];
    case 31:
        add_default_param("previewIconSize", 24);
        [[fallthrough]];
    case 32:
        add_default_param("iconCurrentSectionName", "Essential");
        [[fallthrough]];
    case 33:
        add_default_param("enablePeriodicSyncro", false);
        add_default_param("periodicSyncroPeriod", 300);
        [[fallthrough]];
    case 34:
    case 35:
        add_default_param("findscreen_find_innameItem", false);
        [[fallthrough]];
    case 36: // Предполагаемый формат - "IDзаписи1,x1,y1,w1,h1;IDзаписи2,x2,y2,w2,h2..."
        add_default_param("dockableWindowsState", "");
        [[fallthrough]];
    case 37:
        add_default_param("dockableWindowsBehavior", "single");
        [[fallthrough]];
    case 38: {
        auto val = conf->value("dockableWindowsState").toString();
        if (val.trimmed().size() > 1) {
            auto chunks = val.split(';');
            for (auto &c : chunks)
                c += ",0";

            val = chunks.join(';');
            conf->setValue("dockableWindowsState", val);
        }
        [[fallthrough]];
    }
    case 39: {
        // rename '*syncro*' to '*synchro*'
        auto pv = conf->value("enablePeriodicSyncro");
        conf->remove("enablePeriodicSyncro");
        add_default_param("enablePeriodicSynchro", pv);

        pv = conf->value("periodicSyncroPeriod");
        conf->remove("periodicSyncroPeriod");
        add_default_param("periodicSynchroPeriod", pv);

        pv = conf->value("syncroConsoleDetails");
        conf->remove("syncroConsoleDetails");
        add_default_param("synchroConsoleDetails", pv);
    }
    }

    // Устанавливается новый номер версии
    conf->setValue("version", 40);

    // Конфигурация записывается на диск
    conf->sync();
}
