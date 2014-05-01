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

    this->popularButtons.append(this->ui->wflpb0);
    this->popularButtons.append(this->ui->wflpb1);
    this->popularButtons.append(this->ui->wflpb2);
    this->popularButtons.append(this->ui->wflpb3);
    this->popularButtons.append(this->ui->wflpb4);
    this->popularButtons.append(this->ui->wflpb5);
    this->popularButtons.append(this->ui->wflpb6);
    this->popularButtons.append(this->ui->wflpb7);
    this->popularButtons.append(this->ui->wflpb8);
    this->popularButtons.append(this->ui->wflpb9);

    for(int i=0; i<10; i++) {
        FileButton *fb = this->recentButtons.at(i);
        connect(fb, SIGNAL(myToggled(QString, FileButton*)), this, SLOT(buttonSelected(QString, FileButton*)));

        fb = this->popularButtons.at(i);
        connect(fb, SIGNAL(myToggled(QString, FileButton*)), this, SLOT(buttonSelected(QString, FileButton*)));
    }

    this->selectedButton = NULL;
}

void FileList::init(DataStore *ds)
{
    this->ds = ds;
    this->update();
}

void FileList::update()
{
    // update file list (invoked in main after opening / editing a file from browser or from this for the same)
    // we start off by clearing all the labels

    // setup recent files
    for (int i=0; i<10; i++)
        this->recentButtons.at(i)->setPath("");

    int recentFileCount = ds->getRecentFileCount();
    for (int i=0; i<recentFileCount; i++) {
        QString path = ds->getRecentFile(i);
        this->recentButtons.at(i)->setPath(path);
    }

    // setup popular files
    for (int i=0; i<10; i++)
        this->popularButtons.at(i)->setPath("");

    int popularFileCount = ds->getPopularFileCount();
    int count = 10;
    if (popularFileCount < 10)
        count = popularFileCount;
    for (int i=0; i<count; i++){
        QString path = ds->getPopularFile(i);
        this->popularButtons.at(i)->setPath(path);
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

void FileList::on_wflab_file_path_clicked()
{
    QFileInfo fi = QFileInfo(this->selectedFilePath);
    QString basePath = fi.absolutePath();
    emit this->forwardPath(basePath);
}

void FileList::on_wflab_file_copy_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(this->selectedFilePath);
}
