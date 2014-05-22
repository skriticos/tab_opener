#include "test.h"

Test::Test(QObject *parent) : QObject(parent)
{
}

void Test::run()
{
    char* dummy_args[] = { const_cast<char *>("dummyname"), NULL };
    QTest::qExec(this, 1, dummy_args);
}

void Test::testUtilExecDetachedCommand()
{
    QVERIFY(Util::execDetachedCommand("") == false);
    QVERIFY(Util::execDetachedCommand("/bin/true") == true);
    QVERIFY(Util::execDetachedCommand("/bin/true " + QDir::homePath()) == true);
    QVERIFY(Util::execDetachedCommand("/bin/true", QDir::homePath()) == true);
    QVERIFY(Util::execDetachedCommand("/bin/true \"foo bar\" baz") == true);
}

void Test::testDsTable()
{
    // declare variables
    bool result; // used to test insertRecord return values
    QSqlDatabase testDB; // in-memory database for testing
    QSqlQuery query; // re-used query to verify database changes made by DsTable
    QString testTblName; // name of the table that is used for testing
    DsTable testTblUninitialized, testTbl1, testTbl2, testTblInvalid;
    DsTable::SchemaField ext, primary, secondary, tertiary, keyword; // table schema fields
    QList<DsTable::SchemaField> testSchema, invalidSchema; // table schema for initialization
    DsTable::Record record, invalidRecord, record1, record2; // records that are used to perform testing

    result = true;
    if(result) result = result; // remove compiler warn. for unsued variable (macros don't seem to count as being used)

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
    QVERIFY(!result);

    result = testTbl1.initTable(testTblName, testSchema, testDB);
    QVERIFY(result);

    // TEST METHOD   DsTable::insertRecord()
    result = testTblUninitialized.insertRecord(record1);
    QVERIFY(result == false);

    result = testTbl1.insertRecord(invalidRecord); // missing fields
    QVERIFY(result == false);

    invalidRecord["extension"] = true; // does not mach schema
    invalidRecord["act_primary"] = "/foo/html";
    invalidRecord["act_secondary"] = "/bar/html";
    invalidRecord["act_tertiary"] = 17;

    result = testTbl1.insertRecord(invalidRecord);
    QVERIFY(result == false);

    result = testTbl1.insertRecord(record1);
    QVERIFY(result == true);

    result = testTbl1.insertRecord(record2);
    QVERIFY(result == true);

    query = testDB.exec("SELECT * FROM " + testTblName);
    query.first();

    QVERIFY(query.value(0) == "html");
    QVERIFY(query.value(1) == "/foo/html");
    QVERIFY(query.value(2) == "/bar/html");

    query.next();

    QVERIFY(query.value(0) == "svg");
    QVERIFY(query.value(1) == "/foo/svg");
    QVERIFY(query.value(2) == "/bar/svg");

    record1["act_primary"] = "/foo/changed/html";
    record1["act_secondary"] = "/bar/changed/html";

    result = testTbl1.insertRecord(record1);
    QVERIFY(result == true);

    query = testDB.exec("SELECT * FROM " + testTblName);
    query.first();

    QVERIFY(query.value(0) == "html");
    QVERIFY(query.value(1) == "/foo/changed/html");
    QVERIFY(query.value(2) == "/bar/changed/html");

    // SECOND TEST CONSTRUCTOR: LOAD DATA FROM DATABASE
    result = testTbl2.initTable(testTblName, testSchema, testDB);
    QVERIFY(result);

    record = testTbl2.getRecord("html");

    QVERIFY(record.value("extension") == "html");
    QVERIFY(record.value("act_primary") == "/foo/changed/html");
    QVERIFY(record.value("act_secondary") == "/bar/changed/html");
    QVERIFY(record.value("act_tertiary") == 17);

    record = testTbl2.getRecord("svg");

    QVERIFY(record.value("extension") == "svg");
    QVERIFY(record.value("act_primary") == "/foo/svg");
    QVERIFY(record.value("act_secondary") == "/bar/svg");

    // TEST DsTable::recordExists()
    QVERIFY(testTbl2.contains("html"));
    QVERIFY(!testTbl2.contains("foobar"));

    // TEST METHOD   DsTable::getRecordCount()
    QVERIFY(testTbl2.size() == 2);

    // DETELING RECORD
    result = testTbl2.deleteRecord("html");
    QVERIFY(result);

    query = testDB.exec("SELECT * FROM " + testTblName);
    query.last();

    QVERIFY(query.at()+1 == 1); // note: SQLite driver does not support query.size(), but this works
    QVERIFY(query.value(0) == "svg");
    QVERIFY(query.value(1) == "/foo/svg");
    QVERIFY(query.value(2) == "/bar/svg");

    // TEST CLEAR DATABASE
    testTbl2.clearRecords();

    QVERIFY(testTbl2.size() == 0);

    query = testDB.exec("SELECT * FROM " + testTblName);
    query.last();
    QVERIFY(query.at() == QSql::AfterLastRow || query.at() == QSql::BeforeFirstRow);

    // CLEANUP
    testDB.close();
}

void Test::testDsTableFav()
{
    QSqlDatabase testDB = QSqlDatabase::addDatabase("QSQLITE", "connTest2");
    testDB.setDatabaseName(":memory:");
    testDB.open();

    DsTableFav dtf;
    DsTable::Record record;

    DsTable::SchemaField path, cmd;
    QList<DsTable::SchemaField> testSchema;
    path = {"path", DsTable::TEXT };
    cmd = {"cmd", DsTable::TEXT };
    testSchema << path << cmd;

    // note: path will be the first key, cmd the fourth after counter and timestamp

    dtf.initTable("tblTestDsTableFav", testSchema, testDB);

    // **********************************************************************
    // we start with five items in the database to see if partial lists work
    // as expected

    for(int i=0; i<5; i++){
        record["path"] = "path" + QString::number(i);
        record["cmd"]  = "cmd"  + QString::number(i);
        dtf.insertRecord(record);
        QThread::msleep(2); // timestams need to be sequential for this test to work
    };

    QList<DsTable::Record> recent10 = dtf.getRecent10();
    QVERIFY(recent10.size() == 5);
    QVERIFY(recent10.at(0).value("path") == "path4");
    QVERIFY(recent10.at(4).value("path") == "path0");

    QList<DsTable::Record> top10 = dtf.getTop10();
    QVERIFY(top10.size() == 5);
    // we can't make any good assertions on the sorting of the toplist,
    // as all records have the same count.

    // but we can bump one record to the top
    record["path"] = "path2";
    record["cmd"] = "cmd2";
    dtf.insertRecord(record);

    top10 = dtf.getTop10();
    QVERIFY(top10.size() == 5);
    QVERIFY(top10.at(0).value("path") == "path2");

    recent10 = dtf.getRecent10();
    QVERIFY(recent10.size() == 5);
    QVERIFY(recent10.at(0).value("path") == "path2");

    dtf.clearRecords();

    for(int i=15; i>0; i--){
        record["path"] = "path" + QString::number(i);
        record["cmd"]  = "cmd"  + QString::number(i);
        for(int j=0; j<i*2; j++)
            dtf.insertRecord(record);
        QThread::msleep(2); // timestams need to be sequential for this test to work
    };
    top10 = dtf.getTop10();
    recent10 = dtf.getRecent10();
    QVERIFY(top10.size() == 10);
    QVERIFY(recent10.size() == 10);
    QVERIFY(top10.at(0).value("path") == "path15");
    QVERIFY(recent10.at(0).value("path") == "path1");
}
