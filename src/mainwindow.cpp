#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ds = new DataStore(this);
    setPresets();
    wconfig = new ConfigWidget(ds, this);
    connect(wconfig, SIGNAL(accepted()), this, SLOT(setPresets()));

    shortEsc = new QShortcut(QKeySequence(tr("Esc")), this);
    connect(shortEsc, SIGNAL(activated()), this, SLOT(close()));
    ui->wpc_edit->setVisible(false);

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

void MainWindow::on_wpb_edit_toggled(bool checked)
{
    ui->wpc_edit->setVisible(checked);
    ui->wpcharms->setVisible(!checked);
}

void MainWindow::on_wb_folders_activated(const QModelIndex &index)
{
   QString path = dirmodel->fileInfo(index).absoluteFilePath();
   ui->wb_files->setRootIndex(filemodel->setRootPath(path));
}

void MainWindow::on_config_presets_clicked()
{
    wconfig->show();
}
