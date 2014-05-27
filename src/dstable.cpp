#include "dstable.h"

/**
 * @brief DsTable::DsTable
 * @param parent
 */
DsTable::DsTable(QObject *parent) : QObject(parent)
{
    this->tableInitialized = false;
}

/**
 * @brief DsTable::initTable
 * @param tableName
 * @param fieldSchema
 * @param db
 * @return
 */
bool DsTable::initTable(QString tableName, QList<DsTable::SchemaField> fieldSchema, QSqlDatabase db)
{
    bool result;
    QString fieldKey;
    QStringList reservedSqliteKeywords;

    result = true;
    reservedSqliteKeywords
        << "ABORT" << "ACTION" << "ADD" << "AFTER" << "ALL" << "ALTER" << "ANALYZE"
        << "AND" << "AS" << "ASC" << "ATTACH" << "AUTOINCREMENT" << "BEFORE" << "BEGIN"
        << "BETWEEN" << "BY" << "CASCADE" << "CASE" << "CAST" << "CHECK" << "COLLATE"
        << "COLUMN" << "COMMIT" << "CONFLICT" << "CONSTRAINT" << "CREATE" << "CROSS"
        << "CURRENT_DATE" << "CURRENT_TIME" << "CURRENT_TIMESTAMP" << "DATABASE"
        << "DEFAULT" << "DEFERRABLE" << "DEFERRED" << "DELETE" << "DESC" << "DETACH"
        << "DISTINCT" << "DROP" << "EACH" << "ELSE" << "END" << "ESCAPE" << "EXCEPT"
        << "EXCLUSIVE" << "EXISTS" << "EXPLAIN" << "FAIL" << "FOR" << "FOREIGN"
        << "FROM" << "FULL" << "GLOB" << "GROUP" << "HAVING" << "IF" << "IGNORE"
        << "IMMEDIATE" << "IN" << "INDEX" << "INDEXED" << "INITIALLY" << "INNER"
        << "INSERT" << "INSTEAD" << "INTERSECT" << "INTO" << "IS" << "ISNULL"
        << "JOIN" << "KEY" << "LEFT" << "LIKE" << "LIMIT" << "MATCH" << "NATURAL"
        << "NO" << "NOT" << "NOTNULL" << "NULL" << "OF" << "OFFSET" << "ON" << "OR"
        << "ORDER" << "OUTER" << "PLAN" << "PRAGMA" << "PRIMARY" << "QUERY" << "RAISE"
        << "RECURSIVE" << "REFERENCES" << "REGEXP" << "REINDEX" << "RELEASE"
        << "RENAME" << "REPLACE" << "RESTRICT" << "RIGHT" << "ROLLBACK" << "ROW"
        << "SAVEPOINT" << "SELECT" << "SET" << "TABLE" << "TEMP" << "TEMPORARY"
        << "THEN" << "TO" << "TRANSACTION" << "TRIGGER" << "UNION" << "UNIQUE"
        << "UPDATE" << "USING" << "VACUUM" << "VALUES" << "VIEW" << "VIRTUAL"
        << "WHEN" << "WHERE" << "WITH" << "WITHOUT";

    if(tableName == "") return false;
    if(!db.isOpen())    return false;

    for(int i=0; i<fieldSchema.size(); i++){

        fieldKey = fieldSchema.at(i).fieldName.toUpper();
        // yes, I had this problem and spent too much time to debug it
        if(reservedSqliteKeywords.contains(fieldKey))
            return false;
    }

    this->tableName = tableName;
    this->schema = fieldSchema;
    this->lookupKey = fieldSchema.first().fieldName;
    this->db = db;

    if(this->db.tables().contains(tableName))
        result = this->_loadTable();
    else
        result = this->_createTable();

    this->tableInitialized = true;

    return result;
}

/**
 * @brief DsTable::insertRecord
 * @param record
 */
