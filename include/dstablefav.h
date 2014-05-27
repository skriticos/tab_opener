#ifndef DSTABLEFAV_H
#define DSTABLEFAV_H

#include "dstable.h"

class DsTableFav : public DsTable
{
    Q_OBJECT

public:
    explicit DsTableFav(QObject *parent = 0);

    virtual bool insertRecord(Record record);

    QList<Record> getTop10();
    QList<Record> getRecent10();

private:
    virtual bool _createTable();

};

#endif // DSTABLEFAV_H
