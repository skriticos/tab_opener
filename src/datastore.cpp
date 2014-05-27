#include "datastore.h"

DataStore::DataStore(QObject *parent) : QObject(parent)
{
    this->dsDB = QSqlDatabase::addDatabase("QSQLITE");
    this->dsDB.setDatabaseName(QDir::homePath() + QDir::separator() + ".tab_opener.db");
    this->dsDB.open();

    // tblGeneral setup
    DsTable::SchemaField generalKey = {"gkey", DsTable::TEXT}; // general lookup key
    DsTable::SchemaField generalVal = {"gval", DsTable::TEXT}; // general value
    QList<DsTable::SchemaField> generalSchema; generalSchema << generalKey << generalVal;

    this->tblGeneral = new DsTable(this);
    this->tblGeneral->initTable("tblGeneral", generalSchema, this->dsDB);

    // tblExtension setup
    DsTable::SchemaField extStr    = {"ext_str", DsTable::TEXT};     // extension string
    DsTable::SchemaField extActPri = {"ext_act_pri", DsTable::TEXT}; // extension primary action
    DsTable::SchemaField extActSec = {"ext_act_sec", DsTable::TEXT}; // extension secondary action
    QList<DsTable::SchemaField> extSchema; extSchema << extStr << extActPri << extActSec;

    this->tblExtensions = new DsTable(this);
    this->tblExtensions->initTable("tblExtensions", extSchema, this->dsDB);

    // tblFiles setup
    DsTable::SchemaField filePath  = {"path", DsTable::TEXT};
    QList<DsTable::SchemaField> fileSchema;
    fileSchema << filePath;

    this->tblFiles = new DsTableFav(this);
    this->tblFiles->initTable("tblFiles", fileSchema, this->dsDB);

    // tblCommands setup
    DsTable::SchemaField cmdCommand = {"command", DsTable::TEXT};
    DsTable::SchemaField cmdPath    = {"working_directory", DsTable::TEXT};
    QList<DsTable::SchemaField> cmdSchema;
    cmdSchema << cmdCommand << cmdPath;

    this->tblCommands = new DsTableFav(this);
    this->tblCommands->initTable("tblCommands", cmdSchema, this->dsDB);

    // tblFileNotes setup
    DsTable::SchemaField fnPath  = {"path", DsTable::TEXT};
    DsTable::SchemaField fnNote  = {"note", DsTable::TEXT};
    QList<DsTable::SchemaField> fileNoteSchema;
    fileNoteSchema << fnPath << fnNote;

    this->tblFileNotes = new DsTable(this);
    this->tblFileNotes->initTable("tblFileNotes", fileNoteSchema, this->dsDB);

    // tblCommandNotes setup
    DsTable::SchemaField cnCmd  = {"command", DsTable::TEXT};
    DsTable::SchemaField cnNote = {"note", DsTable::TEXT};
    QList<DsTable::SchemaField> commandNoteSchema;
    commandNoteSchema << cnCmd << cnNote;

    this->tblCommandNotes = new DsTable(this);
    this->tblCommandNotes->initTable("tblCommandNotes", commandNoteSchema, this->dsDB);
}

DataStore::~DataStore()
{
    this->dsDB.close();
}

// this has to be called from MainWindow after all signals and slots are connected
void DataStore::initWidgets()
{
    this->_updateFileHistory();
    this->_updateCommandHistory();
}

void DataStore::setCommand(QString cmd, QString path)
{
    DsTable::Record record;
    record.insert("command", cmd);
    record.insert("working_directory", path);
    this->setGeneralValue("current_command", cmd);
    tblCommands->insertRecord(record);
    this->_updateCommandHistory();
}

QString DataStore::getGeneralValue(QString key)
{
    if(tblGeneral->contains(key))
        return tblGeneral->getRecord(key).value("gval").toString();
    return QString();
}

void DataStore::setGeneralValue(QString key, QString value)
{
    DsTable::Record record;
    record.insert("gkey", key);
    record.insert("gval", value);
    tblGeneral->insertRecord(record);
}

void DataStore::setPreset(int pos, QString path) {
    QString presetKey = "preset" + QString::number(pos);
    setGeneralValue(presetKey, path);
}

QString DataStore::getExtActPri(QString ext)
{
    if(this->tblExtensions->contains(ext))
        return this->tblExtensions->getRecord(ext).value("ext_act_pri").toString();
    return QString();
}

QString DataStore::getExtActSec(QString ext)
{
    if(this->tblExtensions->contains(ext))
        return this->tblExtensions->getRecord(ext).value("ext_act_sec").toString();
    return QString();
}

QString DataStore::getPreset(int pos)
{
    QString presetKey = "preset" + QString::number(pos);
    return getGeneralValue(presetKey);
}

QString DataStore::getFileNote(QString filePath)
{
    if(filePath.isEmpty())
        return "";
    if(tblFileNotes->contains(filePath))
        return tblFileNotes->getRecord(filePath).value("note").toString();
    return "";
}

QString DataStore::getCommandNote(QString command)
{
    if(command.isEmpty())
        return "";
    if(tblCommandNotes->contains(command))
        return tblCommandNotes->getRecord(command).value("note").toString();
    return "";
}

void DataStore::setFileNote(QString filePath, QString note)
{
    DsTable::Record record;
    record.insert("path", filePath);
    record.insert("note", note);
    tblFileNotes->insertRecord(record);
}

void DataStore::setCommandNote(QString command, QString note)
{
    DsTable::Record record;
    record.insert("command", command);
    record.insert("note", note);
    tblCommandNotes->insertRecord(record);
}

void DataStore::_updateFileHistory()
{
    QList<DsTable::Record> recent10 = tblFiles->getRecent10();
    QList<DsTable::Record> top10    = tblFiles->getTop10();

    QList<History::Entry> recentFileHistory, popularFileHistory;

    for(int i=0; i<recent10.size(); i++){
        History::Entry entry;
        entry.filePath = recent10.at(i).value("path").toString();
        recentFileHistory.append(entry);
    }
    for(int i=0; i<top10.size(); i++){
        History::Entry entry;
        entry.filePath = top10.at(i).value("path").toString();
        popularFileHistory.append(entry);
    }

    emit this->sigUpdateFileHistory(recentFileHistory, popularFileHistory);
}

void DataStore::_updateCommandHistory()
{
    QList<DsTable::Record> recent10 = tblCommands->getRecent10();
    QList<DsTable::Record> top10    = tblCommands->getTop10();

    QList<History::Entry> recentCommandHistory, popularCommandHistory;

    for(int i=0; i<recent10.size(); i++){
        History::Entry entry;
        entry.commandString = recent10.at(i).value("command").toString();
        entry.workingDirecotry = recent10.at(i).value("working_directory").toString();
        recentCommandHistory.append(entry);
    }
    for(int i=0; i<top10.size(); i++){
        History::Entry entry;
        entry.commandString = top10.at(i).value("command").toString();
        entry.workingDirecotry = top10.at(i).value("working_directory").toString();
        popularCommandHistory.append(entry);
    }
    emit this->sigUpdateCommandHistory(recentCommandHistory, popularCommandHistory);
}

void DataStore::setExtensionValues(QString extStr, QString extActPri, QString extActSec)
{
    DsTable::Record record;
    record.insert("ext_str", extStr);
    record.insert("ext_act_pri", extActPri);
    record.insert("ext_act_sec", extActSec);
    tblExtensions->insertRecord(record);
}

void DataStore::setFile(QString path)
{
    DsTable::Record record;
    record.insert("path", path);
    tblFiles->insertRecord(record);
    this->_updateFileHistory();
}
