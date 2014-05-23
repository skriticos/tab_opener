#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    // window
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    // keyboard shortcuts
    this->shortEsc = new QShortcut(QKeySequence(tr("Esc")), this);
    connect(shortEsc, SIGNAL(activated()), this, SLOT(onEscPressed()));

    connect(ui->wprb0, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb1, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb2, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb3, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb4, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb5, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb6, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb7, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb8, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
    connect(ui->wprb9, SIGNAL(presetClicked(QString)), ui->fileBrowser, SLOT(setSelectedFolder(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWidget(DataStore *ds)
{
    this->ds = ds;

    // file and command quicklists
    ui->wfileinner->init(ds);
    connect(this->ui->wfileinner, SIGNAL(openOrEditClicked()), this, SLOT(close()));

    ui->wcmdinner->init(ds);
    // TODO: properly connect cmdlist widget
    // connect(ui->wcmdinner, SIGNAL(commandSelected(QString,QString)),
    //         this, SLOT(setCommand(QString,QString)));

    // presets
    this->updatePresets();

    connect(ui->commandWidget, SIGNAL(commandChanged(QString)),
            ui->notesWidget,   SLOT(commandChanged(QString)));

    connect(ui->commandWidget, SIGNAL(processExecutionStarted()),
            ui->fileBrowser,   SLOT(commandProcessStarted()));
    connect(ui->commandWidget, SIGNAL(processExecutionStopped()),
            ui->fileBrowser,   SLOT(commandProcessStopped()));
    connect(ui->fileBrowser,   SIGNAL(folderSelected(QString)),
            ui->commandWidget, SLOT(selectedFolderChanged(QString)));
    connect(ui->fileBrowser,   SIGNAL(execCommand(QString)),
            ui->commandWidget, SLOT(execCmd(QString)));
    connect(ui->fileBrowser,   SIGNAL(execMultiCommand(QStringList)),
            ui->commandWidget, SLOT(execMultiCmds(QStringList)));

    connect(ui->fileBrowser, SIGNAL(fileSelected(QString)),
            ui->notesWidget, SLOT  (selectedFileChanged(QString)));

    connect(ui->fileBrowser, SIGNAL(actPriSecTriggered()),
            this,            SLOT  (close()));

    connect(ui->wfileinner,  SIGNAL(fileSelected(QString)),
            ui->fileBrowser, SLOT  (setSelectedFile(QString)));

    this->ui->notesWidget->initWidget(this->ds);
    this->ui->fileBrowser->initFileBrowser(this->ds);
    this->ui->commandWidget->initCommandWidget(this->ds);
}

void MainWindow::updatePresets()
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

void MainWindow::onEscPressed()
{
    if(ui->commandWidget->processExecuting()){
        int ret = QMessageBox::warning(
                    this,
                    "Command still running",
                    "A command is still running. Exiting now will abort the process."
                    "Are you sure you want to exit now?",
                    QMessageBox::Abort, QMessageBox::Cancel);
        if(ret == QMessageBox::Abort)
            this->close();
    } else {
        this->close();
    }
}
