#include "datastore.h"

DataStore::DataStore(QObject *parent) : QObject(parent)
{
    this->dsDB = QSqlDatabase::addDatabase("QSQLITE");
    this->dsDB.setDatabaseName(QDir::homePath() + QDir::separator() + ".tab_opener.db");
    this->dsDB.open();

    // tblGeneral setup
    DsTable::SchemaField generalKey = {GKEY, DsTable::TEXT}; // general lookup key
    DsTable::SchemaField generalVal = {GVAL, DsTable::TEXT}; // general value
    QList<DsTable::SchemaField> generalSchema;
    generalSchema << generalKey << generalVal;

    this->tblGeneral = new DsTable("tblGeneral", generalSchema, this->dsDB, this);

    // tblExtension setup
    DsTable::SchemaField extStr    = {EXT_STR, DsTable::TEXT};     // extension string
    DsTable::SchemaField extActPri = {EXT_ACT_PRI, DsTable::TEXT}; // extension primary action
    DsTable::SchemaField extActSec = {EXT_ACT_SEC, DsTable::TEXT}; // extension secondary action
    QList<DsTable::SchemaField> extSchema;
    extSchema << extStr << extActPri << extActSec;

    this->tblExtensions = new DsTable("tblExtensions", extSchema, this->dsDB, this);

    // tblFiles setup
    DsTable::SchemaField filePath  = {PATH, DsTable::TEXT};
    QList<DsTable::SchemaField> fileSchema;
    fileSchema << filePath;

    this->tblFiles = new DsTableFav("tblFiles", fileSchema, this->dsDB, this);

    // tblCommands setup
    DsTable::SchemaField cmdCommand = {COMMAND, DsTable::TEXT};
    DsTable::SchemaField cmdPath    = {WORKING_DIRECTORY, DsTable::TEXT};
    QList<DsTable::SchemaField> cmdSchema;
    cmdSchema << cmdCommand << cmdPath;

    this->tblCommands = new DsTableFav("tblCommands", cmdSchema, this->dsDB, this);

    // tblFileNotes setup
    DsTable::SchemaField fnPath  = {PATH, DsTable::TEXT};
    DsTable::SchemaField fnNote  = {NOTE, DsTable::TEXT};
    QList<DsTable::SchemaField> fileNoteSchema;
    fileNoteSchema << fnPath << fnNote;

    this->tblFileNotes = new DsTable("tblFileNotes", fileNoteSchema, this->dsDB, this);

    // tblCommandNotes setup
    DsTable::SchemaField cnCmd  = {COMMAND, DsTable::TEXT};
    DsTable::SchemaField cnNote = {NOTE, DsTable::TEXT};
    QList<DsTable::SchemaField> commandNoteSchema;
    commandNoteSchema << cnCmd << cnNote;

    this->tblCommandNotes = new DsTable("tblCommandNotes", commandNoteSchema, this->dsDB, this);
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

    _initFileBrowserWidget();
}

void DataStore::slotCfgPresetsChanged(QStringList presetList)
{
    for(int i = 0; i < presetList.size(); i++) {
        _setGeneralValue("preset" + QString::number(i), presetList.at(i));
    }
    _updatePresets();
}

void DataStore::slotCfgExtensionChanged(Config::ExtensionEntry extEntry)
{
    DsTable::Record record;
    record.insert(EXT_STR, extEntry.extName);
    record.insert(EXT_ACT_PRI, extEntry.extActPri);
    record.insert(EXT_ACT_SEC, extEntry.extActSec);
    tblExtensions->insertRecord(record);
}

void DataStore::slotCfgExtensionDeleted(QString extStr)
{
    tblExtensions->deleteRecord(extStr);
}

void DataStore::slotCfgTerminalChanged(QString terminalCmd)
{
    _setGeneralValue(TERMINAL_EMULATOR, terminalCmd);
}

void DataStore::slotCfgExtFileBrowserChanged(QString fbrowserCmd)
{
    _setGeneralValue(FILE_BROWSER, fbrowserCmd);
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

void DataStore::slotFileNoteChanged(QString filePath, QString noteText)
{
    DsTable::Record record;
    record.insert(PATH, filePath);
    record.insert(NOTE, noteText);
    this->tblFileNotes->insertRecord(record);
}

void DataStore::slotCmdNoteChanged(QString cmdStr, QString noteText)
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

    if(tblCommandNotes->contains(cmdStr)) {
        noteText = tblCommandNotes->getRecord(cmdStr).value(NOTE).toString();
    }

    emit this->sigCmdSelectionChanged(cmdStr, noteText);
}

// triggered when file selection is changed
// updates file notes (NotesWidget)
void DataStore::slotSelectedFileChanged(QString filePath)
{
    QString noteText;

    if(tblFileNotes->contains(filePath)) {
        noteText = tblFileNotes->getRecord(filePath).value(NOTE).toString();
    }

    _setGeneralValue(SELECTED_FILE, filePath);
    emit this->sigFileSelectionChanged(filePath, noteText);
}

void DataStore::slotSelectedFolderChanged(QString newFolderPath)
{
    _setGeneralValue(NAVIGATOR_PATH, newFolderPath);
}

