#pragma once

#include <QDebug>
#include <QSettings>
#include "../../libraries/Singleton.h"

class AppConfig : public Singleton<AppConfig> {
    friend class Singleton<AppConfig>;
  public:

    ~AppConfig() { qInfo() << " APPCONFIG DTOR"; sync(); delete conf; }

    void init();

    void sync() { conf->sync(); }

    QString getConfigFileName() const { return conf->fileName(); }

    QString get_tetradir() const;
    bool set_tetradir(QString dirName);

    // Нужно ли показывать подтверждение при выполнении действия "cut на ветке
    bool get_cutbranchconfirm() const { return conf->value("cutbranchconfirm").toBool(); }
    void set_cutbranchconfirm(bool confirm) { conf->setValue("cutbranchconfirm", confirm); }

    // Нужно ли печатать отладочные сообщения в консоль
    bool get_printdebugmessages() const { return conf->value("printdebugmessages").toBool(); }
    void set_printdebugmessages(bool isPrint) { conf->setValue("printdebugmessages", isPrint); }

    QString get_interfacelanguage() const { return get_parameter("interfacelanguage"); }
    void set_interfacelanguage(QString language) { conf->setValue("interfacelanguage", language); }

    // Номер последнего префикса в виде строки с ведущими нулями
    QString get_addnewrecord_expand_info() const { return get_parameter("addnewrecord_expand_info"); }
    void set_addnewrecord_expand_info(QString);

    // Геометрия основного окна
    QByteArray get_mainwingeometry() const { return QByteArray::fromBase64(conf->value("mainwingeometry", "").toString().toLatin1()); }
    void set_mainwingeometry(QByteArray dataGeometry) { conf->setValue("mainwingeometry", QVariant(QString(dataGeometry.toBase64().data()))); }

    QList<int> get_vspl_size_list() const { return get_splitter_size_list("vspl"); }
    void set_vspl_size_list(QList<int> list) { set_splitter_size_list("vspl", list); }

    QList<int> get_hspl_size_list() const { return get_splitter_size_list("hspl"); }
    void set_hspl_size_list(QList<int> list) { set_splitter_size_list("hspl", list); }

    QList<int> get_findsplitter_size_list() const { return get_splitter_size_list("findsplitter"); }
    void set_findsplitter_size_list(QList<int> list) { set_splitter_size_list("findsplitter", list); }

    QList<int> get_splitter_size_list(QString name) const;
    void set_splitter_size_list(QString name, QList<int> list);

    QStringList get_tree_position() const { return conf->value("tree_position", "1").toString().split(","); }
    void set_tree_position(QStringList list) { conf->setValue("tree_position", list.join(",")); }

    // ID записи в таблице конечных записей, которая выделена на экране
    QString get_recordtable_selected_record_id() const { return conf->value("recordtableSelectedRecordId", 0).toString(); }
    void set_recordtable_selected_record_id(QString id) { conf->setValue("recordtableSelectedRecordId", id); }

    // Настройки виджета поиска по базе
    int get_findscreen_wordregard() const { return conf->value("findscreen_wordregard", 0).toInt(); }
    void set_findscreen_wordregard(int pos) { conf->setValue("findscreen_wordregard", pos); }

    int get_findscreen_howextract() const { return conf->value("findscreen_howextract", 0).toInt(); }
    void set_findscreen_howextract(int pos) { conf->setValue("findscreen_howextract", pos); }

    int getFindScreenTreeSearchArea() const { return conf->value("findScreenTreeSearchArea", 0).toInt(); }
    void setFindScreenTreeSearchArea(int pos) { conf->setValue("findScreenTreeSearchArea", pos); }

    bool get_findscreen_find_in_field(QString fieldName) const { return conf->value("findscreen_find_in" + fieldName, 0).toBool(); }
    void set_findscreen_find_in_field(QString fieldName, bool isChecked) { conf->setValue("findscreen_find_in" + fieldName, isChecked); }

    bool get_findscreen_show() const { return conf->value("findscreen_show", 0).toBool(); }
    void set_findscreen_show(bool isShow) { conf->setValue("findscreen_show", isShow); }

