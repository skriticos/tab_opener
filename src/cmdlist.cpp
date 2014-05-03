#include "cmdlist.h"
#include "ui_cmdlist.h"

CmdList::CmdList(QWidget *parent) : QWidget(parent), ui(new Ui::CmdList)
{
    ui->setupUi(this);

    // put button pointers in list to make iterating easy
    this->recentCommandButtons.append(this->ui->wclrb0);
    this->recentCommandButtons.append(this->ui->wclrb1);
    this->recentCommandButtons.append(this->ui->wclrb2);
    this->recentCommandButtons.append(this->ui->wclrb3);
    this->recentCommandButtons.append(this->ui->wclrb4);
    this->recentCommandButtons.append(this->ui->wclrb5);
    this->recentCommandButtons.append(this->ui->wclrb6);
    this->recentCommandButtons.append(this->ui->wclrb7);
    this->recentCommandButtons.append(this->ui->wclrb8);
    this->recentCommandButtons.append(this->ui->wclrb9);

    this->popularCommandButtons.append(this->ui->wclpb0);
    this->popularCommandButtons.append(this->ui->wclpb1);
    this->popularCommandButtons.append(this->ui->wclpb2);
    this->popularCommandButtons.append(this->ui->wclpb3);
    this->popularCommandButtons.append(this->ui->wclpb4);
    this->popularCommandButtons.append(this->ui->wclpb5);
    this->popularCommandButtons.append(this->ui->wclpb6);
    this->popularCommandButtons.append(this->ui->wclpb7);
    this->popularCommandButtons.append(this->ui->wclpb8);
    this->popularCommandButtons.append(this->ui->wclpb9);

    // connect toggled signals for all buttons
    for (int i=0; i<10; i++){
        CommandButton *cb;

        cb = this->recentCommandButtons.at(i);
        connect(cb, SIGNAL(myToggled(QString,QString,CommandButton*)),
                this, SLOT(selected(QString,QString,CommandButton*)));

        cb = this->popularCommandButtons.at(i);
        connect(cb, SIGNAL(myToggled(QString,QString,CommandButton*)),
                this, SLOT(selected(QString,QString,CommandButton*)));
    }

    // init variables
    this->selectedCommandButton = NULL;
}

void CmdList::init(DataStore *ds)
{
    this->ds = ds;
    this->update();
}

void CmdList::update()
{
    // clear buttons
    for (int i=0; i<10; i++){
        this->recentCommandButtons.at(i)->setCommand("", "");
        this->popularCommandButtons.at(i)->setCommand("", "");
    }

    // setup recent buttons
    int cnt = ds->getRecentCommandCount();
    for (int i=0; i<cnt; i++) {
        RunEntry *runEntry = ds->getRecentCommand(i);
        QString cmd = runEntry->command;
        QString wd = runEntry->execPath;
        this->recentCommandButtons.at(i)->setCommand(cmd, wd);
    }

    // setup popular buttons
}

CmdList::~CmdList()
{
    delete ui;
}

void CmdList::selected(QString cmd, QString wd, CommandButton *cb)
{
    if (this->selectedCommandButton != NULL){
        this->selectedCommandButton->toggle();
    }
    this->selectedCommandButton = cb;

    emit this->commandSelected(cmd, wd);
}

void CmdList::on_wclab_copy_clicked()
{

}
