#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QSqlDatabase>

#include "dstable.h"
#include "dstablefav.h"

#include "defines.h"
#include "util.h"
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

    void initWidgetData();

signals:
    // note:
    // we only send out signals from this module when database
    // queries are required
    // this is the case on startup and on database writes
    // other signals are passed directly between modules

    void sigInitConfig(QStringList presetList,
                       QList<Config::ExtensionEntry> extList,
                       QString terminalCmd,
                       QString fbrowserCmd);

    void sigInitCommand(QString commandStr);
    void sigInitNotesSelection(QString section);
    void sigInitGlobalNotes(QString noteText);

    void sigUpdateFileHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void sigUpdateCommandHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void sigUpdatePresets(QStringList presetList);

    void sigFileSelectionChanged(QString filePath, QString noteText); // through fileBrowser
    void sigCmdSelectionChanged(QString cmdStr, QString noteText); // through commandWidget

    void sigFbInitLocation(QString folderPath, QString filePath);
    void sigFileOrExtAppOpened();

    void sigInvalidExtension();

public slots:
    void slotCfgPresetsChanged(QStringList presetList);
    void slotCfgExtensionChanged(Config::ExtensionEntry extEntry);
    void slotCfgExtensionDeleted(QString extStr);
    void slotCfgTerminalChanged(QString terminalCmd);
    void slotCfgExtFileBrowserChanged(QString fbrowserCmd);

    void slotCommandExecuted(QString commandString, QString workingDirectory);
    void slotGlobalNoteChanged(QString noteText);
    void slotFileNoteChanged(QString filePath, QString noteText);
    void slotCmdNoteChanged(QString cmdStr, QString noteText);

    void slotCmdChanged(QString cmdStr);
    void slotSelectedFileChanged(QString filePath);
    void slotSelectedFolderChanged(QString newFolderPath);
    void slotNoteSelectionChanged(QString newSelection);

    void slotOpenExtApp(FileOpen::ExtApp extApp, QString folderPath); // opens terminal / external file browser
    void slotOpenFile(FileOpen::OpenType openType, QString filePath); // opens a file (FileBrowser, FileHistory)

private:
    void _initConfig();
    void _updateFileHistory();
    void _updateCommandHistory();
    void _updatePresets();
    void _initNoteWidget();
    void _initCommandWidget();
    void _initFileBrowserWidget();

    void    _setGeneralValue(QString key, QString value);
    QString _getGeneralValue(QString key);

    QSqlDatabase dsDB;
};

#endif // DATASTORE_H
