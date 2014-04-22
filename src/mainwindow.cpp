#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    shortEsc = new QShortcut(QKeySequence(tr("Esc")), this);
    connect(shortEsc, SIGNAL(activated()), this, SLOT(close()));
    ui->wpc_edit->setVisible(false);
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
