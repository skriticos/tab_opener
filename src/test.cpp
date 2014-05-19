#include "test.h"

Test::Test(QObject *parent) : QObject(parent)
{
}

void Test::run()
{
    char* dummy_args[] = { const_cast<char *>("dummyname"), NULL };
    QTest::qExec(this, 1, dummy_args);
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
