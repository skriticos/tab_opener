#include "datastore.h"

DataStore::DataStore(QObject *parent) : QObject(parent)
{
    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName(QDir::homePath() + QDir::separator() + ".tab_opener.db");
    myDB.open();

    if(!myDB.tables().contains("presets")) {
        myDB.exec("CREATE TABLE presets (id INTEGER PRIMARY KEY ASC, path TEXT);");
        for (int i=0; i<10; i++)
            myDB.exec("INSERT INTO presets (path) VALUES ('n/a')");
    }

    loadData();

    /*
    myFileExtOpenEdit["html"] = QStringList() << "/usr/bin/google-chrome %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["css"] = QStringList() << "/usr/bin/google-chrome %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["js"] = QStringList() << "/usr/bin/google-chrome %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["svg"] = QStringList() << "/usr/bin/google-chrome %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["go"] = QStringList() << "/usr/bin/go run %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["h"] = QStringList() << "" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["cpp"] = QStringList() << "" << "/usr/bin/gvim %F";

    QSqlDatabase mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("/home/seth/.tab_opener.db");
    mydb.open();
    if (mydb.tables().contains("extensions")) {
        qDebug() << "extension table found, reading entries..";
    } else {
        mydb.exec("create table extensions ( extension varchar(20), appopen varchar(255), appedit varchar(255) );");
    }
    mydb.close();
    */
}

DataStore::~DataStore()
{
    this->saveData();
    myDB.close();
}

void DataStore::loadData()
{
    QSqlQuery qPreset = myDB.exec("SELECT path FROM presets;");
    for (int i=0; i<10; i++){
        qPreset.next();
        this->presets[i] = qPreset.value(0).toString();
    }
}

void DataStore::saveData()
{
    QSqlQuery qPreset(myDB);
    qPreset.prepare("UPDATE presets SET path=:path WHERE id=:id;");
    for (int i=0; i<10; i++){
        qPreset.bindValue(":path", presets[i]);
        qPreset.bindValue(":id", i+1);
        qPreset.exec();
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
