#ifndef COMMANDBUTTON_H
#define COMMANDBUTTON_H

#include <QDebug>

#include "hoverbutton.h"

class CommandButton : public HoverButton
{
    Q_OBJECT
public:
    explicit CommandButton(QWidget *parent = 0);
    void setCommand(QString cmd, QString wd);

signals:
    void myToggled(QString command, QString workingDirectory, CommandButton *cb);

public slots:
    void onToggled(bool down);

private:
    QString command;
    QString workingDirectory;

};

#endif // COMMANDBUTTON_H
