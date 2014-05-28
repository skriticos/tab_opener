#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QSqlDatabase>

#include "dstable.h"
#include "dstablefav.h"

#include "defines.h"
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

signals:
    // note:
    // we only send out signals from this module when database
    // queries are required
    // this is the case on startup and on database writes
    // other signals are passed directly between modules

    void sigInitCommand(QString commandStr);
    void sigInitNotesSelection(QString section);
    void sigInitGlobalNotes(QString noteText);

    void sigUpdateFileHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void sigUpdateCommandHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void sigUpdatePresets(QStringList presetList);

    void sigFileSelectionChanged(QString filePath, QString noteText); // through fileBrowser
    void sigCmdSelectionChanged(QString cmdStr, QString noteText); // through commandWidget

public slots:
    void slotCommandExecuted(QString commandString, QString workingDirectory);
    void slotGlobalNoteChanged(QString noteText);
    void slotFileNoteChanged(QString noteText, QString filePath);
    void slotCmdNoteChanged(QString noteText, QString cmdStr);

    void slotCmdChanged(QString cmdStr);
    void slotSelectedFileChanged(QString filePath);
    void slotNoteSelectionChanged(QString newSelection);

private:
    void _updateFileHistory();
    void _updateCommandHistory();
    void _updatePresets();
    void _initNoteWidget();
    void _initCommandWidget();

    QSqlDatabase dsDB;
};

#endif // DATASTORE_H
