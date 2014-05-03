#include "commandbutton.h"

CommandButton::CommandButton(QWidget *parent) : HoverButton(parent)
{
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
}

void CommandButton::setCommand(QString cmd, QString wd)
{
    this->command = cmd;
    this->workingDirectory = wd;

    QString btnLabel;
    if (cmd.size() > 35)
        btnLabel = "..." + cmd.right(35);
    else
        btnLabel = cmd;
    this->setText(btnLabel);
}

void CommandButton::onToggled(bool down)
{
    if (down){
        if(!this->text().isEmpty())
            emit myToggled(this->command, this->workingDirectory, this);
        else {
            this->toggle();
        }
    }
}
