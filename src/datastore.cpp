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


