#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(DataStore *ds, QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    // window
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    // keyboard shortcuts
    this->shortEsc = new QShortcut(QKeySequence(tr("Esc")), this);
    connect(shortEsc, SIGNAL(activated()), this, SLOT(slotEscPressed()));

    ui->fileHistory->setType(History::FILEHISTORY);
    ui->commandHistory->setType(History::COMMANDHISTORY);

    // datastore
    connect(ds, SIGNAL(sigUpdatePresets(QStringList)), ui->presetWidget, SLOT(slotUpdatePresets(QStringList)));
    connect(ds, SIGNAL(sigUpdateCommandHistory(QList<History::Entry>,QList<History::Entry>)),
            ui->commandHistory, SLOT(slotUpdateWidget(QList<History::Entry>,QList<History::Entry>)));
    connect(ds, SIGNAL(sigUpdateFileHistory(QList<History::Entry>,QList<History::Entry>)),
            ui->fileHistory, SLOT(slotUpdateWidget(QList<History::Entry>,QList<History::Entry>)));

    // fileHistory
    connect(ui->fileHistory, SIGNAL(sigSelectedFileChanged(QString)), ui->fileBrowser, SLOT(slotSelectFile(QString)));
    connect(ui->fileHistory, SIGNAL(sigFilePriActRequested(QString)), ui->fileBrowser, SLOT(slotOpenFilePrimary()));
    connect(ui->fileHistory, SIGNAL(sigFileSecActRequested(QString)), ui->fileBrowser, SLOT(slotOpenFileSeconday()));

    // commandHistory
    connect(ui->commandHistory, SIGNAL(sigSelectedCommandChanged(QString)),
            ui->commandWidget, SLOT(slotUpdateCmd(QString)));
    connect(ui->commandHistory, SIGNAL(sigSelectedFolderChanged(QString)),
            ui->fileBrowser, SLOT(slotSelectFolder(QString)));

    // presetWidget interactions
    connect(ui->presetWidget, SIGNAL(sigFolderChanged(QString)), ui->fileBrowser, SLOT(slotSelectFolder(QString)));

    // notesWidget

    // commandWidget
    connect(ui->commandWidget, SIGNAL(sigProcStarted()), ui->fileBrowser, SLOT(slotScmOff()));
    connect(ui->commandWidget, SIGNAL(sigProcStopped()), ui->fileBrowser, SLOT(slotScmOn()));
    connect(ui->commandWidget, SIGNAL(sigCmdChanged(QString)), ui->notesWidget, SLOT(slotUpdateCmd(QString)));

    // fileBrowser interactions
    connect(ui->fileBrowser, SIGNAL(sigFolderSelected(QString)), ui->presetWidget, SLOT(slotFolderChanged(QString)));
    connect(ui->fileBrowser, SIGNAL(sigFileSelected(QString)), ui->fileHistory, SLOT(slotFileSelected(QString)));
    connect(ui->fileBrowser, SIGNAL(sigFolderSelected(QString)),
            ui->commandHistory, SLOT(slotWorkingDirectorySelected(QString)));
    connect(ui->fileBrowser, SIGNAL(sigFolderSelected(QString)), ui->commandWidget, SLOT(slotUpdateFolder(QString)));
    connect(ui->fileBrowser, SIGNAL(sigExecCommand(QString)), ui->commandWidget, SLOT(slotExecCmd(QString)));
    connect(ui->fileBrowser, SIGNAL(sigExecMultiCommand(QStringList)),
            ui->commandWidget, SLOT(slotExecMultiCmds(QStringList)));
    connect(ui->fileBrowser, SIGNAL(sigFileSelected(QString)), ui->notesWidget, SLOT(slotUpdateFile(QString)));
    connect(ui->fileBrowser, SIGNAL(sigFileOpened()), this, SLOT(slotRequestClose()));

    // send initial data load
    ds->initWidgets();

    this->ui->notesWidget->initWidget(ds);
    this->ui->fileBrowser->initFileBrowser(ds);
    this->ui->commandWidget->initCommandWidget(ds);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotEscPressed()
{
    if(ui->commandWidget->isExecuting()){
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

void MainWindow::slotRequestClose()
{
    if(!ui->commandWidget->isExecuting()){
        this->close();
    }
}
