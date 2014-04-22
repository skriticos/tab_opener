#include "filelist.h"
#include "ui_filelist.h"

FileList::FileList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileList)
{
    ui->setupUi(this);
}

FileList::~FileList()
{
    delete ui;
}
