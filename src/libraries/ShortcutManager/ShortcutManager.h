#pragma once

#include <QKeySequence>
#include <QMap>
#include <QObject>
#include <QSettings>

class QAction;
class QKeySequence;
class QToolButton;

/// @brief singleton class
class ShortcutManager : public QObject {
    Q_OBJECT

  public:
    ShortcutManager() = delete;
    ShortcutManager(const ShortcutManager &) = delete;
    ShortcutManager &operator=(const ShortcutManager &) = delete;
    ShortcutManager(ShortcutManager &&) = delete;
    ShortcutManager &operator=(ShortcutManager &&) = delete;
    ~ShortcutManager() {}

    static void init(QString confName) { _self = new ShortcutManager(confName); }
    static ShortcutManager &get() { return *_self; }

    enum shortcutSections {
        SECTION_NOTE,
        SECTION_TREE,
        SECTION_EDITOR,
        SECTION_ATTACH,
        SECTION_MISC,
        SECTION_TOTAL_COUNT // must be last
    };

    struct Data {
        QKeySequence sequence;
        QString description; // Описание (короткая строка)
        QString explanation; // Пояснение (длинная строка)
    };

    static const QStringList availableSection;
    static QMap<QString, Data> defaultKeyTable[SECTION_TOTAL_COUNT]; // map keys is actions

    void saveConfig(const QMap<QString, QKeySequence> table[]) const;
    void updateShortcutInApplication() { emit updateWidgetShortcut(); }
    void initKeyTable();

    QKeySequence getKeySequence(shortcutSections section, QString actionName) const { return keyTable[section][actionName]; }
    void setKeySequence(shortcutSections section, QString actionName, QString keySequence);
    QString getDescription(shortcutSections section, QString actionName) const;
    QString getExplanation(shortcutSections section, QString actionName) const;

    QString getDescriptionWithShortcut(shortcutSections section, QString actionName) const;
    QString getFullDescription(shortcutSections section, QString actionName) const;
    QString getKeySequenceAsText(shortcutSections section, QString actionName) const { return keyTable[section][actionName].toString(); }

    QKeySequence getDefaultKeySequence(shortcutSections section, QString actionName) const { return defaultKeyTable[section][actionName].sequence; }
    QString getDefaultKeySequenceAsText(shortcutSections section, QString actionName) const { return getDefaultKeySequence(section, actionName).toString(); }

    void initAction(shortcutSections section, QString actionName, QAction *action);
    void initActions(shortcutSections section, const QList<QPair<QString, QAction *>> &actions);
    void initToolButton(shortcutSections section, QString actionName, QToolButton *action);

    QStringList getActionsNameList(shortcutSections section) const { return defaultKeyTable[section].keys(); }

    bool isOverloadEnable(QString sectionNameA, QString sectionNameB) const;

  signals:
    void updateWidgetShortcut();

  private:
    explicit ShortcutManager(QString confName, QObject *parent = nullptr);

    void initDefaultKeyTable();

    static ShortcutManager *_self;
    static QMap<QString, QKeySequence> keyTable[SECTION_TOTAL_COUNT]; // map keys is action names

    QString configFileName;
};
