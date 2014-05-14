#include "datastore.h"

DataStore::DataStore(QObject *parent) : QObject(parent)
{
    dsDB = QSqlDatabase::addDatabase("QSQLITE");
    dsDB.setDatabaseName(QDir::homePath() + QDir::separator() + ".tab_opener.db");
    dsDB.open();


    DsTable::SchemaField noteKey = {"gkey", DsTable::TEXT};
    DsTable::SchemaField noteVal = {"gval", DsTable::TEXT};
    QList<DsTable::SchemaField> noteSchema; noteSchema << noteKey << noteVal;

    this->tblGeneral = new DsTable(this);
    this->tblGeneral->initTable("tblGeneral", noteSchema, dsDB);

    if(!dsDB.tables().contains("presets")) {
        dsDB.exec("CREATE TABLE presets (id INTEGER PRIMARY KEY ASC, path TEXT)");
        for (int i=0; i<10; i++)
            dsDB.exec("INSERT INTO presets (path) VALUES ('')");
    }

    if(!dsDB.tables().contains("extensions")) {
        dsDB.exec("CREATE TABLE extensions (id INTEGER PRIMARY KEY ASC, ext TEXT, open TEXT, edit TEXT)");
    }

    if(!dsDB.tables().contains("recent_files")) {
        dsDB.exec("CREATE TABLE recent_files (id INTEGER PRIMARY KEY ASC, path TEXT)");
    }

    if(!dsDB.tables().contains("file_usage")) {
        dsDB.exec("CREATE TABLE file_usage (id INTEGER PRIMARY KEY ASC, path TEXT, usage_count INTEGER)");
    }

    if(!dsDB.tables().contains("recent_commands")){
        dsDB.exec("CREATE TABLE recent_commands (id INTEGER PRIMARY KEY ASC, command TEXT, path TEXT)");
    }

    if (!dsDB.tables().contains("command_usage")){
        dsDB.exec("CREATE TABLE command_usage "
                  "(id INTEGER PRIMARY KEY ASC, command TEXT, working_directory TEXT, usage_count INTEGER)");
    }

    loadData();
}

DataStore::~DataStore()
{
    this->saveData();
    dsDB.close();
}

void DataStore::loadData()
{
    QSqlQuery qPreset = dsDB.exec("SELECT path FROM presets");
    for (int i=0; i<10; i++){
        qPreset.next();
        this->presets[i] = qPreset.value(0).toString();
    }

    QSqlQuery qExtensions = dsDB.exec("SELECT ext, open, edit FROM extensions");
    while (qExtensions.next()){
        QString ext = qExtensions.value(0).toString();
        QString open = qExtensions.value(1).toString();
        QString edit = qExtensions.value(2).toString();
        extMap[ext] = NULL;
        openApps[ext] = open;
        editApps[ext] = edit;
    }

    // recent files
    QSqlQuery qRecentFiles = dsDB.exec("SELECT path FROM recent_files");
    while (qRecentFiles.next()) {
        QString path = qRecentFiles.value(0).toString();
        this->recentFiles.append(path);
    }

    // file usage
    QSqlQuery qFileUsage = dsDB.exec("SELECT path, usage_count FROM file_usage");
    while (qFileUsage.next()){
        QString path = qFileUsage.value(0).toString();
        int usage_count = qFileUsage.value(1).toInt();
        FileEntry *fe = new FileEntry(path);
        fe->usageCount = usage_count;
        this->fileEntryIndex[path] = fe;
        this->fileLog.append(fe);
    }

    // recent commands
    QSqlQuery qRecentCommands = dsDB.exec("SELECT command, path FROM recent_commands");
    while (qRecentCommands.next()){
        QString command = qRecentCommands.value(0).toString();
        QString path = qRecentCommands.value(1).toString();
        RunEntry *re = new RunEntry(command, path);
        this->recentCommands.append(re);
    }

    // command usage
    QSqlQuery qCommandUsage = dsDB.exec("SELECT command, working_directory, usage_count FROM command_usage");
    while (qCommandUsage.next()){
        QString cmd = qCommandUsage.value(0).toString();
        QString wd = qCommandUsage.value(1).toString();
        int uc = qCommandUsage.value(2).toInt();
        RunEntry *re = new RunEntry(cmd, wd);
        re->usageCount = uc;
        this->commandIndex[cmd] = re;
        this->commandLog.append(re);
    }
}

