#include "util.h"

Util::Util(QObject *parent) : QObject(parent)
{
}

bool Util::execDetachedCommand(QString cmdStr, QString wd)
{
    QStringList cmdList, args, args2;
    QString prog, argbuf;
    bool quoted = false;

    // empty command is bad
    if(cmdStr.isEmpty())
        return false;

    cmdList = cmdStr.split(" ");
    prog = cmdList.first();
    if(cmdList.size()>1)
        args = cmdList.mid(1);

    // we merge quoted arguments
    for(int i=0; i<args.length(); i++){
        if(args.at(i).startsWith("\"")){
            quoted = true;
            argbuf += args.at(i).mid(1);
            if(args.at(i).endsWith("\"")){
                argbuf.chop(1);
                args2 << argbuf;
                argbuf.clear();
                quoted = false;
            }
        } else if(quoted){
            argbuf += " " + args.at(i);
            if(args.at(i).endsWith("\"")){
                argbuf.chop(1);
                args2 << argbuf;
                argbuf.clear();
                quoted = false;
            }
        } else {
            args2 << args.at(i);
        }
    }

    if(wd.isEmpty())
        wd = QDir::homePath();

    return QProcess::startDetached(prog, args2, wd);
}
