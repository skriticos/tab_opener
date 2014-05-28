#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(DataStore *ds, QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    // window
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    // keyboard shortcuts
    connect(new QShortcut(QKeySequence(tr("Esc")), this), SIGNAL(activated()), this, SLOT(slotEscPressed()));

    // configWidget
    this->configWidget = new ConfigWidget(this);

    // history widget
    ui->fileHistory->setType(History::FILEHISTORY);
    ui->commandHistory->setType(History::COMMANDHISTORY);

    // datastore
    connect(ds, SIGNAL(sigInitConfig(QStringList,QList<Config::ExtensionEntry>,QString,QString)),
            this->configWidget, SLOT(slotInitConfig(QStringList,QList<Config::ExtensionEntry>,QString,QString)));
    connect(ds, SIGNAL(sigUpdateCommandHistory(QList<History::Entry>,QList<History::Entry>)),
            ui->commandHistory, SLOT(slotUpdateWidget(QList<History::Entry>,QList<History::Entry>)));
    connect(ds, SIGNAL(sigUpdateFileHistory(QList<History::Entry>,QList<History::Entry>)),
            ui->fileHistory, SLOT(slotUpdateWidget(QList<History::Entry>,QList<History::Entry>)));
    connect(ds, SIGNAL(sigUpdatePresets(QStringList)), ui->presetWidget, SLOT(slotUpdatePresets(QStringList)));
    connect(ds, SIGNAL(sigInitNotesSelection(QString)), ui->notesWidget, SLOT(slotInitSelection(QString)));
    connect(ds, SIGNAL(sigInitGlobalNotes(QString)), ui->notesWidget, SLOT(slotInitGlobalNote(QString)));
    connect(ds, SIGNAL(sigCmdSelectionChanged(QString,QString)),
            ui->notesWidget, SLOT(slotUpdateCommandNote(QString,QString)));
    connect(ds, SIGNAL(sigFileSelectionChanged(QString,QString)),
            ui->notesWidget, SLOT(slotUpdateFileNote(QString,QString)));
    connect(ds, SIGNAL(sigInitCommand(QString)), ui->commandWidget, SLOT(slotUpdateCmd(QString)));

    // config widget
    connect(this->configWidget, SIGNAL(sigPresetsChanged(QStringList)), ds, SLOT(slotCfgPresetsChanged(QStringList)));
    connect(this->configWidget, SIGNAL(sigExtensionChanged(Config::ExtensionEntry)),
            ds, SLOT(slotCfgExtensionChanged(Config::ExtensionEntry)));
    connect(this->configWidget, SIGNAL(sigExtensionDeleted(QString)), ds, SLOT(slotCfgExtensionDeleted(QString)));
    connect(this->configWidget, SIGNAL(sigTerminalChanged(QString)), ds, SLOT(slotCfgTerminalChanged(QString)));
    connect(this->configWidget, SIGNAL(sigExtFileBrowserChanged(QString)),
            ds, SLOT(slotCfgExtFileBrowserChanged(QString)));

    connect(this->configWidget, SIGNAL(sigTerminalChanged(QString)),
            ui->fileBrowser, SLOT(slotTerminalEmulatorChanged(QString)));
    connect(this->configWidget, SIGNAL(sigExtFileBrowserChanged(QString)),
            ui->fileBrowser, SLOT(slotExtFileBrowserChanged(QString));

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
    connect(ui->notesWidget, SIGNAL(sigSelectionChanged(QString)), ds, SLOT(slotNoteSelectionChanged(QString)));

    // commandWidget
    connect(ui->commandWidget, SIGNAL(sigProcStarted()), ui->fileBrowser, SLOT(slotScmOff()));
    connect(ui->commandWidget, SIGNAL(sigProcStopped()), ui->fileBrowser, SLOT(slotScmOn()));
    connect(ui->commandWidget, SIGNAL(sigCmdChanged(QString)), ds, SLOT(slotCmdChanged(QString)));
    connect(ui->commandWidget, SIGNAL(sigCmdChanged(QString)), ui->commandHistory, SLOT(slotCommandSelected(QString)));
    connect(ui->commandWidget, SIGNAL(sigCmdExecuted(QString,QString)), ds, SLOT(slotCommandExecuted(QString,QString)));

    // fileBrowser interactions
    connect(ui->fileBrowser, SIGNAL(sigFolderSelected(QString)), ui->presetWidget, SLOT(slotFolderChanged(QString)));
    connect(ui->fileBrowser, SIGNAL(sigFileSelected(QString)), ui->fileHistory, SLOT(slotFileSelected(QString)));
    connect(ui->fileBrowser, SIGNAL(sigFolderSelected(QString)),
            ui->commandHistory, SLOT(slotWorkingDirectorySelected(QString)));
    connect(ui->fileBrowser, SIGNAL(sigFolderSelected(QString)), ui->commandWidget, SLOT(slotUpdateFolder(QString)));
    connect(ui->fileBrowser, SIGNAL(sigExecCommand(QString)), ui->commandWidget, SLOT(slotExecCmd(QString)));
    connect(ui->fileBrowser, SIGNAL(sigExecMultiCommand(QStringList)),
            ui->commandWidget, SLOT(slotExecMultiCmds(QStringList)));
    connect(ui->fileBrowser, SIGNAL(sigFileSelected(QString)), ds, SLOT(slotSelectedFileChanged(QString)));
    connect(ui->fileBrowser, SIGNAL(sigFileOpened()), this, SLOT(slotRequestClose()));
    connect(ui->fileBrowser, SIGNAL(sigConfigClicked()), this->configWidget, SLOT(show()));

    // send initial data load
    ds->initWidgetData();

    this->ui->fileBrowser->initFileBrowser(ds);
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