bool DsTable::insertRecord(Record record)
{
    bool result;
    QString lKey; // lookup key value
    QSqlQuery query(this->db);
    QStringList fieldNames, fieldPlaceholderNames;
    QMetaType::Type metaType;

    if(!this->tableInitialized){
        return false;
    }

    // verify record field names contain schema fields
    // note: we don't care about additional fields as long as the schema fields are properly populated
    for(int i=0; i<this->schema.size(); i++){

        QString fieldName = this->schema.at(i).fieldName;
        FieldType fieldType = this->schema.at(i).fieldType;

        // verify schema field is present
        if(!record.contains(fieldName)){
            return false;
        }

        // verify schema field has correct type
        metaType = (QMetaType::Type)record.value(fieldName).type();
        switch(fieldType){
        case TEXT:
            if(metaType != QMetaType::QString){
                return false;
            }
            break;
        case INTEGER:
            if(metaType != QMetaType::Int && metaType != QMetaType::Long && metaType != QMetaType::LongLong){
                return false;
            }
            break;
        case REAL:
            if(metaType != QMetaType::Float){
                return false;
            }
            break;
        case BOOLEAN:
            if(metaType != QMetaType::Bool){
                return false;
            }
            break;
        case BLOB:
            if(metaType != QMetaType::QByteArray){
                return false;
            }
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
    Q_ASSERT(result);

    // insert record into runtime database
    this->records.insert(lKey, record);

    return true;
}

/**
 * @brief DsTable::deleteRecord
 * @param lKey
 * @return
 */
bool DsTable::deleteRecord(QString lKey)
{
    QSqlQuery query(this->db);

    if(!this->records.contains(lKey))
        return false;

    query.prepare("DELETE FROM " + this->tableName + " WHERE " + this->lookupKey + "='html'");
    if(!query.exec())
        return false;

    this->records.remove(lKey);

    return true;
}

/**
 * @brief DsTable::getRecordCount
 * @return
 */
int DsTable::size()
{
    return this->records.size();
}

/**
 * @brief DsTable::recordExists
 * @param lookupKey
 * @return
 */
bool DsTable::contains(QString lookupKey)
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

QStringList DsTable::getRecordKeys()
{
    return this->records.keys();
}

/**
 * @brief DsTable::clearRecords
 */
void DsTable::clearRecords()
{
    if(!this->tableInitialized) return;
    this->records.clear();
    this->db.exec("DELETE FROM " + this->tableName);
}

/**
 * @brief DsTable::loadTable
 *
 * Notice: we don't assume that anything nefarious is going on in the database,
 *         so the query input is not verified. Hm, maybe we should verify the
 *         query data?
 */
bool DsTable::_loadTable()
{
    QSqlQuery query;
    QString fieldName;
    QVariant fieldValue;
    Record record;

    query = this->db.exec("SELECT * FROM " + this->tableName);
    if(query.lastError().number() != -1)
        return false;

    // read records
    while (query.next()){

        record.clear();

        // iterate through schema fields
        for(int i=0; i<this->schema.size(); i++){

            fieldName = this->schema.at(i).fieldName;
            fieldValue = query.value(i);

            record.insert(fieldName, fieldValue);
        }
        // commit record
        records.insert(query.value(0).toString(), record);
    }

    return true;
}

/**
 * @brief DsTable::createTable
 */
bool DsTable::_createTable()
{
    QStringList fieldDeclarations;
    QString strFieldType;
    QSqlQuery query;
    SchemaField schemaField;

    for(int i=0; i<this->schema.size(); i++){

        schemaField = this->schema.at(i);
        switch(schemaField.fieldType){
            case TEXT:    strFieldType = "TEXT";    break;
            case INTEGER: strFieldType = "INTEGER"; break;
            case REAL:    strFieldType = "REAL";    break;
            case BOOLEAN: strFieldType = "BOOLEAN"; break;
            case BLOB:    strFieldType = "BLOB";    break;
        }
        fieldDeclarations << schemaField.fieldName + " " + strFieldType;
    }

    query = this->db.exec("CREATE TABLE " + this->tableName + " (" + fieldDeclarations.join(", ") + ")");

    if(query.lastError().number() != -1)
        return false;

    return true;
}
