#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ds = new DataStore(this);
    setPresets();
    wconfig = new ConfigWidget(ds, this);
    connect(wconfig, SIGNAL(accepted()), this, SLOT(setPresets()));

    shortEsc = new QShortcut(QKeySequence(tr("Esc")), this);
    connect(shortEsc, SIGNAL(activated()), this, SLOT(close()));

    // setup navi
    QString homePath = QDir::homePath();
    dirmodel = new QFileSystemModel(this);
    filemodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    // setup location
    ui->wb_folders->setModel(dirmodel);
    ui->wb_files->setModel(filemodel);
    ui->wb_folders->setRootIndex(dirmodel->setRootPath("/"));
    ui->wb_folders->setCurrentIndex(dirmodel->index(homePath));
    ui->wb_files->setRootIndex(filemodel->setRootPath(homePath));
    // setup view properties
    ui->wb_folders->setExpanded(dirmodel->index(homePath), true);
    ui->wb_folders->hideColumn(3);
    ui->wb_folders->hideColumn(2);
    ui->wb_folders->hideColumn(1);

    setPath(QDir::homePath());

    connect(ui->wpc_root, SIGNAL(clicked()), this, SLOT(setRootPath()));

    connect(ui->wprb0, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb1, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb2, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb3, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb4, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb5, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb6, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb7, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb8, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));
    connect(ui->wprb9, SIGNAL(presetClicked(QString)), this, SLOT(setPath(QString)));

    ui->we_notes->setPlainText(ds->getNotes());
    connect(ui->we_notes, SIGNAL(textChanged()), this, SLOT(notesChanged()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPresets()
{
    ui->wprb0->setText(ds->getPreset(0));
    ui->wprb1->setText(ds->getPreset(1));
    ui->wprb2->setText(ds->getPreset(2));
    ui->wprb3->setText(ds->getPreset(3));
    ui->wprb4->setText(ds->getPreset(4));
    ui->wprb5->setText(ds->getPreset(5));
    ui->wprb6->setText(ds->getPreset(6));
    ui->wprb7->setText(ds->getPreset(7));
    ui->wprb8->setText(ds->getPreset(8));
    ui->wprb9->setText(ds->getPreset(9));
}

void MainWindow::setRootPath()
{
    this->setPath(QDir::rootPath());
}

void MainWindow::setPath(QString path)
{
    this->path = path;

    // split path into directories
    QStringList charmParts = path.split(QDir::separator());

    ui->wb_folders->setCurrentIndex(dirmodel->index(path));
    ui->wb_files->setRootIndex(filemodel->setRootPath(path));
    ui->wb_folders->setExpanded(dirmodel->index(path), true);

    // clear previous buttons and labels
    for (int i=cblist.size()-1; i>=0; i--){
        ui->horizontalLayout_2->removeWidget(cblist.at(i));
        delete this->cblist.at(i);
        cblist.removeAt(i);
    }
    for (int i=cllist.size()-1; i>=0; i--){
        ui->horizontalLayout_2->removeWidget(cllist.at(i));
        delete this->cllist.at(i);
        cllist.removeAt(i);
    }

    // add charm buttons to charm navigation
    if (path != "/") {
        for (int i=1; i<charmParts.size(); i++){
            QString p;
            for (int j=1; j<=i; j++)
                p += "/" + charmParts.at(j);
            CharmButton *b = new CharmButton(charmParts.at(i), p, ui->wpcharms);
            this->cblist.append(b);
            connect(b, SIGNAL(charmClicked(QString)), this, SLOT(setPath(QString)));

            int k = ui->horizontalLayout_2->count();
            ui->horizontalLayout_2->insertWidget(k-1, b);
            if (i < charmParts.size()-1){
                QLabel *l = new QLabel("/", ui->wpcharms);
                this->cllist.append(l);
                ui->horizontalLayout_2->insertWidget(k, l);
            }
        }
    }
}

void MainWindow::on_config_presets_clicked()
{
    wconfig->show();
}

void MainWindow::charmClicked(QString path)
{
    this->setPath(path);
}

void MainWindow::on_wpb_home_clicked()
{
   setPath(QDir::homePath());
}

void MainWindow::on_wb_folders_clicked(const QModelIndex &index)
{
   QString path = dirmodel->fileInfo(index).absoluteFilePath();
   delete filemodel;
   filemodel = new QFileSystemModel(this);
   ui->wb_files->setModel(filemodel);
   filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
   ui->wb_files->setRootIndex(filemodel->setRootPath(path));
   this->setPath(path);
}

void MainWindow::notesChanged()
{
    ds->setNotes(ui->we_notes->toPlainText());
}

void MainWindow::on_wpb_terminal_clicked()
{
    QProcess *p = new QProcess(this);
    QStringList sl;
    sl = ds->getTerminalEmulator().split(" ");
    QString cmd = sl.at(0);
    sl.removeAt(0);
    sl << this->path;
    p->start(cmd, QStringList() << sl);
}

void MainWindow::on_wpb_folder_clicked()
{
    QProcess *p = new QProcess(this);
    QStringList sl;
    sl = ds->getFileBrowser().split(" ");
    QString cmd = sl.at(0);
    sl.removeAt(0);
    sl << this->path;
    p->start(cmd, QStringList() << sl);
}

void MainWindow::on_view_file_clicked()
{
    // get currently selected file
    QModelIndex index = ui->wb_files->selectionModel()->selectedIndexes().at(0);
    QString selectedFilePath = filemodel->fileInfo(index).absoluteFilePath();

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
}

void MainWindow::on_edit_file_clicked()
{
    // get currently selected file
    QModelIndex index = ui->wb_files->selectionModel()->selectedIndexes().at(0);
    QString selectedFilePath = filemodel->fileInfo(index).absoluteFilePath();

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
}
