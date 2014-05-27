#ifndef DSTABLEFAV_H
#define DSTABLEFAV_H

#include "dstable.h"

class DsTableFav : public DsTable
{
    Q_OBJECT

public:
    explicit DsTableFav(QObject *parent = 0);
    virtual ~DsTableFav();

    virtual void initTable(QString            tableName,
                   QList<SchemaField> fieldSchema,
                   QSqlDatabase       db);

    virtual void insertRecord(Record record);

    QList<Record> getTop10();
    QList<Record> getRecent10();
};

#endif // DSTABLEFAV_H
