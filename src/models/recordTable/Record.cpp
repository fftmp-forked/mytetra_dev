#include <QDir>
#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QRegularExpression>

#include "Record.h"

#include "libraries/FixedParameters.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/DiskHelper.h"
#include "models/appConfig/AppConfig.h"
#include "models/attachTable/AttachTableData.h"

Record::Record() : attachTableData(this) {
    liteFlag = true; // По-умолчанию объект легкий
}

Record::Record(const Record &obj) {
    // Скопировать нужно каждый кусочек класса, сами они не копируются
    liteFlag = obj.liteFlag;
    fieldList = obj.fieldList;
    text = obj.text;
    pictureFiles = obj.pictureFiles;
    attachTableData = obj.attachTableData;

    // Обратный указатель во включенном объекте должен указывать на новый экземпляр
    attachTableData.setRecord(this);
    attachTableData.updateAttachTableBackLink();
}

// На вход этой функции подается элемент <record>
void Record::setupDataFromDom(QDomElement iDomElement) {
    // Получение списка всех атрибутов текущего элемента
    QDomNamedNodeMap attList;
    attList = iDomElement.attributes();

    // Перебор атрибутов в списке и добавление их в запись
    for (int i = 0; i < attList.count(); i++) {
        QDomAttr attcurr = attList.item(i).toAttr();

        QString name = attcurr.name();
        QString value = attcurr.value();

        this->setNaturalFieldSource(name, value);

        // Распечатка считанных данных в консоль
        // qDebug() << "Read record attr " << name << value;
    }

    // Инициализируется таблица прикрепляемых файлов
    attachTableData.clear(); // Подумать, возможно эта команда не нужна
    attachTableData.setRecord(this);

    // Проверка, есть ли у переданного DOM-элемента таблица файлов для заполнения
    if (!iDomElement.firstChildElement("files").isNull())
        attachTableData.setupDataFromDom(iDomElement.firstChildElement("files")); // Заполнение таблицы приаттаченных файлов
}

QDomElement Record::exportDataToDom(QDomDocument *doc) const {
    QDomElement elem = doc->createElement("record");

    // Перебираются допустимые имена полей, доступных для сохранения
    QStringList availableFieldList = FixedParameters::recordNaturalFieldAvailableList;
    int availableFieldListSize = availableFieldList.size();
    for (int j = 0; j < availableFieldListSize; ++j) {
        QString currentFieldName = availableFieldList.at(j);

        // Устанавливается значение поля как атрибут DOM-узла
        if (isNaturalFieldExists(currentFieldName))
            elem.setAttribute(currentFieldName, getNaturalFieldSource(currentFieldName));
    }

    // К элементу записи прикрепляется элемент таблицы приаттаченных файлов, если таковые есть
    if (attachTableData.size() > 0)
        elem.appendChild(attachTableData.exportDataToDom(doc));

    return elem;
}

void Record::exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const {
    xmlWriter->writeStartElement("record");

    // Перебираются допустимые имена полей, доступных для сохранения
    QStringList availableFieldList = FixedParameters::recordNaturalFieldAvailableList;
    int availableFieldListSize = availableFieldList.size();
    for (int j = 0; j < availableFieldListSize; ++j) {
        QString currentFieldName = availableFieldList.at(j);

        // Устанавливается значение поля как атрибут записи
        if (isNaturalFieldExists(currentFieldName))
            xmlWriter->writeAttribute(currentFieldName, getNaturalFieldSource(currentFieldName));
    }

    // К элементу записи прикрепляется элемент таблицы приаттаченных файлов, если таковые есть
    if (attachTableData.size() > 0)
        attachTableData.exportDataToStreamWriter(xmlWriter);

    xmlWriter->writeEndElement(); // Закрылся record
}

bool Record::isEmpty() const {
    // Заполненная запись не может содержать пустые свойства
    if (fieldList.count() == 0)
        return true;
    else
        return false;
}

bool Record::isLite() const {
    return liteFlag;
}

void Record::switchToLite() {
    // Переключение возможно только из полновесного состояния
    if (liteFlag == true)
        criticalError("Record::switchToLite() : Record " + getIdAndNameAsString() + " already lite");

    text.clear();
    pictureFiles.clear();

    attachTableData.switchToLite();

    liteFlag = true;
}

// Метод используется только при следующих действиях:
// - добавление новой записи
// - помещение записи в буфер обмена
void Record::switchToFat() {
    // Переключение возможно только из легкого состояния
    if (liteFlag != true || text.length() > 0 || pictureFiles.count() > 0)
        criticalError("Unavailable switching record object to fat state. " + getIdAndNameAsString());

    attachTableData.switchToFat();

    liteFlag = false;
}

