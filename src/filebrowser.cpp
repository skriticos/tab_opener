#include "filebrowser.h"
#include "ui_filebrowser.h"

FileBrowser::FileBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileBrowser)
{
    ui->setupUi(this);
}

FileBrowser::~FileBrowser()
{
    delete ui;
}
