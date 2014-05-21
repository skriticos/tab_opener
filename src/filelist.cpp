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
    // clear all buttons
    for(int i=0; i<10; i++){
        this->recentButtons.at(i)->setPath("");
        this->popularButtons.at(i)->setPath("");
    }

    // read database and set button labels
    QList<DsTable::Record> recent = ds->tblFiles->getRecent10();
    QList<DsTable::Record> top    = ds->tblFiles->getTop10();

    for(int i=0; i<recent.size(); i++)
        this->recentButtons.at(i)->setPath(recent.at(i).value("path").toString());
    for(int i=0; i<top.size(); i++)
        this->popularButtons.at(i)->setPath(top.at(i).value("path").toString());
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
    QString extStr = selectedFilePath.split(".").last();

    // prepare command string and arguments
    DsTable::Record record = ds->tblExtensions->getRecord(extStr);
    QString extActPri = record.value("ext_act_pri").toString();

    QStringList rawCmd = extActPri.split(" ");
    QString prog = rawCmd.first();
    QStringList args = rawCmd.mid(1);
    args << selectedFilePath;

    // execute open command
    QProcess p;
    p.startDetached(prog, args);

    // push file to stack
    ds->setFile(selectedFilePath);

    emit this->openOrEditClicked();
}

void FileList::on_wflab_file_edit_clicked()
{
    // extract filetype
    QString extStr = selectedFilePath.split(".").last();

    // prepare command string and arguments
    DsTable::Record record = ds->tblExtensions->getRecord(extStr);
    QString extActSec = record.value("ext_act_sec").toString();
    QString prog = extActSec.split(" ").at(0);
    QStringList args = QStringList() << extActSec.split(" ").mid(1) << selectedFilePath;

    // execute open command
    QProcess *p = new QProcess(this);
    p->startDetached(prog, args);

    // push file to stack
    ds->setFile(selectedFilePath);

    emit this->openOrEditClicked();
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
