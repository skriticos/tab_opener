#ifndef UTIL_H
#define UTIL_H

#include <QtCore>

class Util : public QObject
{
    Q_OBJECT
public:
    explicit Util(QObject *parent = 0);

    static bool execDetachedCommand(QString cmdStr, QString wd = "");

signals:

public slots:

};

#endif // UTIL_H
