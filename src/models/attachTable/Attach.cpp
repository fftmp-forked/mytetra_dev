#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "Attach.h"
#include "AttachTableData.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/DiskHelper.h"
#include "libraries/helpers/MessageHelper.h"
#include "models/recordTable/Record.h"

// Конструктор прикрепляемого файла
Attach::Attach(AttachTableData *iParentTable) {
    init(iParentTable);
}

/// @brief Конструктор прикрепляемого файла с указанием типа прикрепления
Attach::Attach(Type type, AttachTableData *iParentTable) {
    setField("type", type == Type::file ? "file" : "link");

    init(iParentTable);
}

Attach::~Attach() {
    fileContent.clear();
}

void Attach::init(AttachTableData *iParentTable) {
    liteFlag = true; // По-умолчанию легкий объект
    parentTable = iParentTable;
    fileContent.clear();
}

void Attach::setParentTable(AttachTableData *iParentTable) {
    parentTable = iParentTable;
}

/// @brief Допустимые имена полей
QStringList Attach::fieldAvailableList(void) {
    return QStringList() << "id"
                         << "fileName"
                         << "link"
                         << "type";
}

// На вход метода подается тег <file>
void Attach::setupDataFromDom(QDomElement iDomElement) {
    for(const auto & fieldName : fieldAvailableList())        // Перебираются имена полей (XML-тегов)
        fields[fieldName] = iDomElement.attribute(fieldName); // Напрямую устанавливаются значения из XML файла
}

QDomElement Attach::exportDataToDom(QDomDocument *doc) const {
    QDomElement elem = doc->createElement("file");

    for(const auto & fieldName : fieldAvailableList()) // Перебираются имена полей (XML-тегов)
        if (fields[fieldName].size() > 0)
            elem.setAttribute(fieldName, fields[fieldName]);

    return elem;
}

void Attach::exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const {
    xmlWriter->writeStartElement("file");

    for(const auto & fieldName : fieldAvailableList()) // Перебираются имена полей (XML-тегов)
        if (fields[fieldName].size() > 0)
            xmlWriter->writeAttribute(fieldName, fields[fieldName]);

    xmlWriter->writeEndElement(); // Закрылся file
}

bool Attach::isEmpty() const {
    // Заполненный аттач не может содержать пустой id
    return (!fields.contains("id") || getField("id").length() == 0);
}

void Attach::switchToLite() {
    // Переключение возможно только из полновесного состояния
    if (liteFlag == true)
        criticalError("Can't switch attach to lite state. Attach id: " + getField("id") + " File name: " + getField("fileName"));

    fileContent.clear();

    liteFlag = true;
}

void Attach::switchToFat() {
    // Переключение возможно только из легкого состояния
    if (liteFlag != true)
        criticalError("Unavailable switching attach object to fat state. Attach Id: " + getField("id") + " File name: " + getField("fileName"));

    liteFlag = false;
}

// Получение значения поля
QString Attach::getField(QString name) const {
    // Если имя поля недопустимо
    if (fieldAvailableList().contains(name) == false)
        criticalError("Attach::getField() : get unavailable field " + name);

    // ------------------------------------------
    // Проверки и действия для разных типов полей
    // ------------------------------------------

    // Если запрашивается линк на файл
    if (name == "link")
        if (fields["type"] != "link") // И тип аттача не является линком
            criticalError("Attach::getField() : Can't get link from non-link attach.");

    return fields[name]; // Возвращается значение поля
}

// Установка значения поля
void Attach::setField(QString name, QString value) {
    // Если имя поля недопустимо
    if (fieldAvailableList().contains(name) == false)
        criticalError("Attach::setField() : set unavailable field " + name);

    // ------------------------------------------
    // Проверки и действия для разных типов полей
    // ------------------------------------------

    // Поле с именем файла
    if (name == "fileName")
        if (getField("type") == "link")                                  // Если устанавливается имя файла для линка
            if (getField("fileName").length() > 0 && value.length() > 0) // Если имя уже было задано (при создании аттача), и новое имя не пустое
            {
                // Имя файла для линка менять нельзя
                showMessageBox(QObject::tr("Unable to rename a file which attached as a link."));
                return;
            }

    // Поле со ссылкой на файл (содержит путь к файлу, на который указывает линк)
    if (name == "link") {
        QFile tempFile(value);

        // Если файла, на который ссылается линк, не существует
        if (!tempFile.exists()) {
            showMessageBox(QObject::tr("Bad link. File not found."));
            return;
        }
    }

    // Устанавливается значение поля
    fields.insert(name, value);
}

void Attach::pushFatDataToDisk() {
    if (getField("type") != "file")
        criticalError("Can't push fat data for non-file attach.");

    if (liteFlag == true)
        criticalError("Can't push fat data for lite attach. Attach id: " + getField("id") + " File name: " + getField("fileName"));

    QString innerFileName = getInnerFileName();
    QString innerDirName = parentTable->record->getFullDirName();

    QMap<QString, QByteArray> fileList;
    fileList[innerFileName] = fileContent;
    DiskHelper::saveFilesToDirectory(innerDirName, fileList);
}

// То же самое что и pushFatDataToDisk, только в нужную директорию
void Attach::pushFatDataToDirectory(QString dirName) {
    if (getField("type") != "file")
        criticalError("Can't save to directory " + dirName + " non-file attach.");

    if (liteFlag == true)
        criticalError("Can't save to directory lite attach. Attach id: " + getField("id") + " File name: " + getField("fileName"));

    QMap<QString, QByteArray> fileList;
    fileList[getInnerFileName()] = fileContent;
    DiskHelper::saveFilesToDirectory(dirName, fileList);
}

