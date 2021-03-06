#ifndef TEST_H
#define TEST_H

#include <QtCore>
#include <QtTest>

#include "util.h"
#include "dstable.h"
#include "dstablefav.h"
#include "historywidget.h"

class Test : public QObject
{
    Q_OBJECT

public:
    explicit Test(QObject *parent = 0);
    void run();

private slots:
    void testUtilExecDetachedCommand();
    void testDsTable();
    void testDsTableFav();
    void testHistoryWidget();
};

#endif // TEST_H
