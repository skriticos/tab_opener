#include "dstable.h"

DsTable::DsTable(QString tableName, QList<DsTable::SchemaField> fieldSchema, QSqlDatabase db, QObject *parent)
    : QObject(parent)
{
    QString fieldKey;
    QStringList reservedSqliteKeywords;

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

    Q_ASSERT(!tableName.isEmpty());
    Q_ASSERT(db.isOpen());

    for(int i = 0; i < fieldSchema.size(); i++) {

        fieldKey = fieldSchema.at(i).fieldName.toUpper();
        // yes, I had this problem and spent too much time to debug it
        Q_ASSERT(!reservedSqliteKeywords.contains(fieldKey));
    }

    this->tableName = tableName;
    this->schema = fieldSchema;
    this->lookupKey = fieldSchema.first().fieldName;
    this->db = db;

    if(this->db.tables().contains(tableName)) {
        this->_loadTable();
    } else {
        this->_createTable();
    }
}

DsTable::~DsTable()
{

}

void DsTable::insertRecord(Record record)
{
    QString lKey; // lookup key value
    QSqlQuery query(this->db);
    QStringList fieldNames, fieldPlaceholderNames;
    QMetaType::Type metaType;

    // verify record field names contain schema fields
    // note: we don't care about additional fields as long as the schema fields are properly populated
    for(int i = 0; i < this->schema.size(); i++) {

        QString fieldName = this->schema.at(i).fieldName;
        FieldType fieldType = this->schema.at(i).fieldType;

        // verify schema field is present
        Q_ASSERT(record.contains(fieldName));

        // verify schema field has correct type
        metaType = (QMetaType::Type)record.value(fieldName).type();

        switch(fieldType) {
        case TEXT:
            Q_ASSERT(metaType == QMetaType::QString);
            break;

        case INTEGER:
            Q_ASSERT(metaType == QMetaType::Int ||
                     metaType == QMetaType::Long ||
                     metaType == QMetaType::LongLong);
            break;

        case REAL:
            Q_ASSERT(metaType == QMetaType::Float);
            break;

        case BOOLEAN:
            Q_ASSERT(metaType == QMetaType::Bool);
            break;

        case BLOB:
            Q_ASSERT(metaType == QMetaType::QByteArray);
        }
    }

    lKey = record.value(lookupKey).toString();

    // insert record into disk database
    for(int i = 0; i < this->schema.size(); i++) {

        SchemaField schemaField = this->schema.at(i);
        fieldNames << schemaField.fieldName;
        fieldPlaceholderNames << ":" + schemaField.fieldName;
    }

    if(this->records.contains(lKey)) { // update record

        QStringList setStmt, setFieldNames;
        QString whereStmt, fieldName;

        setFieldNames = fieldNames.mid(1);

        for(int i = 0; i < setFieldNames.size(); i++) {
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

    for(int i = 0; i < this->schema.size(); i++) {
        SchemaField schemaField = this->schema.at(i);
        QString fieldName = schemaField.fieldName;
        query.bindValue(":" + fieldName, record.value(fieldName));
    }

    if(!query.exec()) {
        qDebug() << record;
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        Q_ASSERT(QString("SQL QUERY FAILED").isEmpty());
    }

    // insert record into runtime database
    this->records.insert(lKey, record);
}

void DsTable::deleteRecord(QString lKey)
{
    QSqlQuery query(this->db);

    Q_ASSERT(this->records.contains(lKey));

    query.prepare("DELETE FROM " + this->tableName + " WHERE " + this->lookupKey + "=:lKey");
    query.bindValue(":lKey", lKey);
    Q_ASSERT(query.exec());

    this->records.remove(lKey);
}

int DsTable::size()
{
    return this->records.size();
}

bool DsTable::contains(QString lookupKey)
{
    return this->records.contains(lookupKey);
}

DsTable::Record DsTable::getRecord(QString lookupKey)
{
    return this->records.value(lookupKey);
}

QStringList DsTable::getRecordKeys()
{
    return this->records.keys();
}

void DsTable::clearRecords()
{
    this->records.clear();
    this->db.exec("DELETE FROM " + this->tableName);
}

void DsTable::_loadTable()
{
    QSqlQuery query;
    QString fieldName;
    QVariant fieldValue;
    Record record;

    query = this->db.exec("SELECT * FROM " + this->tableName);
    Q_ASSERT(query.lastError().number() == -1);

    // read records
    while(query.next()) {

        record.clear();

        // iterate through schema fields
        for(int i = 0; i < this->schema.size(); i++) {

            fieldName = this->schema.at(i).fieldName;
            fieldValue = query.value(i);

            record.insert(fieldName, fieldValue);
        }

        // commit record
        records.insert(query.value(0).toString(), record);
    }
}

void DsTable::_createTable()
{
    QStringList fieldDeclarations;
    QString strFieldType;
    QSqlQuery query;
    SchemaField schemaField;

    for(int i = 0; i < this->schema.size(); i++) {

        schemaField = this->schema.at(i);

        switch(schemaField.fieldType) {
        case TEXT:
            strFieldType = "TEXT";
            break;

        case INTEGER:
            strFieldType = "INTEGER";
            break;

        case REAL:
            strFieldType = "REAL";
            break;

        case BOOLEAN:
            strFieldType = "BOOLEAN";
            break;

        case BLOB:
            strFieldType = "BLOB";
            break;
        }

        fieldDeclarations << schemaField.fieldName + " " + strFieldType;
    }

    query = this->db.exec("CREATE TABLE " + this->tableName + " (" + fieldDeclarations.join(", ") + ")");
    Q_ASSERT(query.lastError().number() == -1);
}
