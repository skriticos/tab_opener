#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief Initializes the main window
 *
 * - sets up variables
 * - sets up the datastore
 * - sets up the preferences widget (configwidget, this->wconfig)
 * - sets up the file navigation widgets (ui->wb_folders, ui->wb_files)
 * - sets up the presets (bookmarks)
 * - sets up the global notes
 * - connects signals
 */
MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    this->isInit = true;

    // window
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    // datastore
    this->ds = new DataStore(this);

    // variables
    this->processRunnning = false;

    // keyboard shortcuts
    this->shortEsc = new QShortcut(QKeySequence(tr("Esc")), this);
    connect(shortEsc, SIGNAL(activated()), this, SLOT(close()));

    // file and command quicklists
    ui->wfileinner->init(ds);
    // TODO: add signal to filelist that sends selected file path to connect with filebrowser
    connect(this->ui->wfileinner, SIGNAL(openOrEditClicked()), this, SLOT(close()));

    ui->wcmdinner->init(ds);
    connect(ui->wcmdinner, SIGNAL(commandSelected(QString,QString)),
            this, SLOT(setCommand(QString,QString)));

    // presets
    this->updatePresets();

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

    this->ui->notesWidget->initWidget(this->ds);
    this->ui->fileBrowser->initFileBrowser(this->ds);

    connect(ui->fileBrowser, SIGNAL(fileSelected(QString)),
            ui->notesWidget, SLOT  (selectedFileChanged(QString)));

    if(ds->tblGeneral->contains("current_command"))
        ui->wer_cmd->setText(ds->getGeneralValue("current_command"));

    this->isInit = false;
}

/**
 * @brief Convenience method to execute commands
 * @param cmd command and arguments that are to be executed
 *
 * example: MainWindow::exec(QStringList() << "/bin/ls" << "/home/me");
 */
void MainWindow::exec(QStringList cmd)
{
    if (this->processRunnning)
        QMessageBox::warning(this, "Previous command still running",
                             "Refusing to execute command while previous command is still running",
                             QMessageBox::Ok);

    // prepare command
    QString prog = cmd.at(0);
    QStringList args = cmd;
    args.removeFirst();

    // crear terminal output
    this->ui->we_output->clear();
    this->ui->we_output->append(cmd.join(" "));

    // run process
    QProcess *process = new QProcess(this);
    this->guiCmdProcess = process;
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(onMyStdoutReadyRead()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(onMyStderrReadReady()));
    connect(process, SIGNAL(finished(int)), this, SLOT(onMyProcessFinished(int)));
    this->processRunnning = true;
    this->ui->wer_cmd->clear();
    process->setWorkingDirectory(ui->fileBrowser->getSelectedFolder());

    this->ui->wcmdinner->update();

    process->start(prog, args);
    // rest is handled by callbacks
}

/**
 * @brief Execute multiple commands in a row
 * @param commands list of commands that are to be executed
 *
 * This method executes multiple commands in a row and writes output to we_output.
 * It is blocking. Each process has 1 minute to finish.
 */
void MainWindow::multiExec(QList<QStringList> commands)
{
    if (this->processRunnning)
        QMessageBox::warning(this, "Previous command still running",
                             "Refusing to execute command while previous command is still running",
                             QMessageBox::Ok);

    this->ui->we_output->clear();
    this->ui->we_output->append("executing multiple commands..");
    for (int i=0; i<commands.size(); i++){
        this->ui->we_output->append("% " + commands.at(i).join(" "));
        QProcess *proc = new QProcess(this);
        QString cmd = commands.at(i).at(0);
        QStringList args = commands.at(i); args.removeFirst();
        proc->setWorkingDirectory(ui->fileBrowser->getSelectedFolder());
        proc->start(cmd, args);
        proc->waitForFinished(60000);
        this->ui->we_output->append(proc->readAll());
        this->ui->we_output->append(proc->readAllStandardError());
        int exitCode = proc->exitCode();
        delete proc;
        if (exitCode != 0) {
            this->ui->we_output->append("aborting, exit code " + QString::number(exitCode));
            break;
        }
    }

    this->ui->we_output->append("done");
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::setCommand(QString cmd, QString path)
{
    ui->fileBrowser->setSelectedFolder(path);
    this->ui->wer_cmd->setText(cmd);
}

// Execute command, read output and populate output window
void MainWindow::on_werb_exec_clicked()
{
    if (this->processRunnning) {
        QMessageBox::critical(this,
                              "Previous process still running",
                              "Refusing to execute new process before previous process exited");
        return;
    }

    // gather command and arguments
    QString raw = this->ui->wer_cmd->text();
    QString prog = raw.split(" ").at(0);
    QStringList args = raw.split(" "); args.removeFirst();

    // crear terminal output
    this->ui->we_output->clear();

    // run process
    QProcess *process = new QProcess(this);
    this->guiCmdProcess = process;
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(onMyStdoutReadyRead()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(onMyStderrReadReady()));
    connect(process, SIGNAL(finished(int)), this, SLOT(onMyProcessFinished(int)));
    this->processRunnning = true;
    this->ui->we_output->append(raw);
    process->setWorkingDirectory(ui->fileBrowser->getSelectedFolder());

    ds->setCommand(raw, ui->fileBrowser->getSelectedFolder());
    this->ui->wcmdinner->update();

    process->startDetached(prog, args);
    // rest is handled by callbacks
}

// Note: /bin/cat ../foo.html will render the html page.. might want to escape the output.
void MainWindow::onMyStdoutReadyRead()
{
    QByteArray out = this->guiCmdProcess->readAllStandardOutput();
    QString s(out);
    s.replace("&", "&amp;");
    s.replace("<", "&lt;");
    s.replace(">", "&gt;");
    s.replace("\n", "<br>");
    s.replace(" ", "&nbsp;");
    this->ui->we_output->append(s);
}

void MainWindow::onMyStderrReadReady()
{
    QByteArray err = this->guiCmdProcess->readAllStandardError();
    QString s(err);
    s.replace("&", "&amp;");
    s.replace("<", "&lt;");
    s.replace(">", "&gt;");
    s.replace("\n", "<br>");
    s.replace(" ", "&nbsp;");
    this->ui->we_output->append("<span style=\"color:red\">" + s + "</span>");
}

void MainWindow::onMyProcessFinished(int exitCode)
{
    this->processRunnning = false;
    this->ui->we_output->append("<span style=\"color:green\">done, exit code: "
                                + QString::number(exitCode) + "</span>");
    this->guiCmdProcess = NULL;
}

void MainWindow::on_wer_cmd_returnPressed()
{
    this->on_werb_exec_clicked();
}

void MainWindow::on_btnClearCommand_clicked()
{
    ui->wer_cmd->clear();
    ds->setGeneralValue("current_command", "");
    ui->notesWidget->commandChanged("");
}

void MainWindow::on_wer_cmd_textEdited(const QString &arg1)
{
    ds->setGeneralValue("current_command", arg1);
    ui->notesWidget->commandChanged(arg1);
}
