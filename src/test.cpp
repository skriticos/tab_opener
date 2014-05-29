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
}

void Test::testDsTableFav()
{
}

void Test::testHistoryWidget()
{
}

