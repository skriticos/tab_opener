#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QSqlDatabase>

#include "dstable.h"
#include "dstablefav.h"

#include "history.h"

// database column name defines (to avoid typos)
#define COMMAND "command"
#define CURRENT_COMMAND "current_command"
#define EXT_ACT_PRI "ext_act_pri"
#define EXT_ACT_SEC "ext_act_sec"
#define EXT_STR "ext_str"
#define GKEY "gkey"
#define GVAL "gval"
#define NOTE "note"
#define PATH "path"
#define WORKING_DIRECTORY "working_directory"

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

    void    setExtensionValues(QString extStr, QString extActPri, QString extActSec);
    void    setFile(QString path);
    void    setGeneralValue(QString key, QString value);
    void    setPreset(int pos, QString path);
    QString getExtActPri(QString ext);
    QString getExtActSec(QString ext);
    QString getGeneralValue(QString key);
    QString getPreset(int pos);

    QString getFileNote(QString filePath);
    QString getCommandNote(QString command);
    void setFileNote(QString filePath, QString note);
    void setCommandNote(QString command, QString note);

public slots:
    void slotCommandExecuted(QString commandString, QString workingDirectory);

signals:
    // called on init and when history changes
    void sigUpdateFileHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void sigUpdateCommandHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void sigUpdatePresets(QStringList presetList);
    void sigCommandChanged(QString commandStr);

private:
    void _updateFileHistory();
    void _updateCommandHistory();
    void _updatePresets();
    void _updateCommandWidget();

    QSqlDatabase dsDB;
};

#endif // DATASTORE_H
