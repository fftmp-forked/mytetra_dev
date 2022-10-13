#pragma once

#include <QMessageBox>
#include <QString>

/// @brief Выдача на экран простого окна с сообщением
inline void showMessageBox(QString message) {
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}
