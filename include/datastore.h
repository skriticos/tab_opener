#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

struct ExtensionHandlers
{
    int id;
    QString openPath;
    QString editPath;
};

struct FileEntry
{
    int id;
    QString execPath;
    QString filePath;
    int usageCount;

    FileEntry(){
        usageCount = 0;
    }
};

struct RunEntry
{
    QString command;
    QString execPath;
};

class DataStore : public QObject
{
    Q_OBJECT
public:
    explicit DataStore(QObject *parent = 0);
    ~DataStore();
    void loadData();
    void saveData();
    void incrementFile(QString filePath, QString execPath);
    void incrementCmd(QString command, QString execPath);
    void setPreset(int pos, QString path);
    QString getPreset(int pos);
    void setExtension(QString extension, QString openPath, QString editPath);
    QString getNotes();
    void setNotes(QString notes);

    QString getFileBrowser() const;
    void setFileBrowser(const QString &value);

    QString getTerminalEmulator() const;
    void setTerminalEmulator(const QString &value);

signals:

public slots:

private:
    QString presets[10];
    QString notes;
    QString fileBrowser;
    QString terminalEmulator;
    QHash<QString, ExtensionHandlers> extensions;
    QList<FileEntry> recentFiles; // fifo queue, 10 entries
    QList<FileEntry> popularFiles; // sorted list, most used first
    QList<RunEntry> recentCommands; // fifo queue, 10 entries
    QList<RunEntry> popularCommands; // sorted list, most used first

    QSqlDatabase myDB;
};

#endif // DATASTORE_H
