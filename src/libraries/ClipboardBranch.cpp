#include <QBoxLayout>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolButton>
#include <QtDebug>

#include "ClipboardBranch.h"
#include "FixedParameters.h"
#include "helpers/DebugHelper.h"

ClipboardBranch::ClipboardBranch(void) : QMimeData()  {
    branchData.branch.clear();
    branchData.record.clear();

    clipboardBranchFormat.clear();
    clipboardBranchFormat << FixedParameters::appTextId + "/branch";
}

void ClipboardBranch::print(void) const {
    // Перебираются ветки
    for (const auto & current_branch : branchData.branch) {
        // Перебираются и печатаются значения полей текущей ветки
        qDebug() << "Tree item fields:";
        for (auto f = current_branch.keyBegin(); f != current_branch.keyEnd(); ++f)
            if (*f == "id" || *f == "name")
                qDebug() << *f << ":" << current_branch.value(*f);

        // Запоминается id текущей ветки
        QString branch_id = current_branch.value("id");

        // Находятся все записи, принадлежащие текущей ветке
        for (const auto & current_record : branchData.record.values(branch_id)) {

            QMap<QString, QString> current_record_fields = current_record.getNaturalFieldList();
            for (auto f = current_record_fields.keyBegin(); f != current_record_fields.keyEnd(); ++f)
                if (*f == "id" || *f == "name")
                    qDebug() << *f << ":" << current_record_fields.value(*f);
        }
    }
}

// Получение идентификторов веток в виде, описывающим дерево
// Формат строк списка:
// ID узла 1, список подчиненных узлов
// ID узла 2, список подчиненных узлов и т.д.
QList<CLIPB_TREE_ONE_LINE> ClipboardBranch::getIdTree(void) const {
    QList<CLIPB_TREE_ONE_LINE> tree;

    // Перебираются строки с ветками
    for (const auto & current_branch : branchData.branch) {
        QString current_id = current_branch.value("id");

        // Составляется список подветок
        QStringList subbranches;
        for (const auto & current_subbranch : branchData.branch)
            if (current_subbranch.value("parent_id") == current_id)
                subbranches << current_subbranch.value("id");

        CLIPB_TREE_ONE_LINE one_line;
        one_line.branch_id = current_id;
        one_line.subbranches_id = subbranches;
        tree << one_line;
    }

    return tree;
}

void ClipboardBranch::printIdTree(void) const {
    QList<CLIPB_TREE_ONE_LINE> tree = getIdTree();

    for (const auto & one_line : tree)
        qDebug() << one_line.branch_id << one_line.subbranches_id;
}

void ClipboardBranch::addBranch(QString parent_id, QMap<QString, QString> branch_fields) {
    branch_fields["parent_id"] = parent_id;
    branchData.branch << branch_fields;
}

void ClipboardBranch::addRecord(QString branch_id, Record record) {
    /// @todo: Сделать проверку, есть ли ветка с указанным id
    branchData.record.insert(branch_id, record);
}

// Получение полей для указанной ветки
QMap<QString, QString> ClipboardBranch::getBranchFieldsById(QString id) {
    // Перебираются ветки чтобы найти ветку с нужным идентификатором
    for (const auto & current_branch : branchData.branch) {
        if (current_branch.contains("id") && current_branch["id"] == id)
            return current_branch;
    }

    criticalError("Can not find id " + id + " in clipboard data");

    return QMap<QString, QString>();
}

// Получение списка записей для указанной ветки
QList<Record> ClipboardBranch::getBranchRecords(QString id) {
    QList<Record> records;
    for (const auto & current_record : branchData.record.values(id))
        records.insert(0, current_record);

    return records;
}

QVariant ClipboardBranch::retrieveData(const QString &format, QMetaType preferredType) const {
    Q_UNUSED(preferredType)

    if (format == clipboardBranchFormat[0]) {
        QVariant v;
        v.setValue(branchData);
        return v;
    }

    return 0;
}
