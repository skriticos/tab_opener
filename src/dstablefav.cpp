#include "dstablefav.h"

QList<DsTable::SchemaField> patchSchema(QList<DsTable::SchemaField> fieldSchema){
    fieldSchema.insert(1, {"counter", DsTable::INTEGER});
    fieldSchema.insert(2, {"timestamp", DsTable::INTEGER});
    return fieldSchema;
}

// notice: the fieldSchema is patched before passed to base class constructor
//         see patchSchema function for details
DsTableFav::DsTableFav(
        QString tableName, QList<DsTable::SchemaField> fieldSchema, QSqlDatabase db, QObject *parent) :
        DsTable(tableName, patchSchema(fieldSchema), db, parent)
{
}

DsTableFav::~DsTableFav()
{
}

/*
 * Instead of just inserting the record into the database as the parent does,
 * the record is extended with timestamp and count to be retrived by the top10 / recent10 methods.
 * In other words providing file path to location favourites is enough for this method.
 */
void DsTableFav::insertRecord(DsTable::Record record)
{
    qint64 timesamp = 0;
    bool recordExists = false;
    bool result;
    QSqlQuery query(this->db);

    // retrive exists and count from the database
    query.prepare("SELECT EXISTS ( SELECT 1 FROM " + this->tableName + " WHERE "
                  + this->lookupKey + "=:l_key LIMIT 1 )");
    query.bindValue(":l_key", record.value(this->lookupKey));
    result = query.exec();
    Q_ASSERT(result);
    query.next();
    query.value(0).toLongLong() == 1 ? recordExists = true : recordExists = false;

    if(recordExists){ // record already exists

        // read record values
        query.prepare("SELECT * FROM " + this->tableName + " WHERE " + this->lookupKey + " =:l_key");
        query.bindValue(":l_key", record.value(this->lookupKey));
        result = query.exec();
        Q_ASSERT(result);
        query.next();
        int fieldCount = this->schema.size();
        for(int i=0; i<fieldCount; i++){
            QString fieldName = this->schema.at(i).fieldName;
            record[fieldName] = query.value(i);
        }
        // increment counter
        record["counter"] = record["counter"].toInt() + 1;
    } else { // record does not exist
        record["counter"] = 1;
    }

    timesamp = QDateTime::currentMSecsSinceEpoch();
    record["timestamp"] = timesamp;

    DsTable::insertRecord(record);
}

QList<DsTable::Record> DsTableFav::getTop10()
{
    QList<Record> result;
    QSqlQuery query(this->db);
    bool success;

    success = query.exec("SELECT * FROM " + this->tableName + " ORDER BY counter DESC LIMIT 10");
    Q_ASSERT(success);

    while(query.next()){
        Record record;
        for(int i=0; i<this->schema.size(); i++){
            QString fieldName = this->schema.at(i).fieldName;
            record[fieldName] = query.value(i);
        }
        result.append(record);
    }
    return result;
}

QList<DsTable::Record> DsTableFav::getRecent10()
{
    QList<Record> result;
    QSqlQuery query(this->db);
    bool success;

    success = query.exec("SELECT * FROM " + this->tableName + " ORDER BY timestamp DESC LIMIT 10");
    Q_ASSERT(success);

    while(query.next()){
        Record record;
        for(int i=0; i<this->schema.size(); i++){
            QString fieldName = this->schema.at(i).fieldName;
            record[fieldName] = query.value(i);
        }
        result.append(record);
    }
    return result;
}