    bool get_runinminimizedwindow() const { return conf->value("runinminimizedwindow").toBool(); }
    void set_runinminimizedwindow(bool flag) { conf->setValue("runinminimizedwindow", flag); }

    QString get_synchrocommand() const { return get_parameter("synchrocommand"); }
    void set_synchrocommand(QString command) { conf->setValue("synchrocommand", command); }

    bool get_synchroonstartup() const { return conf->value("synchroonstartup").toBool(); }
    void set_synchroonstartup(bool flag) { conf->setValue("synchroonstartup", flag); }

    bool get_synchroonexit() const { return conf->value("synchroonexit").toBool(); }
    void set_synchroonexit(bool flag) { conf->setValue("synchroonexit", flag); }

    bool getSynchroConsoleDetails() const { return conf->value("synchroConsoleDetails").toBool(); }
    void setSynchroConsoleDetails(bool flag) { conf->setValue("synchroConsoleDetails", flag); }

    int getEditorCursorPosition() const { return conf->value("editorCursorPosition", 0).toInt(); }
    void setEditorCursorPosition(int n) { conf->setValue("editorCursorPosition", n); }

    int getEditorScrollBarPosition() const { return conf->value("editorScrollBarPosition", 0).toInt(); }
    void setEditorScrollBarPosition(int n) { conf->setValue("editorScrollBarPosition", n); }

    bool getRememberCursorAtHistoryNavigation() const { return conf->value("rememberCursorAtHistoryNavigation").toBool(); }
    void setRememberCursorAtHistoryNavigation(bool flag) { conf->setValue("rememberCursorAtHistoryNavigation", flag); }

    bool getRememberCursorAtOrdinarySelection() const { return conf->value("rememberCursorAtOrdinarySelection").toBool(); }
    void setRememberCursorAtOrdinarySelection(bool flag) { conf->setValue("rememberCursorAtOrdinarySelection", flag); }

    int getUglyQssReplaceHeightForTableView() const { return conf->value("uglyQssReplaceHeightForTableView", 0).toInt(); }
    void setUglyQssReplaceHeightForTableView(int n) { conf->setValue("uglyQssReplaceHeightForTableView", n); }

    // Перечень полей, отображаемых в таблице конечных записей
    QStringList getRecordTableShowFields() const { return conf->value("recordTableShowFields", "name").toString().split(","); }
    void setRecordTableShowFields(QStringList fields) {
        if (fields.size() == 0)
            fields << "name";
        conf->setValue("recordTableShowFields", fields.join(","));
    }

    // Нужно ли отображать горизонтальные заголовки в таблице конечных записей
    bool getRecordTableShowHorizontalHeaders() const { return conf->value("recordTableShowHorizontalHeaders").toBool(); }
    void setRecordTableShowHorizontalHeaders(bool flag) { conf->setValue("recordTableShowHorizontalHeaders", flag); }

    // Нужно ли отображать вертикальные заголовки в таблице конечных записей
    bool getRecordTableShowVerticalHeaders() const { return conf->value("recordTableShowVerticalHeaders").toBool(); }
    void setRecordTableShowVerticalHeaders(bool flag) { conf->setValue("recordTableShowVerticalHeaders", flag); }

    // Ширина полей, отображаемых в таблице конечных записей
    QStringList getRecordTableFieldsWidth() const { return (conf->value("recordTableFieldsWidth", "256")).toString().split(","); }
    void setRecordTableFieldsWidth(QStringList fieldsWidth);

    QString getFocusWidget() const { return get_parameter("focusWidget"); }
    void setFocusWidget(QString widgetName) { conf->setValue("focusWidget", widgetName); }

    // Флаг, показывающий распахнуты ли все инструметы управления в окне поиска по базе
    bool getFindInBaseExpand() const { return conf->value("findInBaseExpand").toBool(); }
    void setFindInBaseExpand(bool state) { conf->setValue("findInBaseExpand", state); }

    // Разрешено ли использовать собственный формат вывода даты и времени
    bool getEnableCustomDateTimeFormat() const { return conf->value("enableCustomDateTimeFormat").toBool(); }
    void setEnableCustomDateTimeFormat(bool state) { conf->setValue("enableCustomDateTimeFormat", state); }

