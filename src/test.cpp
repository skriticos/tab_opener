#include "test.h"

Test::Test(QObject *parent) : QObject(parent)
{
}

void Test::run()
{
    char *dummy_args[] = { const_cast<char *>("dummyname"), NULL };
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
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE", "testDb");
    db.setDatabaseName(":memory:");
    db.open();

    DsTable::SchemaField col1 = {"col1", DsTable::TEXT};
    DsTable::SchemaField col2 = {"col2", DsTable::INTEGER};
    QList<DsTable::SchemaField> schema1;
    schema1 << col1 << col2;

    DsTable *dt1 = new DsTable("tblTest", schema1, db);
    QVERIFY(dt1->size() == 0);
    QVERIFY(dt1->contains("foo") == false);

    DsTable::Record record;
    record.insert("col1", "foo");
    record.insert("col2", 10);
    dt1->insertRecord(record);
    QVERIFY(dt1->size() == 1);
    QVERIFY(dt1->contains("foo") == true);

    QStringList recordKeys = dt1->getRecordKeys();
    qDebug() << recordKeys.size();
    QVERIFY(recordKeys.size() == 1);
    QVERIFY(recordKeys.first() == "foo");

    DsTable::Record record2 = dt1->getRecord("foo");
    QVERIFY(record2.value("col2").toInt() == 10);

    dt1->deleteRecord("foo");
    QVERIFY(dt1->size() == 0);
    QVERIFY(dt1->contains("foo") == false);

    db.close();
}

void Test::testDsTableFav()
{
}

void Test::testHistoryWidget()
{
}

