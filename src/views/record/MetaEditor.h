#pragma once

#include <QLabel>
#include <QWidget>

#include "../../libraries/wyedit/Editor.h"
#include "../findInBaseScreen/FindScreen.h"

class QHBoxLayout;
class QGridLayout;
class QScrollArea;
class QSplitter;
class AttachTableScreen;

class MetaEditor : public Editor {
    Q_OBJECT

  public slots:

    void setField(QString n, QString v);
    void clearAll();
    void onClickToTag(const QString &text);

  signals:
    // Сигнал вырабатывается при клике на текстовой метке
    void setFindTextSignal(QString text);

  public:
    MetaEditor(QWidget *parent = nullptr);

    void setName(QString name);
    void setAuthor(QString author);
    void setUrl(QString url);
    void setTags(QString tags);
    void setFindScreenSignal(FindScreen *findScreenObj);

    static void toAttachCallback();

    void switchToEditorLayout();
    void switchToAttachLayout();

    void setReadOnly(bool state);

    void setFocusToBaseWidget();

  private:
    void setupLabels();
    void setupUI();
    void metaAssembly();

    QLabel *recordName;   // Надпись Title
    QLabel *recordAuthor; // Надпись Author(s)

    QLabel *labelUrl; // Надпись "Url"
    QLabel *recordUrl;

    QLabel *labelTags; // Надпись "Tags"
    QWidget *recordTagsContainer;
    QHBoxLayout *recordTagsLayout;
    QScrollArea *recordTagsScrollArea;
    QString recordTagsText;
    QStringList recordTagsTextList;
    QList<QLabel *> recordTagsLabels;

    QSplitter *editorAndFileTableSplitter;

    // Виджет слоя прикрепляемых файлов
    AttachTableScreen *attachTableScreen;

    // Виджет слоя редактирования текста
    QGridLayout *editorMainLayer;
    QWidget *editorMainScreen;

    // Группировалка виджетов всех слоев (слоя редактирования и слоя прикрепляемых файлов)
    QVBoxLayout *metaEditorJoinLayer;
};