    // Строка собственного формата вывода даты и времени
    QString getCustomDateTimeFormat() const { return get_parameter("customDateTimeFormat"); }
    void setCustomDateTimeFormat(QString format) { conf->setValue("customDateTimeFormat", format); }

    // Путь на диске по которому пользователь открывал файлы чтобы приаттачить их к записи
    QString getAttachAppendDir() const { return get_parameter("attachAppendDir"); }
    void setAttachAppendDir(QString dir) { conf->setValue("attachAppendDir", dir); }

    // Путь на диске по которому пользователь сохранял (Сохранить как...) приаттаченные файлы
    QString getAttachSaveAsDir() const { return get_parameter("attachSaveAsDir"); }
    void setAttachSaveAsDir(QString dir) { conf->setValue("attachSaveAsDir", dir); }

    // Разрешено ли логирование
    bool getEnableLogging() const { return conf->value("enableLogging").toBool(); }
    void setEnableLogging(bool state) { conf->setValue("enableLogging", state); }

    // Разрешена ли подсветка записей с прикрепленными файлами
    bool getEnableRecordWithAttachHighlight() const { return conf->value("enableRecordWithAttachHighlight").toBool(); }
    void setEnableRecordWithAttachHighlight(bool state) { conf->setValue("enableRecordWithAttachHighlight", state); }

    // Цвет подсветки записей с прикрепленными файлами
    QString getRecordWithAttachHighlightColor() const { return get_parameter("recordWithAttachHighlightColor"); }
    void setRecordWithAttachHighlightColor(QString color); // Формат цвета - "#0099FF"

    // Разрешена ли периодическая проверка файла базы на предмет изменения сторонней программой
    bool getEnablePeriodicCheckBase() const { return conf->value("enablePeriodicCheckBase").toBool(); }
    void setEnablePeriodicCheckBase(bool state) { conf->setValue("enablePeriodicCheckBase", state); }

    // Период проверки файла базы на предмет изменения сторонней программой
    int getCheckBasePeriod() const { return get_parameter("checkBasePeriod").toInt(); }
    void setCheckBasePeriod(int period) { conf->setValue("checkBasePeriod", period); }

    // Необходимо ли выводить сообщение если база была изменена другой программой
    bool getEnablePeriodicCheckMessage() const { return conf->value("enablePeriodicCheckMessage").toBool(); }
    void setEnablePeriodicCheckMessage(bool state) { conf->setValue("enablePeriodicCheckMessage", state); }

    // Размер иконок в окне выбора (иконки, прикрепляемые к веткам)
    int getPreviewIconSize() const { return conf->value("previewIconSize", 0).toInt(); }
    void setPreviewIconSize(int n) { conf->setValue("previewIconSize", n); }

    // Последняя выбранная секция в окошке выбора иконок
    QString getIconCurrentSectionName() const { return get_parameter("iconCurrentSectionName"); }
    void setIconCurrentSectionName(QString name) { conf->setValue("iconCurrentSectionName", name); }

    // Разрешена ли периодическая синхронизация
    bool getEnablePeriodicSynchro() const { return conf->value("enablePeriodicSynchro").toBool(); }
    void setEnablePeriodicSynchro(bool state) { conf->setValue("enablePeriodicSynchro", state); }

    // Период автоматической периодической синхронизации
    int getPeriodicSynchroPeriod() const { return get_parameter("periodicSynchroPeriod").toInt(); }
    void setPeriodicSynchroPeriod(int period) { conf->setValue("periodicSynchroPeriod", period); }

    // Список открепляемых окон с координатами окон
    QString getDockableWindowsState() const { return get_parameter("dockableWindowsState"); }
    void setDockableWindowsState(QString state) { conf->setValue("dockableWindowsState", state); }

    QString getDockableWindowsBehavior() const { return get_parameter("dockableWindowsBehavior"); }
    void setDockableWindowsBehavior(QString mode);

  private:
    QSettings *conf;

    QString get_parameter(QString name) const;

    void add_default_param(const QString name, const QVariant value);
    void set_version_1_default_params();
    void update_version_process();
};
