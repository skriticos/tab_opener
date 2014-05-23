#include "util.h"

Util::Util(QObject *parent) : QObject(parent)
{
}

Util::ParsedCommand Util::parseCmdStr(QString cmdStr)
{
    ParsedCommand parsedCmd;
    QStringList cmdList, args;
    QString argbuf;
    bool quoted = false;

    Q_ASSERT(!cmdStr.isEmpty());

    cmdList = cmdStr.split(" ");
    parsedCmd.program = cmdList.first();
    if(cmdList.size()>1)
        args = cmdList.mid(1);

    // we merge quoted arguments
    for(int i=0; i<args.length(); i++){
        if(args.at(i).startsWith("\"")){
            quoted = true;
            argbuf += args.at(i).mid(1);
            if(args.at(i).endsWith("\"")){
                argbuf.chop(1);
                parsedCmd.args << argbuf;
                argbuf.clear();
                quoted = false;
            }
        } else if(quoted){
            argbuf += " " + args.at(i);
            if(args.at(i).endsWith("\"")){
                argbuf.chop(1);
                parsedCmd.args << argbuf;
                argbuf.clear();
                quoted = false;
            }
        } else {
            parsedCmd.args << args.at(i);
        }
    }

    return parsedCmd;
}

bool Util::execDetachedCommand(QString cmdStr, QString wd)
{
    // empty command is bad
    if(cmdStr.isEmpty())
        return false;

    ParsedCommand parsedCmd = Util::parseCmdStr(cmdStr);

    if(wd.isEmpty())
        wd = QDir::homePath();

    return QProcess::startDetached(parsedCmd.program, parsedCmd.args, wd);
}
