#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QListWidgetItem>

#include "dstable.h"

class FileEntry
{
public:
    QString filePath;
    int usageCount;

    explicit FileEntry(QString path);
    static bool isMore(FileEntry *a, FileEntry *b);
};

class RunEntry
{
public:

    QString command;
    QString execPath;
    int usageCount;

    explicit RunEntry(QString command, QString path);
    static bool isMore(RunEntry *a, RunEntry *b);
};

class DataStore : public QObject
{
    Q_OBJECT
public:
    explicit DataStore(QObject *parent = 0);
    ~DataStore();
    void loadData();
    void saveData();

    DsTable *tblGeneral;
    DsTable *tblExtensions;

    bool    setExtensionValues(QString extStr, QString extActPri, QString extActSec);
    bool    setGeneralValue(QString key, QString value);
    bool    setPreset(int pos, QString path);
    QString getGeneralValue(QString key);
    QString getPreset(int pos);




    QString getRecentFile(int pos);
    int getRecentFileCount();
    void pushRecentFile(QString path);

    QString getPopularFile(int pos);
    int getPopularFileCount();

    RunEntry* getRecentCommand(int pos);
    int getRecentCommandCount();
    void pushRecentCommand(QString command, QString path);

    RunEntry* getPopularCommand(int pos);
    int getPopularCommandCount();

signals:

public slots:

private:
    QStringList recentFiles;
    QList<FileEntry*> fileLog; // sorted list of file entries based on usage count
    QMap<QString, FileEntry*> fileEntryIndex; //inde of path of file entries

    QList<RunEntry*> recentCommands;

    // note that we don't keep multiple copies of commands for different paths
    // if a command is run in a new path, we are updating the path and increment
    // the command count
    QMap<QString, RunEntry*> commandIndex;
    QList<RunEntry*> commandLog; // sorted list of commands by count


    QSqlDatabase dsDB;
};

#endif // DATASTORE_H
