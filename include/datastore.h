#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QObject>
#include <QSql>
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
    void loadData();
    void saveData();
    void incrementFile(QString filePath, QString execPath);
    void incrementCmd(QString command, QString execPath);
    void setPreset(int pos, QString path);
    void setExtension(QString extension, QString openPath, QString editPath);

signals:

public slots:

private:
    QStringList presets;
    QHash<QString, ExtensionHandlers> extensions;
    QList<FileEntry> recentFiles; // fifo queue, 10 entries
    QList<FileEntry> popularFiles; // sorted list, most used first
    QList<RunEntry> recentCommands; // fifo queue, 10 entries
    QList<RunEntry> popularCommands; // sorted list, most used first
};

#endif // DATASTORE_H
