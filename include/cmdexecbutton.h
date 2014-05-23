#ifndef CMDEXECBUTTON_H
#define CMDEXECBUTTON_H

#include <QtCore>
#include <QWidget>
#include <QMovie>
#include "hoverbutton.h"

class CmdExecButton : public HoverButton
{
    Q_OBJECT

public:
    explicit CmdExecButton(QWidget *parent = 0);

public slots:
    void processExecutionStarted();
    void processExecutionStopped();

private slots:
    void setExecFrame();

private:
    QMovie *execAnimation;
};

#endif // CMDEXECBUTTON_H