// Метод используется только при выводе сообщений об ошибке
QString Record::getIdAndNameAsString() const {
    QString id, name;

    if (fieldList.contains("id"))
        id = fieldList["id"];

    if (fieldList.contains("name"))
        name = fieldList["name"];

    return "Record ID: " + id + " Name: " + name;
}

// Получение значения поля
QString Record::getField(const QString &name) const {
    // Если имя поля недопустимо
    if (FixedParameters::isRecordFieldAvailable(name) == false)
        criticalError("RecordTableData::getField() : get unavailable field " + name);

    // Для настоящего поля
    if (FixedParameters::isRecordFieldNatural(name))
        return getNaturalField(name);

    // Для вычислимого поля
    if (FixedParameters::isRecordFieldCalculable(name))
        return getCalculableField(name);

    return "";
}

// Защищенная функция
// Проверка допустимости имени происходит в вызывающем коде
QString Record::getNaturalField(QString name) const {
    QString result = "";

    // Если поле с таким названием есть
    if (fieldList.contains(name)) {
        result = fieldList[name]; // Возвращается значение поля
    }

    // qDebug() << "RecordTableData::get_field : pos" << pos <<"name"<<name<<"value"<<result;

    return result;
}

// Защищенная функция
// Проверка допустимости имени происходит в вызывающем коде (необходимо для оптимизации)
QString Record::getCalculableField(QString name) const {
    // Наличие аттачей
    if (name == "hasAttach") {
        if (this->attachTableData.size() > 0)
            return "1";
        else
            return "0";
    }

    // Количество аттачей
    if (name == "attachCount")
        return QString::number(this->attachTableData.size());

    return "";
}

void Record::setField(const QString &name, const QString &value) {
    // Если имя поля недопустимо (установить значение можно только для натурального поля)
    if (FixedParameters::isRecordFieldNatural(name) == false)
        criticalError("In RecordTableData::setField() unavailable field name " + name + " try set to " + value);

    fieldList.insert(name, value);
}

bool Record::isNaturalFieldExists(const QString &name) const {
    return fieldList.contains(name);
}

QString Record::getNaturalFieldSource(QString name) const {
    // Если имя поля недопустимо
    if (FixedParameters::isRecordFieldNatural(name) == false)
        criticalError("RecordTableData::getNaturalFieldSource() : get unavailable field " + name);

    // Если поле с таким названием есть
    if (fieldList.contains(name))
        return fieldList[name]; // Возвращается значение поля
    else
        return QString();
}

void Record::setNaturalFieldSource(QString name, QString value) {
    // Если имя поля недопустимо
    if (FixedParameters::isRecordFieldNatural(name) == false)
        criticalError("In RecordTableData::setNaturalFieldSource() unavailable field name " + name + " try set to " + value);

    // Устанавливается значение поля
    fieldList.insert(name, value);
}

// Получение значений всех натуральных (невычислимых) полей
// Поля, которые могут быть у записи, но не заданы, не передаются
QMap<QString, QString> Record::getNaturalFieldList() const {
    // Список имен инфополей
    QStringList fieldNames = FixedParameters::recordNaturalFieldAvailableList;

    QMap<QString, QString> resultFieldList;

    // Перебираются названия полей
    for (int i = 0; i < fieldNames.size(); ++i) {
        QString currName = fieldNames.at(i);

        // Если поле с таким именем существует
        if (fieldList.contains(currName))
            resultFieldList[currName] = fieldList[currName];
    }

    qDebug() << "Record::getFieldList() : " << resultFieldList;

    return resultFieldList;
}

AttachTableData Record::getAttachTable() const {
    if (this->isLite() != attachTableData.isLite())
        criticalError("getAttachTable(): Unsynchro lite state for record: " + getIdAndNameAsString());

    return attachTableData;
}

AttachTableData *Record::getAttachTablePointer() {
    if (this->isLite() != attachTableData.isLite())
        criticalError("getAttachTable(): Unsynchro lite state for record: " + getIdAndNameAsString());

    return &attachTableData;
}

void Record::setAttachTable(AttachTableData iAttachTable) {
    if (this->isLite() != attachTableData.isLite())
        criticalError("setAttachTable(): Unsynchro lite state for record: " + getIdAndNameAsString());

    attachTableData = iAttachTable;
}

// Получение текста записи из памяти
QString Record::getText() const {
    // У легкого объекта невозможно запросить текст из памяти, если так происходит - это ошибка вызывающей логики
    if (liteFlag == true)
        criticalError("Can't get text from lite record object" + getIdAndNameAsString());

    return QString(text); // Текст просто преобразуется из QByteArray
}

