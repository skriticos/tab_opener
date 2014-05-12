#include "dstable.h"

/**
 * @brief DsTable::DsTable
 * @param tableName
 * @param fieldSchema
 * @param db
 * @param parent
 */
DsTable::DsTable(QString tableName,
                 QList<DsTable::SchemaField> fieldSchema,
                 QSqlDatabase db,
                 QObject *parent) : QObject(parent)
{
    this->tableName = tableName;
    this->schema = fieldSchema;
    this->lookupKey = fieldSchema.first().fieldName;
    this->db = db;

    if(this->db.tables().contains(tableName))
        this->loadTable();
    else
        this->createTable();
}

/**
 * @brief DsTable::insertRecord
 * @param record
 */
bool DsTable::insertRecord(Record record)
{
    bool result = true;
    QString lKey; // lookup key value
    QSqlQuery query(this->db);
    QStringList fieldNames, fieldPlaceholderNames;

    // verify record field names contain schema fields (type not checked yet)
    for(int i=0; i<this->schema.size(); i++){
        QString fieldName = this->schema.at(i).fieldName;
        if(!record.contains(fieldName)){
            qDebug() << "DsTable::insertRecord(): missing fieldname detected: " << fieldName;
            result = false;
            goto end;
        }
    }

    lKey = record.value(lookupKey).toString();

    // insert record into disk database
    for(int i=0; i<this->schema.size(); i++){

        SchemaField schemaField = this->schema.at(i);
        fieldNames << schemaField.fieldName;
        fieldPlaceholderNames << ":" + schemaField.fieldName;
    }

    if(this->records.contains(lKey)){ // update record

        QStringList setStmt, setFieldNames;
        QString whereStmt, fieldName;

        setFieldNames = fieldNames.mid(1);
        for(int i=0; i<setFieldNames.size(); i++){
            fieldName = setFieldNames.at(i);
            setStmt << fieldName + "=:" + fieldName;
        }
        fieldName = fieldNames.first();
        whereStmt = fieldName + "=:" + fieldName;

        query.prepare("UPDATE " + this->tableName + " SET " + setStmt.join(", ") + " WHERE " + whereStmt);

    } else { // create record

        query.prepare("INSERT INTO " + this->tableName + " (" + fieldNames.join(", ") + ") "
                      "VALUES (" + fieldPlaceholderNames.join(", ") + ")");
    }

    for(int i=0; i<this->schema.size(); i++){
        SchemaField schemaField = this->schema.at(i);
        QString fieldName = schemaField.fieldName;
        query.bindValue(":" + fieldName, record.value(fieldName));
    }

    result = query.exec();

    // insert record into runtime database
    if(result)
        this->records.insert(lKey, record);

end:
    return result;
}

/**
 * @brief DsTable::getRecordCount
 * @return
 */
int DsTable::getRecordCount()
{
    return this->records.size();
}

/**
 * @brief DsTable::recordExists
 * @param lookupKey
 * @return
 */
bool DsTable::recordExists(QString lookupKey)
{
    return this->records.contains(lookupKey);
}

/**
 * @brief DsTable::getRecord
 * @param lookupKey
 * @return
 */
DsTable::Record DsTable::getRecord(QString lookupKey)
{
    return this->records.value(lookupKey);
}

/**
 * @brief DsTable::clearRecords
 */
void DsTable::clearRecords()
{
    this->records.clear();
    this->db.exec("DELETE FROM " + this->tableName);
}

/**
 * @brief DsTable::loadTable
 */
void DsTable::loadTable()
{
    QSqlQuery query = this->db.exec("SELECT * FROM " + this->tableName);

    // TODO: continue here
}

/**
 * @brief DsTable::createTable
 */
void DsTable::createTable()
{
    QStringList fieldDeclarations;

    for(int i=0; i<this->schema.size(); i++){

        SchemaField schemaField = this->schema.at(i);
        QString strFieldType;

        switch(schemaField.fieldType){
            case TEXT:    strFieldType = "TEXT";    break;
            case INTEGER: strFieldType = "INTEGER"; break;
            case REAL:    strFieldType = "REAL";    break;
            case BOOLEAN: strFieldType = "BOOLEAN"; break;
            case BLOB:    strFieldType = "BLOB";    break;
        }
        fieldDeclarations << schemaField.fieldName + " " + strFieldType;
    }

    QSqlQuery result =
        this->db.exec("CREATE TABLE " + this->tableName + " (" + fieldDeclarations.join(", ") + ")");
}
