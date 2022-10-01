#include <QMessageBox>

#include "ShortcutSettingsController.h"
#include "libraries/ShortcutManager/ShortcutManager.h"


void ShortcutSettingsController::init() {
    // Создается вид с деревом настроек клавиатурных комбинаций
    view=new ShortcutSettingsView( qobject_cast<QWidget *>(parent()) ); // Вид размещается внутри виджета Screen
    view->setObjectName("shortcutSettingsView");
    // view->setController(this);

    // Создание модели данных
    model=new ShortcutSettingsModel(this);
    model->setObjectName("shortcutSettingsModel");

    view->setModel(model);
    view->init(); // Инит вида должен вызываться после установки модели
}


void ShortcutSettingsController::applyChanges() {
    // Проверка на наличие задвоенных сочетаний клавиш
    auto err = model->check();
    if(err) {
        QMessageBox msgBox;
        msgBox.setText(err.value());
        msgBox.exec();
    } else {
        model->save(); // Сохранение изменений на диск
        ShortcutManager::get().initKeyTable();
        static_cast<QWidget *>( parent() )->close(); // Закрытие окна настроек горячих клавиш
    }
}


ShortcutSettingsController::ShortcutData ShortcutSettingsController::getShortcutData(const QModelIndex &index) const {
    ShortcutData shortcutData;

    shortcutData.section    =model->data( index.parent() ).toString();
    shortcutData.command    =model->data( index.sibling(index.row(), 0) ) .toString();
    shortcutData.description=model->data( index.sibling(index.row(), 1) ) .toString();
    shortcutData.keys       =model->data( index.sibling(index.row(), 2) ) .toString();
    shortcutData.defaultKeys=model->data( index.sibling(index.row(), 3) ) .toString();

    return shortcutData;
}


ShortcutSettingsController::ShortcutData ShortcutSettingsController::getEmptyShortcutData() const {
    ShortcutData shortcutData;

    shortcutData.section    ="";
    shortcutData.command    ="";
    shortcutData.description="";
    shortcutData.keys       ="";
    shortcutData.defaultKeys="";

    return shortcutData;
}


void ShortcutSettingsController::setShortcut(QString shortcutFullName, QString sequenceText) {
    QModelIndex index=model->findShortcut( shortcutFullName );
    model->setData( index.sibling(index.row(), 2), sequenceText );
}

