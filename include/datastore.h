#ifndef DATASTORE_H
#define DATASTORE_H

#include <QtCore>
#include <QSqlDatabase>

#include "dstable.h"
#include "dstablefav.h"

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

    bool    setCommand(QString cmd, QString path, QString notes);
    bool    setExtensionValues(QString extStr, QString extActPri, QString extActSec);
    bool    setFile(QString path, QString notes);
    bool    setGeneralValue(QString key, QString value);
    bool    setPreset(int pos, QString path);
    QString getGeneralValue(QString key);
    QString getPreset(int pos);

private:
    QSqlDatabase dsDB;
};

#endif // DATASTORE_H
