#ifndef CMDEXECBUTTON_H
#define CMDEXECBUTTON_H

#include <QtCore>
#include <QtWidgets>
#include <QMovie>
#include "hoverbutton.h"

class CmdExecButton : public HoverButton
{
    Q_OBJECT

public:
    explicit CmdExecButton(QWidget *parent = 0);

public slots:
    void slotProcExecStarted();
    void slotProcExecStopped();

private slots:
    void _slotSetExecIconFrame();

private:
    QMovie *execAnimation;
};

#endif // CMDEXECBUTTON_H
