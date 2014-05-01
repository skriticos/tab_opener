#include "filelist.h"
#include "ui_filelist.h"

FileList::FileList(QWidget *parent) : QWidget(parent), ui(new Ui::FileList)
{
    ui->setupUi(this);

    this->recentButtons.append(this->ui->wflrb0);
    this->recentButtons.append(this->ui->wflrb1);
    this->recentButtons.append(this->ui->wflrb2);
    this->recentButtons.append(this->ui->wflrb3);
    this->recentButtons.append(this->ui->wflrb4);
    this->recentButtons.append(this->ui->wflrb5);
    this->recentButtons.append(this->ui->wflrb6);
    this->recentButtons.append(this->ui->wflrb7);
    this->recentButtons.append(this->ui->wflrb8);
    this->recentButtons.append(this->ui->wflrb9);

    for(int i=0; i<10; i++) {
        FileButton *fb = this->recentButtons.at(i);
        connect(fb, SIGNAL(myToggled(QString, FileButton*)), this, SLOT(buttonSelected(QString, FileButton*)));
    }

    this->selectedButton = NULL;
}

void FileList::init(DataStore *ds)
{
    this->ds = ds;

    int recentFileCount = ds->getRecentFileCount();
    for (int i=0; i<recentFileCount; i++) {
        QString path = ds->getRecentFile(i);
        this->recentButtons.at(i)->setPath(path);
    }
}

void FileList::update()
{
    // update file list (invoked in main after opening / editing a file from browser or from this for the same)
    // we start off by clearing all the labels
    for (int i=0; i<10; i++)
        this->recentButtons.at(i)->setPath("");

    int recentFileCount = ds->getRecentFileCount();
    for (int i=0; i<recentFileCount; i++) {
        QString path = ds->getRecentFile(i);
        this->recentButtons.at(i)->setPath(path);
    }
}

FileList::~FileList()
{
    delete ui;
}

void FileList::buttonSelected(QString path, FileButton *butt)
{
    if(this->selectedButton != NULL) {
        this->selectedButton->toggle();
    }
    this->selectedFilePath = path;
    this->selectedButton = butt;
}

void FileList::on_wflab_file_view_clicked()
{
    // extract filetype
    QString extension = selectedFilePath.split(".").last();

    // prepare command string and arguments
    QString tmp = ds->getOpenAppsItem(extension);
    QString cmd = tmp.split(" ").at(0);
    QStringList tmp2 = tmp.split(" ");
    tmp2.removeFirst();
    QStringList args = QStringList() << tmp2 << selectedFilePath;

    // execute open command
    QProcess *p = new QProcess(this);
    p->start(cmd, args);

    // push file to stack
    ds->pushRecentFile(selectedFilePath);
}

void FileList::on_wflab_file_edit_clicked()
{
    // extract filetype
    QString extension = selectedFilePath.split(".").last();

    // prepare command string and arguments
    QString tmp = ds->getEditMapItem(extension);
    QString cmd = tmp.split(" ").at(0);
    QStringList tmp2 = tmp.split(" ");
    tmp2.removeFirst();
    QStringList args = QStringList() << tmp2 << selectedFilePath;

    // execute open command
    QProcess *p = new QProcess(this);
    p->start(cmd, args);

    // push file to stack
    ds->pushRecentFile(selectedFilePath);
}
