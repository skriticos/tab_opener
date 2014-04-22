#include "cmdlist.h"
#include "ui_cmdlist.h"

CmdList::CmdList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CmdList)
{
    ui->setupUi(this);
}

CmdList::~CmdList()
{
    delete ui;
}
