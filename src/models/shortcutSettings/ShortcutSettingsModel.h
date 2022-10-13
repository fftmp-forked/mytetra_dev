#pragma once

#include <QStandardItemModel>

/// @brief Модель таблицы с настройками горячих клавиш
class ShortcutSettingsModel : public QStandardItemModel {
    Q_OBJECT

  public:
    ShortcutSettingsModel(QObject *parent = nullptr);
    ~ShortcutSettingsModel() { this->clear(); }

    QModelIndex findShortcut(const QString &shortcutFullName) const;
    void resetAllShortcutsToDefault();

    std::optional<QString> check() const;
    void save();

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int columnCount(const QModelIndex &itemIndex = QModelIndex()) const {
        Q_UNUSED(itemIndex);
        return 4;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const;

  protected:
    bool smartUpdate();
};