void DataStore::saveData()
{
    QSqlQuery qPreset(dsDB);
    qPreset.prepare("UPDATE presets SET path=:path WHERE id=:id");
    for (int i=0; i<10; i++){
        qPreset.bindValue(":path", presets[i]);
        qPreset.bindValue(":id", i+1);
        qPreset.exec();
    }

    // reset extensions table
    dsDB.exec("DROP TABLE extensions");
    dsDB.exec("CREATE TABLE extensions (id INTEGER PRIMARY KEY ASC, ext TEXT, open TEXT, edit TEXT)");
    QSqlQuery qExtensions;
    qExtensions.prepare("INSERT INTO extensions (ext, open, edit) "
                        "VALUES (:ext, :open, :edit)");
    QStringList keys = extMap.keys();
    for (int i=0; i<keys.size(); i++){
        QString ext = keys.at(i);
        qExtensions.bindValue(":ext", ext);
        qExtensions.bindValue(":open", openApps[ext]);
        qExtensions.bindValue(":edit", editApps[ext]);
        qExtensions.exec();
    }

    // recent files
    dsDB.exec("DROP TABLE recent_files");
    dsDB.exec("CREATE TABLE recent_files (id INTEGER PRIMARY KEY ASC, path TEXT)");
    QSqlQuery qRecentFiles;
    qRecentFiles.prepare("INSERT INTO recent_files (path) VALUES (:path)");
    int recentFileCount = this->getRecentFileCount();
    for (int i=0; i<recentFileCount; i++){
        qRecentFiles.bindValue(":path", this->getRecentFile(i));
        qRecentFiles.exec();
    }

    // file usage
    dsDB.exec("DROP TABLE file_usage");
    dsDB.exec("CREATE TABLE file_usage (id INTEGER PRIMARY KEY ASC, path TEXT, usage_count INTEGER)");
    QSqlQuery qFileUsage;
    qFileUsage.prepare("INSERT INTO file_usage (path, usage_count) VALUES (:path, :usage_count)");
    int fileEntryCount = this->fileEntryIndex.size();
    for (int i=0; i<fileEntryCount; i++){
        FileEntry *fe = this->fileLog.at(i);
        qFileUsage.bindValue(":path", fe->filePath);
        qFileUsage.bindValue(":usage_count", fe->usageCount);
        qFileUsage.exec();
    }

    // recent commands
    dsDB.exec("DROP TABLE recent_commands");
    dsDB.exec("CREATE TABLE recent_commands (id INTEGER PRIMARY KEY ASC, command TEXT, path TEXT)");
    QSqlQuery qRecentCommands;
    qRecentCommands.prepare("INSERT INTO recent_commands (command, path) VALUES (:command, :path)");
    for (int i=0; i<this->recentCommands.size(); i++){
        RunEntry *re = this->recentCommands.at(i);
        qRecentCommands.bindValue(":command", re->command);
        qRecentCommands.bindValue(":path", re->execPath);
        qRecentCommands.exec();
    }

    // command usage
    dsDB.exec("DROP TABLE command_usage");
    dsDB.exec("CREATE TABLE command_usage "
              "(id INTEGER PRIMARY KEY ASC, command TEXT, working_directory TEXT, usage_count INTEGER)");
    QSqlQuery qCommandUsage;
    qCommandUsage.prepare("INSERT INTO command_usage (command, working_directory, usage_count) "
                          "VALUES (:cmd, :wd, :uc)");
    int cmdCnt = this->commandIndex.size();
    for (int i=0; i < cmdCnt; i++){
        RunEntry *re = this->commandLog.at(i);
        qCommandUsage.bindValue(":cmd", re->command);
        qCommandUsage.bindValue(":wd", re->execPath);
        qCommandUsage.bindValue(":uc", re->usageCount);
        qCommandUsage.exec();
    }
}

QString DataStore::getGeneralValue(QString key)
{
    if(tblGeneral->recordExists(key))
        return tblGeneral->getRecord(key).value("gval").toString();
    return QString();
}

bool DataStore::setGeneralValue(QString key, QString value)
{
    DsTable::Record record;
    record.insert("gkey", key);
    record.insert("gval", value);
    return tblGeneral->insertRecord(record);
}