void DataStore::slotNoteSelectionChanged(QString newSelection)
{
    this->_setGeneralValue(LAST_NOTE_STATE, newSelection);
}

void DataStore::slotOpenExtApp(FileOpen::ExtApp extApp, QString folderPath)
{
    if(extApp == FileOpen::TERMINAL) {
        Util::execDetachedCommand(_getGeneralValue(TERMINAL_EMULATOR) + " " + "\"" + folderPath + "\"");
    }

    if(extApp == FileOpen::FILEBROWSER) {
        Util::execDetachedCommand(_getGeneralValue(FILE_BROWSER) + " " + "\"" + folderPath + "\"");
    }

    emit this->sigFileOrExtAppOpened();
}

void DataStore::slotOpenFile(FileOpen::OpenType openType, QString filePath)
{
    QString openCommand;
    QString extension = filePath.split(".").last();

    if(!this->tblExtensions->contains(extension)) {
        emit this->sigInvalidExtension();
        return;
    }

    if(openType == FileOpen::PRIMARY) {
        openCommand = this->tblExtensions->getRecord(extension).value(EXT_ACT_PRI).toString();
    } else if(openType == FileOpen::SECONDARY) {
        openCommand = this->tblExtensions->getRecord(extension).value(EXT_ACT_SEC).toString();
    }

    Util::execDetachedCommand(openCommand + " " + "\"" + filePath + "\"");

    DsTable::Record record;
    record.insert(PATH, filePath);
    tblFiles->insertRecord(record);

    _updateFileHistory();
    emit this->sigFileOrExtAppOpened();
}

QString DataStore::_getGeneralValue(QString key)
{
    if(tblGeneral->contains(key)) {
        return tblGeneral->getRecord(key).value(GVAL).toString();
    }

    return QString();
}

void DataStore::_setGeneralValue(QString key, QString value)
{
    DsTable::Record record;
    record.insert(GKEY, key);
    record.insert(GVAL, value);
    tblGeneral->insertRecord(record);
}

void DataStore::_initConfig()
{
    QStringList presetList;
    QList<Config::ExtensionEntry> extList;
    QString terminalCmd;
    QString fbrowserCmd;

    for(int i = 0; i < 10; i++) {
        if(tblGeneral->contains("preset" + QString::number(i))) {
            presetList << _getGeneralValue("preset" + QString::number(i));
        } else {
            break;
        }
    }

    QStringList extKeys = this->tblExtensions->getRecordKeys();

    for(QString extKey : extKeys) {
        DsTable::Record record = this->tblExtensions->getRecord(extKey);
        Config::ExtensionEntry extEntry;
        extEntry.extName = record.value(EXT_STR).toString();
        extEntry.extActPri = record.value(EXT_ACT_PRI).toString();
        extEntry.extActSec = record.value(EXT_ACT_SEC).toString();
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

    for(int i = 0; i < recent10.size(); i++) {
        History::Entry entry;
        entry.filePath = recent10.at(i).value(PATH).toString();
        recentFileHistory.append(entry);
    }

    for(int i = 0; i < top10.size(); i++) {
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

    for(int i = 0; i < recent10.size(); i++) {
        History::Entry entry;
        entry.commandString = recent10.at(i).value(COMMAND).toString();
        entry.workingDirecotry = recent10.at(i).value(WORKING_DIRECTORY).toString();
        recentCommandHistory.append(entry);
    }

    for(int i = 0; i < top10.size(); i++) {
        History::Entry entry;
        entry.commandString = top10.at(i).value(COMMAND).toString();
        entry.workingDirecotry = top10.at(i).value(WORKING_DIRECTORY).toString();
        popularCommandHistory.append(entry);
    }

    emit this->sigUpdateCommandHistory(recentCommandHistory, popularCommandHistory);
}

void DataStore::_updatePresets()
{
    QStringList presetList;

    for(int i = 0; i < 10; i++) {
        presetList << _getGeneralValue("preset" + QString::number(i));
    }

    emit this->sigUpdatePresets(presetList);
}

void DataStore::_initCommandWidget()
{
    if(this->tblGeneral->contains(CURRENT_COMMAND)) {
        emit this->sigInitCommand(this->_getGeneralValue(CURRENT_COMMAND));
    }
}

void DataStore::_initFileBrowserWidget()
{
    emit this->sigFbInitLocation(_getGeneralValue(NAVIGATOR_PATH), _getGeneralValue(SELECTED_FILE));
}

// setup NotesWidget
// only explicitly push last state and global notes
// file and command notes are pushed through a roundtrip of the
// CommandWidget and FileBrowserWidget when they are set up
void DataStore::_initNoteWidget()
{
    if(this->tblGeneral->contains(LAST_NOTE_STATE)) {
        emit this->sigInitNotesSelection(this->_getGeneralValue(LAST_NOTE_STATE));
    } else {
        emit this->sigInitNotesSelection(GLOBAL);
    }

    if(this->tblGeneral->contains(NOTES)) {
        emit this->sigInitGlobalNotes(this->_getGeneralValue(NOTES));
    }
}