// Получение значения текста напрямую из файла, без заполнения свойства text
QString Record::getTextDirect() const {
    // У тяжелого объекта невозможно получить текст записи из файла (у тяжелого объекта текст записи хранится в памяти)
    if (liteFlag == false)
        criticalError("Can't run Record::getTextDirect() for non lite record " + getIdAndNameAsString());

    // Выясняется полное имя файла с текстом записи
    QString fileName = getFullTextFileName();

    checkAndCreateTextFile();

    QFile f(fileName);

    // Открывается файл
    if (!f.open(QIODevice::ReadOnly))
        criticalError("File " + fileName + " not readable. Check permission.");

    qDebug() << "Record::getTextDirect() : return direct data";
    return QString::fromUtf8(f.readAll());
}

// Тип QTextDocument является унаследованным от QObject, а QObject
// нельзя передавать по значению, т.к. у него нет конструктора копирования,
// поэтому возвращаться должен указатель на QTextDocument
// std::unique_ptr<QTextDocument> Record::getTextDocument() const
QSharedPointer<QTextDocument> Record::getTextDocument() const
// QTextDocument* Record::getTextDocument() const
{
    const QString recordDirName = this->getFullDirName(); // Директория, где находится файл записи и картинки

    QString content = this->getTextDirect(); // Содержимое файла

    // Текстовый документ создается в куче, и функция вернет умный указатель на него
    QSharedPointer<QTextDocument> textDocument(new QTextDocument());
    // QTextDocument* textDocument(new QTextDocument());

    // Устанавливается URL директории документа, с помощью него будут высчитываться
    // относительные ссылки при загрузке картинок
    textDocument->setMetaInformation(QTextDocument::DocumentUrl, "file:" + recordDirName + "/");
    textDocument->setHtml(content);

    return textDocument; // Возвращается умный указатель на текстовый документ в куче
}

// Установка текста записи как свойства объекта
void Record::setText(const QString &iText) {
    // Легкому объекту невозможно установить текст, если так происходит - это ошибка вызывающей логики
    if (liteFlag == true)
        criticalError("Can't set text for lite record object" + getIdAndNameAsString());
    text = iText.toUtf8(); // Текст просто запоминается в виде QByteArray
}

// Сохранение текста записи на диск без установки текста записи как свойства
void Record::saveTextDirect(QString iText) const {
    QString fileName = getFullTextFileName();

    // Текст сохраняется в файл
    QFile wfile(fileName);

    if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
        criticalError("Can't open text file " + fileName + " for write.");

    QTextStream out(&wfile);
    out << iText;
}

// Запись текста записи, хранимого в памяти, на диск
void Record::saveText() {
    QString fileName = getFullTextFileName();
    QFile wfile(fileName);

    if (!wfile.open(QIODevice::WriteOnly))
        criticalError("Record::saveText() : Can't open binary file " + fileName + " for write.");

    // Сохраняется QByteArray с текстом записи
    wfile.write(text);
}

QMap<QString, QByteArray> Record::getPictureFiles() const {
    // У легкого объекта невозможно запросить картинки, если так происходит - это ошибка вызывающей логики
    if (liteFlag == true)
        criticalError("Can't get picture files from lite record object" + getIdAndNameAsString());

    return pictureFiles;
}

/// @todo: Переделать на копирование по ссылке
void Record::setPictureFiles(QMap<QString, QByteArray> iPictureFiles) {
    // Легкому объекту невозможно установить картики, если так происходит - это ошибка вызывающей логики
    if (liteFlag == true)
        criticalError("Can't set picture files for lite record object" + getIdAndNameAsString());

    pictureFiles = iPictureFiles;
}

// Запись "тяжелых" атрибутов (текста, картинок и приаттаченных файлов) на диск
void Record::pushFatAttributes() {
    // Легкий объект невозможно сбросить на диск, потому что он не содержит данных, сбрасываемых в файлы
    if (liteFlag == true)
        criticalError("Can't push lite record object" + getIdAndNameAsString());

    // Заполняются имена директории и полей
    // Директория при проверке создается если ее не было
    QString dirName;
    QString fileName;
    checkAndFillFileDir(dirName, fileName);

    // Запись файла с текстом записи
    saveText();

    // Если есть файлы картинок, они вставляются в конечную директорию
    if (pictureFiles.size() > 0)
        DiskHelper::saveFilesToDirectory(dirName, pictureFiles);

    // Если есть приаттаченные файлы, они вставляются в конечную директорию
    if (attachTableData.size() > 0)
        attachTableData.saveAttachFilesToDirectory(dirName);
}

