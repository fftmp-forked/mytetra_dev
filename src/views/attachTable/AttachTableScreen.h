#pragma once

#include <QAction>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

class AttachTableView;
class AttachTableController;

/// @brief Виджет таблицы приаттаченных файлов (с кнопками)
class AttachTableScreen : public QWidget {
    Q_OBJECT
  public:
    AttachTableScreen(QWidget *parent = nullptr);

    void clear(void);

    QAction *actionAddAttach;        // Добавление локального файла
    QAction *actionAddAttachFromUrl; // Добавление файла по интернет-Url
    QAction *actionAddLink;          // Добавление линка на локальный файл
    QAction *actionEditFileName;     // Редактирование информации о файле (имени файла)
    QAction *actionDeleteAttach;     // Удаление файла
    QAction *actionOpenAttach;       // Просмотр файла
    QAction *actionSaveAsAttach;     // Сохранить как... файл
    QAction *actionShowAttachInfo;   // Информация об аттаче
    QAction *actionSwitchToEditor;  // Переключение на редактор

    void setReadOnly(bool state);

  public slots:

    void setupShortcuts(void);

  protected:
    void setupActions(void);
    void setupUI();
    void setupSignals(void);
    void assembly();

    // Указатель на контроллер таблицы приаттаченных файлов
    AttachTableController *attachTableController;

    // Указатель на таблицу приаттаченных файлов (представление)
    AttachTableView *attachTableView;

    // Экранные элементы
    QToolBar *toolsLine;

};
