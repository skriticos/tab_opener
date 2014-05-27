#include "cmdexecbutton.h"

CmdExecButton::CmdExecButton(QWidget *parent) : HoverButton(parent)
{
    this->execAnimation = new QMovie(":/images/rc/exec.gif");
    connect(this->execAnimation, SIGNAL(frameChanged(int)), this, SLOT(_slotSetExecIconFrame()));
}

void CmdExecButton::slotProcExecStarted()
{
    this->execAnimation->start();
}

void CmdExecButton::slotProcExecStopped()
{
    this->execAnimation->stop();
    this->setIcon(QIcon(":/images/rc/play.png"));
}

void CmdExecButton::_slotSetExecIconFrame()
{
    this->setIcon(QIcon(this->execAnimation->currentPixmap()));
}
