#include "datastore.h"

DataStore::DataStore(QObject *parent) : QObject(parent)
{
    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName(QDir::homePath() + QDir::separator() + ".tab_opener.db");
    myDB.open();

    if(!myDB.tables().contains("presets")) {
        myDB.exec("CREATE TABLE presets (id INTEGER PRIMARY KEY ASC, path TEXT)");
        for (int i=0; i<10; i++)
            myDB.exec("INSERT INTO presets (path) VALUES ('')");
    }
    if(!myDB.tables().contains("notes")) {
        myDB.exec("CREATE TABLE notes (id INTEGER PRIMARY KEY ASC, note TEXT)");
        myDB.exec("INSERT INTO notes (note) VALUES ('notes')");
    }
    if(!myDB.tables().contains("general")) {
        myDB.exec("CREATE TABLE general (id INTEGER PRIMARY KEY ASC, key TEXT, value TEXT)");
        myDB.exec("INSERT INTO general (key, value) VALUES ('fileBrowser', '/usr/bin/dolphin')");
        myDB.exec("INSERT INTO general (key, value) VALUES ('terminalEmulator', '/usr/bin/konsole --workdir')");
    }

    if(!myDB.tables().contains("extensions")) {
        myDB.exec("CREATE TABLE extensions (id INTEGER PRIMARY KEY ASC, ext TEXT, open TEXT, edit TEXT)");
    }

    if(!myDB.tables().contains("recent_files")) {
        myDB.exec("CREATE TABLE recent_files (id INTEGER PRIMARY KEY ASC, path TEXT)");
    }

    if(!myDB.tables().contains("file_usage")) {
        myDB.exec("CREATE TABLE file_usage (id INTEGER PRIMARY KEY ASC, path TEXT, usage_count INTEGER)");
    }

    loadData();
}

DataStore::~DataStore()
{
    this->saveData();
    myDB.close();
}

void DataStore::loadData()
{
    QSqlQuery qPreset = myDB.exec("SELECT path FROM presets");
    for (int i=0; i<10; i++){
        qPreset.next();
        this->presets[i] = qPreset.value(0).toString();
    }
    QSqlQuery qNotes = myDB.exec("SELECT note FROM notes");
    qNotes.next();
    this->notes = qNotes.value(0).toString();

    QSqlQuery qGeneral = myDB.exec("SELECT key, value FROM general");
    while(qGeneral.next()){
        if(qGeneral.value(0).toString() == "fileBrowser")
            this->fileBrowser = qGeneral.value(1).toString();
        if(qGeneral.value(0).toString() == "terminalEmulator")
            this->terminalEmulator = qGeneral.value(1).toString();
    }

    QSqlQuery qExtensions = myDB.exec("SELECT ext, open, edit FROM extensions");
    while (qExtensions.next()){
        QString ext = qExtensions.value(0).toString();
        QString open = qExtensions.value(1).toString();
        QString edit = qExtensions.value(2).toString();
        extMap[ext] = NULL;
        openApps[ext] = open;
        editApps[ext] = edit;
    }

    // recent files
    QSqlQuery qRecentFiles = myDB.exec("SELECT path FROM recent_files");
    while (qRecentFiles.next()) {
        QString path = qRecentFiles.value(0).toString();
        this->recentFiles.append(path);
    }

    // file usage
    QSqlQuery qFileUsage = myDB.exec("SELECT path, usage_count FROM file_usage");
    while (qFileUsage.next()){
        QString path = qFileUsage.value(0).toString();
        int usage_count = qFileUsage.value(1).toInt();
        FileEntry *fe = new FileEntry(path);
        fe->usageCount = usage_count;
        this->fileEntryIndex[path] = fe;
        this->fileLog.append(fe);
    }
}

void DataStore::saveData()
{
    QSqlQuery qPreset(myDB);
    qPreset.prepare("UPDATE presets SET path=:path WHERE id=:id");
    for (int i=0; i<10; i++){
        qPreset.bindValue(":path", presets[i]);
        qPreset.bindValue(":id", i+1);
        qPreset.exec();
    }
    QSqlQuery qNotes(myDB);
    qNotes.prepare("UPDATE notes SET note=:note WHERE id=1");
    qNotes.bindValue(":note", notes);
    qNotes.exec();

    QSqlQuery qGeneral(myDB);
    qGeneral.prepare("UPDATE general SET value=:value WHERE key=:key");
    qGeneral.bindValue(":key", "fileBrowser");
    qGeneral.bindValue(":value", this->fileBrowser);
    qGeneral.exec();
    qGeneral.prepare("UPDATE general SET value=:value WHERE key=:key");
    qGeneral.bindValue(":key", "terminalEmulator");
    qGeneral.bindValue(":value", this->terminalEmulator);
    qGeneral.exec();

    // reset extensions table
    myDB.exec("DROP TABLE extensions");
    myDB.exec("CREATE TABLE extensions (id INTEGER PRIMARY KEY ASC, ext TEXT, open TEXT, edit TEXT)");
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
    myDB.exec("DROP TABLE recent_files");
    myDB.exec("CREATE TABLE recent_files (id INTEGER PRIMARY KEY ASC, path TEXT)");
    QSqlQuery qRecentFiles;
    qRecentFiles.prepare("INSERT INTO recent_files (path) VALUES (:path)");
    int recentFileCount = this->getRecentFileCount();
    for (int i=0; i<recentFileCount; i++){
        qRecentFiles.bindValue(":path", this->getRecentFile(i));
        qRecentFiles.exec();
    }

    // file usage
    myDB.exec("DROP TABLE file_usage");
    myDB.exec("CREATE TABLE file_usage (id INTEGER PRIMARY KEY ASC, path TEXT, usage_count INTEGER)");
    QSqlQuery qFileUsage;
    qFileUsage.prepare("INSERT INTO file_usage (path, usage_count) VALUES (:path, :usage_count)");
    int fileEntryCount = this->fileEntryIndex.size();
    for (int i=0; i<fileEntryCount; i++){
        FileEntry *fe = this->fileLog.at(i);
        qFileUsage.bindValue(":path", fe->filePath);
        qFileUsage.bindValue(":usage_count", fe->usageCount);
        qFileUsage.exec();
    }
}

void DataStore::setPreset(int pos, QString path)
{
    this->presets[pos] = path;
}

QString DataStore::getPreset(int pos)
{
    return this->presets[pos];
}

QString DataStore::getNotes()
{
    return this->notes;
}

void DataStore::setNotes(QString notes)
{
    this->notes = notes;
}
QString DataStore::getFileBrowser() const
{
    return fileBrowser;
}

void DataStore::setFileBrowser(const QString &value)
{
    fileBrowser = value;
}
QString DataStore::getTerminalEmulator() const
{
    return terminalEmulator;
}

void DataStore::setTerminalEmulator(const QString &value)
{
    terminalEmulator = value;
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

void DataStore::pushRecentFile(QString path)
{
    if(!recentFiles.contains(path)){
        this->recentFiles.prepend(path);
        if (this->recentFiles.size() > 10)
            this->recentFiles.removeLast();
    }

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
