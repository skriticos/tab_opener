#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QSqlDatabase>

#include "dstable.h"
#include "dstablefav.h"

#include "history.h"

class DataStore : public QObject
{
    Q_OBJECT
public:
    explicit DataStore(QObject *parent = 0);
    ~DataStore();

    DsTableFav *tblCommands;
    DsTableFav *tblFiles;
    DsTable    *tblExtensions;
    DsTable    *tblGeneral;
    DsTable    *tblFileNotes;
    DsTable    *tblCommandNotes;

    void    initWidgets();

    bool    setCommand(QString cmd, QString path);
    bool    setExtensionValues(QString extStr, QString extActPri, QString extActSec);
    bool    setFile(QString path);
    bool    setGeneralValue(QString key, QString value);
    bool    setPreset(int pos, QString path);
    QString getExtActPri(QString ext);
    QString getExtActSec(QString ext);
    QString getGeneralValue(QString key);
    QString getPreset(int pos);

    QString getFileNote(QString filePath);
    QString getCommandNote(QString command);
    bool setFileNote(QString filePath, QString note);
    bool setCommandNote(QString command, QString note);

signals:
    // called on init and when history changes
    void sigUpdateFileHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void sigUpdateCommandHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);

private:
    void _updateFileHistory();
    void _updateCommandHistory();

    QSqlDatabase dsDB;
};

#endif // DATASTORE_H
