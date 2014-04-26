#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QObject>
#include <QSql>
#include <QDebug>

struct ExtensionHandlers
{
    QString openPath;
    QString editPath;
};

struct FileEntry
{
    QString execPath;
    QString filePath;
};

struct RunEntry
{
    QString execPath;
    QString command;
};

class DataStore : public QObject
{
    Q_OBJECT
public:
    explicit DataStore(QObject *parent = 0);
    void loadData();
    void saveData();

signals:

public slots:

private:
    QStringList presets;
    QHash<QString, ExtensionHandlers> extensions;
    QList<FileEntry> recentFiles; // fifo queue, 10 entries
    QList<FileEntry> popularFiles; // sorted list
    QList<RunEntry> recentCommands; // fifo queue, 10 entries
    QList<RunEntry> popularCommands;
};

#endif // DATASTORE_H
