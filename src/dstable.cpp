#include "dstable.h"

/**
 * @brief DsTable::testClass
 */
void DsTable::testClass()
{
    qDebug() << "testing DsTable..";

    // declare variables
    bool result; // used to test insertRecord return values
    QSqlDatabase testDB; // in-memory database for testing
    QSqlQuery query; // re-used query to verify database changes made by DsTable
    QString testTblName; // name of the table that is used for testing
    DsTable testTblUninitialized, testTbl1, testTbl2, testTblInvalid;
    DsTable::SchemaField ext, primary, secondary, tertiary, keyword; // table schema fields
    QList<DsTable::SchemaField> testSchema, invalidSchema; // table schema for initialization
    DsTable::Record record, invalidRecord, record1, record2; // records that are used to perform testing

    // initialize variables
    testTblName = "tblTest";

    ext       = {"extension",     DsTable::TEXT};
    primary   = {"act_primary",   DsTable::TEXT};
    secondary = {"act_secondary", DsTable::TEXT};
    tertiary  = {"act_tertiary",  DsTable::INTEGER};
    testSchema << ext << primary << secondary << tertiary;

    keyword = {"primary", DsTable::TEXT};
    invalidSchema << keyword;

    invalidRecord["foo"] = "bar";

    record1["extension"] = "html"; // valid record
    record1["act_primary"] = "/foo/html";
    record1["act_secondary"] = "/bar/html";
    record1["act_tertiary"] = 17;

    record2["extension"] = "svg"; // valid record
    record2["act_primary"] = "/foo/svg";
    record2["act_secondary"] = "/bar/svg";
    record2["act_tertiary"] = 17;

    // PREPARE TEST DATABASE
    testDB = QSqlDatabase::addDatabase("QSQLITE", "connTest");
    testDB.setDatabaseName(":memory:");
    testDB.open();

    // TEST TABLE INITIALIZATION
    result = testTblInvalid.initTable("none", invalidSchema, testDB);
    Q_ASSERT(!result);

    result = testTbl1.initTable(testTblName, testSchema, testDB);
    Q_ASSERT(result);

    // TEST METHOD   DsTable::insertRecord()
    result = testTblUninitialized.insertRecord(record1);
    Q_ASSERT_X(result == false, "DsTable::insertRecord()", "returned true for uninitialized table");

    result = testTbl1.insertRecord(invalidRecord); // missing fields
    Q_ASSERT_X(result == false, "DsTable::insertRecord()", "returned true for invalid record");

    invalidRecord["extension"] = true; // does not mach schema
    invalidRecord["act_primary"] = "/foo/html";
    invalidRecord["act_secondary"] = "/bar/html";
    invalidRecord["act_tertiary"] = 17;

    result = testTbl1.insertRecord(invalidRecord);
    Q_ASSERT_X(result == false, "DsTable::insertRecord()", "returned true for invalid record");

    result = testTbl1.insertRecord(record1);
    Q_ASSERT_X(result == true, "DsTable::insertRecord()", "returned false for valid record");

    result = testTbl1.insertRecord(record2);
    Q_ASSERT(result == true);

    query = testDB.exec("SELECT * FROM " + testTblName);
    query.first();

    Q_ASSERT(query.value(0) == "html");
    Q_ASSERT(query.value(1) == "/foo/html");
    Q_ASSERT(query.value(2) == "/bar/html");

    query.next();

    Q_ASSERT(query.value(0) == "svg");
    Q_ASSERT(query.value(1) == "/foo/svg");
    Q_ASSERT(query.value(2) == "/bar/svg");

    record1["act_primary"] = "/foo/changed/html";
    record1["act_secondary"] = "/bar/changed/html";

    result = testTbl1.insertRecord(record1);
    Q_ASSERT(result == true);

    query = testDB.exec("SELECT * FROM " + testTblName);
    query.first();

    Q_ASSERT(query.value(0) == "html");
    Q_ASSERT(query.value(1) == "/foo/changed/html");
    Q_ASSERT(query.value(2) == "/bar/changed/html");

    // SECOND TEST CONSTRUCTOR: LOAD DATA FROM DATABASE
    result = testTbl2.initTable(testTblName, testSchema, testDB);
    Q_ASSERT(result);

    record = testTbl2.getRecord("html");

    Q_ASSERT(record.value("extension") == "html");
    Q_ASSERT(record.value("act_primary") == "/foo/changed/html");
    Q_ASSERT(record.value("act_secondary") == "/bar/changed/html");
    Q_ASSERT(record.value("act_tertiary") == 17);

    record = testTbl2.getRecord("svg");

    Q_ASSERT(record.value("extension") == "svg");
    Q_ASSERT(record.value("act_primary") == "/foo/svg");
    Q_ASSERT(record.value("act_secondary") == "/bar/svg");

    // TEST DsTable::recordExists()
    Q_ASSERT(testTbl2.contains("html"));
    Q_ASSERT(!testTbl2.contains("foobar"));

    // TEST METHOD   DsTable::getRecordCount()
    Q_ASSERT(testTbl2.size() == 2);

    // DETELING RECORD
    result = testTbl2.deleteRecord("html");
    Q_ASSERT(result);

    query = testDB.exec("SELECT * FROM " + testTblName);
    query.last();

    Q_ASSERT(query.at()+1 == 1); // note: SQLite driver does not support query.size(), but this works
    Q_ASSERT(query.value(0) == "svg");
    Q_ASSERT(query.value(1) == "/foo/svg");
    Q_ASSERT(query.value(2) == "/bar/svg");

    // TEST CLEAR DATABASE
    testTbl2.clearRecords();

    Q_ASSERT(testTbl2.size() == 0);

    query = testDB.exec("SELECT * FROM " + testTblName);
    query.last();
    Q_ASSERT(query.at() == QSql::AfterLastRow || query.at() == QSql::BeforeFirstRow);

    // CLEANUP
    testDB.close();

    qDebug() << "testing DsTable passed!";
}

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
        result = this->loadTable();
    else
        result = this->createTable();

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
        qDebug() << "DsTable::insertRecord():" << "table not initialized";
        return false;
    }

    // verify record field names contain schema fields
    // note: we don't care about additional fields as long as the schema fields are properly populated
    for(int i=0; i<this->schema.size(); i++){

        QString fieldName = this->schema.at(i).fieldName;
        FieldType fieldType = this->schema.at(i).fieldType;

        // verify schema field is present
        if(!record.contains(fieldName)){
            qDebug() << "DsTable::insertRecord():" << "record missing schema field" << fieldName;
            return false;
        }

        // verify schema field has correct type
        metaType = (QMetaType::Type)record.value(fieldName).type();
        switch(fieldType){
        case TEXT:
            if(metaType != QMetaType::QString){
                qDebug() << "DsTable::insertRecord():" << "field validation failed" << fieldName;
                return false;
            }
            break;
        case INTEGER:
            if(metaType != QMetaType::Int){
                qDebug() << "DsTable::insertRecord():" << "field validation failed" << fieldName;
                return false;
            }
            break;
        case REAL:
            if(metaType != QMetaType::Float){
                qDebug() << "DsTable::insertRecord():" << "field validation failed" << fieldName;
                return false;
            }
            break;
        case BOOLEAN:
            if(metaType != QMetaType::Bool){
                qDebug() << "DsTable::insertRecord():" << "field validation failed" << fieldName;
                return false;
            }
            break;
        case BLOB:
            if(metaType != QMetaType::QByteArray){
                qDebug() << "DsTable::insertRecord():" << "field validation failed" << fieldName;
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
    if(!result){
        qDebug() << "DsTable::insertRecord():" << "SQL query failed" << query.lastError();
        return false;
    }

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
bool DsTable::loadTable()
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
bool DsTable::createTable()
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
