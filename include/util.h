#ifndef UTIL_H
#define UTIL_H

#include <QtCore>

class Util : public QObject
{
    Q_OBJECT
public:
    struct ParsedCommand {
        QString program;
        QStringList args;
    };

    explicit Util(QObject *parent = 0);

    static ParsedCommand parseCmdStr(QString cmdStr);
    static bool execDetachedCommand(QString cmdStr, QString wd = "");
};

#endif // UTIL_H
