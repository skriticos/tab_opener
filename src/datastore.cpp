#include "datastore.h"

DataStore::DataStore(QObject *parent) : QObject(parent)
{
    this->dsDB = QSqlDatabase::addDatabase("QSQLITE");
    this->dsDB.setDatabaseName(QDir::homePath() + QDir::separator() + ".tab_opener.db");
    this->dsDB.open();

    // tblGeneral setup
    DsTable::SchemaField generalKey = {GKEY, DsTable::TEXT}; // general lookup key
    DsTable::SchemaField generalVal = {GVAL, DsTable::TEXT}; // general value
    QList<DsTable::SchemaField> generalSchema; generalSchema << generalKey << generalVal;

    this->tblGeneral = new DsTable("tblGeneral", generalSchema,this->dsDB, this);

    // tblExtension setup
    DsTable::SchemaField extStr    = {EXT_STR, DsTable::TEXT};     // extension string
    DsTable::SchemaField extActPri = {EXT_ACT_PRI, DsTable::TEXT}; // extension primary action
    DsTable::SchemaField extActSec = {EXT_ACT_SEC, DsTable::TEXT}; // extension secondary action
    QList<DsTable::SchemaField> extSchema; extSchema << extStr << extActPri << extActSec;

    this->tblExtensions = new DsTable("tblExtensions", extSchema,this->dsDB, this);

    // tblFiles setup
    DsTable::SchemaField filePath  = {PATH, DsTable::TEXT};
    QList<DsTable::SchemaField> fileSchema;
    fileSchema << filePath;

    this->tblFiles = new DsTableFav("tblFiles", fileSchema,this->dsDB, this);

    // tblCommands setup
    DsTable::SchemaField cmdCommand = {COMMAND, DsTable::TEXT};
    DsTable::SchemaField cmdPath    = {WORKING_DIRECTORY, DsTable::TEXT};
    QList<DsTable::SchemaField> cmdSchema;
    cmdSchema << cmdCommand << cmdPath;

    this->tblCommands = new DsTableFav("tblCommands", cmdSchema,this->dsDB, this);

    // tblFileNotes setup
    DsTable::SchemaField fnPath  = {PATH, DsTable::TEXT};
    DsTable::SchemaField fnNote  = {NOTE, DsTable::TEXT};
    QList<DsTable::SchemaField> fileNoteSchema;
    fileNoteSchema << fnPath << fnNote;

    this->tblFileNotes = new DsTable("tblFileNotes", fileNoteSchema,this->dsDB, this);

    // tblCommandNotes setup
    DsTable::SchemaField cnCmd  = {COMMAND, DsTable::TEXT};
    DsTable::SchemaField cnNote = {NOTE, DsTable::TEXT};
    QList<DsTable::SchemaField> commandNoteSchema;
    commandNoteSchema << cnCmd << cnNote;

    this->tblCommandNotes = new DsTable("tblCommandNotes", commandNoteSchema,this->dsDB, this);
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
    this->_updatePresets();
}

void DataStore::setCommand(QString cmd, QString path)
{
    DsTable::Record record;
    record.insert(COMMAND, cmd);
    record.insert(WORKING_DIRECTORY, path);
    this->setGeneralValue(CURRENT_COMMAND, cmd);
    tblCommands->insertRecord(record);
    this->_updateCommandHistory();
}

QString DataStore::getGeneralValue(QString key)
{
    if(tblGeneral->contains(key))
        return tblGeneral->getRecord(key).value(GVAL).toString();
    return QString();
}

void DataStore::setGeneralValue(QString key, QString value)
{
    DsTable::Record record;
    record.insert(GKEY, key);
    record.insert(GVAL, value);
    tblGeneral->insertRecord(record);
}

void DataStore::setPreset(int pos, QString path) {
    QString presetKey = "preset" + QString::number(pos);
    setGeneralValue(presetKey, path);
}

QString DataStore::getExtActPri(QString ext)
{
    if(this->tblExtensions->contains(ext))
        return this->tblExtensions->getRecord(ext).value(EXT_ACT_PRI).toString();
    return QString();
}

QString DataStore::getExtActSec(QString ext)
{
    if(this->tblExtensions->contains(ext))
        return this->tblExtensions->getRecord(ext).value(EXT_ACT_SEC).toString();
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
        return tblFileNotes->getRecord(filePath).value(NOTE).toString();
    return "";
}

QString DataStore::getCommandNote(QString command)
{
    if(command.isEmpty())
        return "";
    if(tblCommandNotes->contains(command))
        return tblCommandNotes->getRecord(command).value(NOTE).toString();
    return "";
}

void DataStore::setFileNote(QString filePath, QString note)
{
    DsTable::Record record;
    record.insert(PATH, filePath);
    record.insert(NOTE, note);
    tblFileNotes->insertRecord(record);
}

void DataStore::setCommandNote(QString command, QString note)
{
    DsTable::Record record;
    record.insert(COMMAND, command);
    record.insert(NOTE, note);
    tblCommandNotes->insertRecord(record);
}

void DataStore::_updateFileHistory()
{
    QList<DsTable::Record> recent10 = tblFiles->getRecent10();
    QList<DsTable::Record> top10    = tblFiles->getTop10();

    QList<History::Entry> recentFileHistory, popularFileHistory;

    for(int i=0; i<recent10.size(); i++){
        History::Entry entry;
        entry.filePath = recent10.at(i).value(PATH).toString();
        recentFileHistory.append(entry);
    }
    for(int i=0; i<top10.size(); i++){
        History::Entry entry;
        entry.filePath = top10.at(i).value(PATH).toString();
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
        entry.commandString = recent10.at(i).value(COMMAND).toString();
        entry.workingDirecotry = recent10.at(i).value(WORKING_DIRECTORY).toString();
        recentCommandHistory.append(entry);
    }
    for(int i=0; i<top10.size(); i++){
        History::Entry entry;
        entry.commandString = top10.at(i).value(COMMAND).toString();
        entry.workingDirecotry = top10.at(i).value(WORKING_DIRECTORY).toString();
        popularCommandHistory.append(entry);
    }
    emit this->sigUpdateCommandHistory(recentCommandHistory, popularCommandHistory);
}

void DataStore::_updatePresets()
{
    QString preset;
    QStringList presetList;

    for(int i=0; i<10; i++){
        preset = this->getPreset(i);
        if(!preset.isEmpty())
            presetList << this->getPreset(i);
    }

    emit this->sigUpdatePresets(presetList);
}

void DataStore::setExtensionValues(QString extStr, QString extActPri, QString extActSec)
{
    DsTable::Record record;
    record.insert(EXT_STR, extStr);
    record.insert(EXT_ACT_PRI, extActPri);
    record.insert(EXT_ACT_SEC, extActSec);
    tblExtensions->insertRecord(record);
}

void DataStore::setFile(QString path)
{
    DsTable::Record record;
    record.insert(PATH, path);
    tblFiles->insertRecord(record);
    this->_updateFileHistory();
}