// Втаскивание в объект содержимого файла с диска
void Attach::popFatDataFromDisk() {
    // Втаскивание возможно только в полновесном состоянии
    if (liteFlag == true)
        criticalError("Can't' pop data for lite attach. Attach id: " + getField("id") + " File name: " + getField("fileName"));

    fileContent.clear();

    QString innerFileName = getInnerFileName();
    QString innerDirName = parentTable->record->getFullDirName();

    fileContent.append((DiskHelper::getFilesFromDirectory(innerDirName, innerFileName)).value(innerFileName));
}

// fullFileName - имя файла с полным путем, который нужно скопировать в каталог записи
bool Attach::copyFileToBase(QString iFileName) {
    qDebug() << "Before check file. iFileName: " + iFileName;
    qDebug() << "Inner file name: " + getFullInnerFileName();

    // Проверка наличия исходного файла
    QFile file(iFileName);

    qDebug() << "Check file finish";

    if (file.exists() == false) {
        showMessageBox(QObject::tr("Can't open file %1. File not exists.").arg(iFileName));
        return false;
    }

    qDebug() << "Before real copy file.";

    bool result = file.copy(getFullInnerFileName());

    qDebug() << "After real copy file.";

    if (result == false)
        showMessageBox(QObject::tr("Can't copy file %1. May be directory %2 not writable, or target file %3 already exists.")
                           .arg(iFileName, getFullInnerDirName(), getFullInnerFileName()));

    return result;
}

// Удаление файла с диска
void Attach::removeFile() {
    if (getField("type") != "file")
        return;

    // Проверка наличия файла
    QFile file(getFullInnerFileName());

    if (file.exists() == false) {
        showMessageBox(QObject::tr("Unable to delete the file %1 from disk: file not found.").arg(getFullInnerFileName()));
        return;
    }

    // Выставляются права чтобы удаление прошло корректно
    file.setPermissions(file.permissions() | QFile::ReadUser | QFile::WriteUser);

    // Непосредственно удаление
    file.remove();
}

QString Attach::constructFileName(const QString type, const QString id, const QString fileName) {
    // Для файла
    if (type == "file") {
        if (fileName.startsWith(".")) { // Если это dot-файл
            return "." + id;
        } else if (fileName.endsWith(".")) { // Если в имени файла точка в конце
            return id + ".";
        } else {
            // Выясняется расширение по видимому имени файла
            QFileInfo fileInfo(fileName);
            QString suffix = fileInfo.suffix();

            if (suffix.length() > 0) {
                return id + "." + suffix; // Если расширение есть
            } else {
                return id; // Без расширения
            }
        }
    }

    // Для линка просто возвращается имя файла, куда указывает линк
    if (type == "link")
        return fileName;

    criticalError("Bad attach type in getInnerFileName():" + type);

    return "";
}

// Внутрисистемное имя файла (без пути)
// Внутрисистемное имя складывается из идентификатора аттача и расширения, взятого от имени файла
QString Attach::getInnerFileName() const {
    return constructFileName(getField("type"), getField("id"), getField("fileName"));
}

// Внутрисистемное имя файла с путем
QString Attach::getFullInnerFileName() const {
    if (getField("type") == "file") // Для файла
    {
        QString resultFileName = getFullInnerDirName() + "/" + getInnerFileName();
        return resultFileName;
    }

    if (getField("type") == "link") // Для линка
        return getField("link");

    criticalError("Bad attach type in getFullInnerFileName():" + getField("type"));

    return "";
}

// Внутрисистемное имя файла с абсолютным путем
QString Attach::getAbsoluteInnerFileName() const {
    QFileInfo fileInfo(getFullInnerFileName());

    return fileInfo.absoluteFilePath();
}

// Внутрисистемный путь к файлу (полный)
QString Attach::getFullInnerDirName() const {
    return parentTable->record->getFullDirName();
}

// Размер аттача в байтах
qint64 Attach::getFileSize() const {
    QString tempFileName;

    if (fields["type"] == "file") {
        QString recordDir = parentTable->record->getFullDirName();
        tempFileName = recordDir + "/" + getInnerFileName();
    }

    if (fields["type"] == "link")
        tempFileName = getField("link");

    QFile tempFile(tempFileName);

    if (!tempFile.exists())
        return 0;
    else
        return tempFile.size();
}

void Attach::renameFile(QString newFileName) {
    if (newFileName.length() == 0) {
        showMessageBox(QObject::tr("Invalid empty file name."));
        return;
    }

    QString type = getField("type");
    if (type == "link") {
        // Имя файла для линка менять нельзя
        showMessageBox(QObject::tr("Unable to rename a file which attached as a link."));
        return;
    }

    QFile file(getFullInnerFileName());

    // Подправляются права, чтобы файл под новым именем нормально читался
    file.setPermissions(file.permissions() | QFile::ReadUser | QFile::WriteUser);

    if (!file.exists()) {
        showMessageBox(QObject::tr("Unable to rename the file %1 from disk: file not found.").arg(getFullInnerFileName()));
        return;
    }

    QString resultFileName = getFullInnerDirName() + "/" + constructFileName(type, getField("id"), newFileName);

    file.rename(resultFileName);

    setField("fileName", newFileName);
}
