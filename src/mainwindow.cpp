#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    shortEsc = new QShortcut(QKeySequence(tr("Esc")), this);
    connect(shortEsc, SIGNAL(activated()), this, SLOT(close()));
    ui->wpc_edit->setVisible(false);

    // load runtime data

    // setup navi
    QString path = "/";
    dirmodel = new QFileSystemModel(this);
    filemodel = new QFileSystemModel(this);
    dirmodel->setRootPath(path);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    filemodel->setRootPath(path);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    ui->wb_folders->setModel(dirmodel);
    ui->wb_files->setModel(filemodel);
    ui->wb_folders->hideColumn(3);
    ui->wb_folders->hideColumn(2);
    ui->wb_folders->hideColumn(1);
}

MainWindow::~MainWindow()
{
    delete ui;
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
