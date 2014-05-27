#ifndef DSTABLEFAV_H
#define DSTABLEFAV_H

#include "dstable.h"

class DsTableFav : public DsTable
{
    Q_OBJECT

public:
    explicit DsTableFav(
                        QString tableName,
                        QList<SchemaField> fieldSchema,
                        QSqlDatabase db,
                        QObject *parent = 0);
    virtual ~DsTableFav();

    virtual void insertRecord(Record record);

    QList<Record> getTop10();
    QList<Record> getRecent10();
};

#endif // DSTABLEFAV_H
