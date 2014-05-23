#include "cmdexecbutton.h"

CmdExecButton::CmdExecButton(QWidget *parent) : HoverButton(parent)
{
    this->execAnimation = new QMovie(":/images/rc/exec.gif");
    connect(this->execAnimation, SIGNAL(frameChanged(int)), this, SLOT(setExecFrame()));
}

void CmdExecButton::processExecutionStarted()
{
    this->execAnimation->start();
}

void CmdExecButton::processExecutionStopped()
{
    this->execAnimation->stop();
    this->setIcon(QIcon(":/images/rc/play.png"));
}

void CmdExecButton::setExecFrame()
{
    this->setIcon(QIcon(this->execAnimation->currentPixmap()));
}
