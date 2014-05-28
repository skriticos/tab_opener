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
void DataStore::initWidgetData()
{
    _initConfig();

    // the history and preset widget don't send out signals on startup, we init them first
    _updateFileHistory();
    _updateCommandHistory();
    _updatePresets();

    // the notes widget init just sets the last state and global text, no signals emitted
    _initNoteWidget();

    // the command widget emits the command changed signal on startup, which is
    // propagated to notes and command history
    _initCommandWidget();
}

void DataStore::slotCfgPresetsChanged(QStringList presetList)
{

}

void DataStore::slotCfgExtensionChanged(Config::ExtensionEntry extEntry)
{

}

void DataStore::slotCfgExtensionDeleted(QString extStr)
{

}

void DataStore::slotCfgTerminalChanged(QString terminalCmd)
{

}

void DataStore::slotCfgExtFileBrowserChanged(QString fbrowserCmd)
{

}

QString DataStore::_getGeneralValue(QString key)
{
    if(tblGeneral->contains(key))
        return tblGeneral->getRecord(key).value(GVAL).toString();
    return QString();
}

void DataStore::_setGeneralValue(QString key, QString value)
{
    DsTable::Record record;
    record.insert(GKEY, key);
    record.insert(GVAL, value);
    tblGeneral->insertRecord(record);
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

void DataStore::slotCommandExecuted(QString commandString, QString workingDirectory)
{
    DsTable::Record record;
    record.insert(COMMAND, commandString);
    record.insert(WORKING_DIRECTORY, workingDirectory);
    tblCommands->insertRecord(record);
    this->_updateCommandHistory();
}

void DataStore::slotGlobalNoteChanged(QString noteText)
{
    this->_setGeneralValue(NOTES, noteText);
}

void DataStore::slotFileNoteChanged(QString noteText, QString filePath)
{
    DsTable::Record record;
    record.insert(PATH, filePath);
    record.insert(NOTE, noteText);
    this->tblFileNotes->insertRecord(record);
}

void DataStore::slotCmdNoteChanged(QString noteText, QString cmdStr)
{
    DsTable::Record record;
    record.insert(COMMAND, cmdStr);
    record.insert(NOTE, noteText);
    this->tblCommandNotes->insertRecord(record);
}

// triggered when command entry is changed
// updates command notes (NotesWidget)
void DataStore::slotCmdChanged(QString cmdStr)
{
    QString noteText;

    this->_setGeneralValue(CURRENT_COMMAND, cmdStr);

    if(tblCommandNotes->contains(cmdStr))
        noteText = tblCommandNotes->getRecord(cmdStr).value(NOTE).toString();

    emit this->sigCmdSelectionChanged(cmdStr, noteText);
}

// triggered when file selection is changed
// updates file notes (NotesWidget)
void DataStore::slotSelectedFileChanged(QString filePath)
{
    QString noteText;

    if(tblFileNotes->contains(filePath))
        noteText = tblFileNotes->getRecord(filePath).value(NOTE).toString();

    emit this->sigFileSelectionChanged(filePath, noteText);
}

void DataStore::slotNoteSelectionChanged(QString newSelection)
{
    this->_setGeneralValue(LAST_NOTE_STATE, newSelection);
}

void DataStore::_initConfig()
{
    QStringList presetList;
    QList<Config::ExtensionEntry> extList;
    QString terminalCmd;
    QString fbrowserCmd;

    for(int i=0; i<10; i++){
        if(tblGeneral->contains("preset" + QString::number(i))){
            presetList << _getGeneralValue("preset" + QString::number(i));
        } else {
            break;
        }
    }

    QStringList extKeys = this->tblExtensions->getRecordKeys();
    for(QString extKey : extKeys){
        DsTable::Record record = this->tblExtensions->getRecord(extKey);
        Config::ExtensionEntry extEntry = {
            record.value(EXT_STR),
            record.value(EXT_ACT_PRI),
            record.value(EXT_ACT_SEC)
        };
        extList << extEntry;
    }

    terminalCmd = _getGeneralValue(TERMINAL_EMULATOR);
    fbrowserCmd = _getGeneralValue(FILE_BROWSER);

    emit this->sigInitConfig(presetList, extList, terminalCmd, fbrowserCmd);
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

void DataStore::_initCommandWidget()
{
    if(this->tblGeneral->contains(CURRENT_COMMAND)){
        emit this->sigInitCommand(this->_getGeneralValue(CURRENT_COMMAND));
    }
}

// setup NotesWidget
// only explicitly push last state and global notes
// file and command notes are pushed through a roundtrip of the
// CommandWidget and FileBrowserWidget when they are set up
void DataStore::_initNoteWidget()
{
    if(this->tblGeneral->contains(LAST_NOTE_STATE))
        emit this->sigInitNotesSelection(this->_getGeneralValue(LAST_NOTE_STATE));
    if(this->tblGeneral->contains(NOTES))
        emit this->sigInitGlobalNotes(this->_getGeneralValue(NOTES));
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
