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
    QString homePath = QDir::homePath();
    dirmodel = new QFileSystemModel(this);
    filemodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    ui->wb_folders->setModel(dirmodel);
    ui->wb_files->setModel(filemodel);
    ui->wb_folders->setRootIndex(dirmodel->setRootPath("/"));
    ui->wb_folders->setCurrentIndex(dirmodel->index(homePath));
    ui->wb_files->setRootIndex(filemodel->setRootPath(homePath));
    ui->wb_folders->setExpanded(dirmodel->index(homePath), true);
    ui->wb_folders->hideColumn(3);
    ui->wb_folders->hideColumn(2);
    ui->wb_folders->hideColumn(1);

    myFileExtOpenEdit["html"] = QStringList() << "/usr/bin/google-chrome %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["css"] = QStringList() << "/usr/bin/google-chrome %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["js"] = QStringList() << "/usr/bin/google-chrome %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["svg"] = QStringList() << "/usr/bin/google-chrome %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["go"] = QStringList() << "/usr/bin/go run %F" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["h"] = QStringList() << "" << "/usr/bin/gvim %F";
    myFileExtOpenEdit["cpp"] = QStringList() << "" << "/usr/bin/gvim %F";
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
