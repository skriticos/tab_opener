#ifndef TEST_H
#define TEST_H

#include <QtCore>

#include "dstable.h"

class Test : public QObject
{
    Q_OBJECT

public:
    explicit Test(QObject *parent = 0);
    void run();

signals:

public slots:

};

#endif // TEST_H