// Статический метод, заменяет в переданном файле записи внутренние ссылки вида "mytetra:" согласно таблице перекодировки
// Используется при импорте, когда идентификаторы записей могут поменяться, следовательно должны поменяться и ссылки
void Record::replaceInternalReferenceByTranslateTable(QString recordFileName, QMap<QString, QString> idRecordTranslate) {
    //Считывание из файла
    QString lines;
    QFile file(recordFileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream stream(&file);
    while (!stream.atEnd())
        lines.append(stream.readLine());

    file.close();

    // Подготавливается регулярное выражение
    QRegularExpression rx("href=\"" + FixedParameters::appTextId + ":\\/\\/note/(.*)\"", QRegularExpression::InvertedGreedinessOption);

    // Заполняется список идентификаторов, встречающихся во внутренних ссылках
    QStringList internalIdList;
    auto m = rx.globalMatch(lines);
    while (m.hasNext())
        internalIdList << m.next().captured(1);

    // Перебираются найденные внутри mytetra-ссылок идентификаторы
    bool isIdReplace = false;
    foreach (QString currentId, internalIdList)
        if (idRecordTranslate.contains(currentId)) // Если идентификатор надо заменить
        {
            lines.replace("href=\"" + FixedParameters::appTextId + "://note/" + currentId + "\"",
                          "href=\"" + FixedParameters::appTextId + "://note/" + idRecordTranslate[currentId] + "\"");
            isIdReplace = true;
        }

    // Запись в файл, если были заменены идентификаторы во внутренних ссылках
    if (isIdReplace) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream writeStream(&file);
            writeStream << lines;
        }

        file.close();
    }
}

// Полное имя директории записи
QString Record::getFullDirName() const {
    if (fieldList.contains("dir") == false)
        criticalError("Record::getFullDirName() : Not present dir field");

    return AppConfig::get().get_tetradir() + "/base/" + fieldList.value("dir");
}

// Короткое имя директории записи
QString Record::getShortDirName() const {
    if (fieldList.contains("dir") == false)
        criticalError("Record::getShortDirName() : Not present dir field");

    return fieldList.value("dir");
}

// Полное имя файла с текстом записи
QString Record::getFullTextFileName() const {
    if (fieldList.contains("file") == false)
        criticalError("Record::getFullDirName() : Not present file field");

    return getFullDirName() + "/" + fieldList.value("file");
}

// Полное имя произвольного файла в каталоге записи
QString Record::getFullFileName(QString fileName) const {
    return getFullDirName() + "/" + fileName;
}

// Функция проверяет наличие полей dir и file (они используются для текста записи)
// проверяет их правильность и заполняет в переданных параметрах полные имена директории и файла
void Record::checkAndFillFileDir(QString &iDirName, QString &iFileName) {
    // Полные имена директории и файла
    iDirName = getFullDirName();
    iFileName = getFullTextFileName();

    QDir recordDir(iDirName);

    // ПРоверки на чтение и запись в этом месте бессмысленны, так как директории просто может не существовать
    // if(!recordDir.isReadable())
    //   critical_error("Record::checkAndFillFileDir() : Directory "+iDirName+" is not readable");

    // Оказывается, что у QDir нет возможности проверить доступность на запись в директорию
    // if(!recordDir.isWritable())
    //  critical_error("Record::checkAndFillFileDir() : Directory "+iDirName+" is not writable");

    // Проверяется наличие директории, куда будет вставляться файл с текстом записи
    if (!recordDir.exists()) {
        // Создается новая директория в директории base
        QDir directory(AppConfig::get().get_tetradir() + "/base");
        bool result = directory.mkdir(getShortDirName());

        if (!result)
            criticalError("Record::checkAndFillFileDir() : Can't create directory '" + getShortDirName() + "'");
    }
}

// В функцию должно передаваться полное имя файла
void Record::checkAndCreateTextFile() const {
    QString fileName = getFullTextFileName();

    QFile f(fileName);
    QFileInfo fileInfo(f);

    // Если директория с файлом не существует
    if (!fileInfo.absoluteDir().exists()) {
        QString messageText = QObject::tr("The database is not consistent.\nThe directory %1 is not found.\nMyTetra will try to create a blank entry to fix it.").arg(fileInfo.absoluteDir().absolutePath());

        qWarning() << messageText;

        // Выводится уведомление что будет создана пустая директория записи
        QMessageBox msgBox;
        msgBox.setWindowTitle(QObject::tr("Warning!"));
        msgBox.setText(messageText);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();

        // Создается пустая директория записи
        QDir tempDir("/");
        tempDir.mkpath(fileInfo.absoluteDir().absolutePath());
    }

    // Если файл записи не существует
    if (!f.exists()) {
        QString messageText = QObject::tr("The database is not consistent.\nThe file %1 is not found.\nMyTetra will try to create a blank entry to fix it.").arg(fileName);

        qWarning() << messageText;

        // Выводится уведомление что будет создан пустой текст записи
        QMessageBox msgBox;
        msgBox.setWindowTitle(QObject::tr("Warning!"));
        msgBox.setText(messageText);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();

        // Создается пустой текст записи
        saveTextDirect(QString());
    }
}