void DataStore::setPreset(int pos, QString path)
{
    this->presets[pos] = path;
}

QString DataStore::getPreset(int pos)
{
    return this->presets[pos];
}

QListWidgetItem* DataStore::getExtMapItem(QString key)
{
    return this->extMap[key];
}

int DataStore::getExtMapSize()
{
    return this->extMap.size();
}

void DataStore::setExtMapItem(QString key, QListWidgetItem *widget)
{
    this->extMap[key] = widget;
}

void DataStore::deleteExtMapItem(QString key)
{
    this->extMap.remove(key);
}

bool DataStore::extMapContains(QString key)
{
    return this->extMap.contains(key);
}

QStringList DataStore::getExtMapKeys()
{
    return extMap.keys();
}

QString DataStore::getOpenAppsItem(QString key)
{
    return this->openApps[key];
}

void DataStore::setOpenAppsItem(QString key, QString value)
{
    this->openApps[key] = value;
}

void DataStore::deleteOpenAppsItem(QString key)
{
    this->openApps.remove(key);
}

QString DataStore::getEditMapItem(QString key)
{
    return this->editApps[key];
}

void DataStore::setEditMapItem(QString key, QString value)
{
    this->editApps[key] = value;
}

void DataStore::deleteEditMapItem(QString key)
{
    this->editApps.remove(key);
}

QString DataStore::getRecentFile(int pos)
{
    return this->recentFiles.at(pos);
}

int DataStore::getRecentFileCount()
{
    return this->recentFiles.size();
}

/**
 * @brief Push a file path to the recent files stack
 * @param path
 */
void DataStore::pushRecentFile(QString path)
{
    // update recent files stack
    if(!recentFiles.contains(path)){
        if (this->recentFiles.size() > 9)
            this->recentFiles.removeLast();
    } else {
        this->recentFiles.removeOne(path);
    }
    this->recentFiles.prepend(path);

    // update usage record
    FileEntry *e;
    if (this->fileEntryIndex.contains(path)) { // file path already exists
        e = this->fileEntryIndex[path];
        e->usageCount++;
    } else { // new file path
        e = new FileEntry(path);
        this->fileEntryIndex[path] = e;
        this->fileLog.append(e);
    }

    // sort usage report
    qStableSort(this->fileLog.begin(), this->fileLog.end(), FileEntry::isMore);
}

RunEntry *DataStore::getRecentCommand(int pos)
{
    return this->recentCommands.at(pos);
}

int DataStore::getRecentCommandCount()
{
    return this->recentCommands.size();
}

void DataStore::pushRecentCommand(QString command, QString path)
{
    // we only check for command string in lookp
    // executing command in different paths still results in one recent entry
    bool contains = false;
    for (int i=0; i<this->getRecentCommandCount(); i++){
        if (this->recentCommands.at(i)->command == command) {
            contains = true;
            break;
        }
    }
    if (!contains) {
        RunEntry *re = new RunEntry(command, path);
        this->recentCommands.prepend(re);
        if (this->recentCommands.size() > 10)
            this->recentCommands.removeLast();
    }

    // update usage records
    if (this->commandIndex.contains(command)){
        this->commandIndex[command]->usageCount++;
    } else {
        RunEntry *re = new RunEntry(command, path);
        this->commandLog.prepend(re);
        this->commandIndex[command] = re;
    }

    // sort usage records
    qStableSort(this->commandLog.begin(), this->commandLog.end(), RunEntry::isMore);
}

RunEntry *DataStore::getPopularCommand(int pos)
{
    return this->commandLog.at(pos);
}

int DataStore::getPopularCommandCount()
{
    return this->commandLog.size();
}

QString DataStore::getPopularFile(int pos)
{
    return this->fileLog.at(pos)->filePath;
}

int DataStore::getPopularFileCount()
{
    return this->fileEntryIndex.size();
}

FileEntry::FileEntry(QString path)
{
    this->filePath = path;
    this->usageCount = 1;
}

bool FileEntry::isMore(FileEntry *a, FileEntry *b)
{
    return a->usageCount > b->usageCount;
}


RunEntry::RunEntry(QString command, QString path)
{
    this->command = command;
    this->execPath = path;
    this->usageCount = 1;
}

bool RunEntry::isMore(RunEntry *a, RunEntry *b)
{
    return a->usageCount > b->usageCount;
}
