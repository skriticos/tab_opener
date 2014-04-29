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
