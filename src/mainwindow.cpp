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
    this->filemodel = NULL;

    // keyboard shortcuts
    this->shortEsc = new QShortcut(QKeySequence(tr("Esc")), this);
    connect(shortEsc, SIGNAL(activated()), this, SLOT(close()));

    this->wfInitBrowser();

    // file and command quicklists
    ui->wfileinner->init(ds);
    connect(ui->wfileinner, SIGNAL(forwardPath(QString)), this, SLOT(setPath(QString)));
    connect(this->ui->wfileinner, SIGNAL(openOrEditClicked()), this, SLOT(close()));

    ui->wcmdinner->init(ds);
    connect(ui->wcmdinner, SIGNAL(commandSelected(QString,QString)),
            this, SLOT(setCommand(QString,QString)));

    // configuration widget (preferences)
    this->wconfig = new ConfigWidget(ds, this);

    // presets
    this->connect(wconfig, SIGNAL(accepted()), this, SLOT(updatePresets()));
    this->updatePresets();

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

    this->ui->notesWidget->initWidget(this->ds);

    if(ds->tblGeneral->contains("selected_file")){
        QString filePath = ds->getGeneralValue("selected_file");
        if(!filePath.isEmpty()){
            ui->wb_files->setCurrentIndex(this->filemodel->index(filePath));
        }
    }
    if(ds->tblGeneral->contains("current_command")){
        ui->wer_cmd->setText(ds->getGeneralValue("current_command"));
    }

    this->isInit = false;

    this->fb = new FileBrowser();
    this->fb->initFileBrowser(this->ds);
    this->fb->show();
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
    process->setWorkingDirectory(this->path);

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
        proc->setWorkingDirectory(this->path);
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

void MainWindow::setRootPath()
{
    this->setPath(QDir::rootPath());
}

void MainWindow::setPath(QString path)
{
    this->path = path;
    ds->setGeneralValue("navigator_path", path);
    if(!this->isInit){
        ds->setGeneralValue("selected_file", "");
        ui->notesWidget->selectedFileChanged("");
    }

    // split path into directories
    QStringList charmParts = path.split(QDir::separator());

    ui->wb_folders->setCurrentIndex(dirmodel->index(path));
    ui->wb_folders->setExpanded(dirmodel->index(path), true);
    ui->wb_files->setRootIndex(filemodel->setRootPath(path));

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

void MainWindow::setCommand(QString cmd, QString path)
{
    this->setPath(path);
    this->ui->wer_cmd->setText(cmd);
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
   this->setPath(path);
}

void MainWindow::on_wpb_terminal_clicked()
{
    QProcess *p = new QProcess(this);
    QStringList sl;
    sl = ds->getGeneralValue("terminal_emulator").split(" ");
    QString cmd = sl.at(0);
    sl.removeAt(0);
    sl << this->path;
    p->start(cmd, QStringList() << sl);

    this->close();
}

void MainWindow::on_wpb_folder_clicked()
{
    QProcess *p = new QProcess(this);
    QStringList sl;
    sl = ds->getGeneralValue("file_browser").split(" ");
    QString cmd = sl.at(0);
    sl.removeAt(0);
    sl << this->path;
    p->start(cmd, QStringList() << sl);

    this->close();
}

void MainWindow::on_view_file_clicked()
{
    // get currently selected file
    QModelIndex index = ui->wb_files->selectionModel()->selectedIndexes().at(0);
    QString selectedFilePath = filemodel->fileInfo(index).absoluteFilePath();

    // extract filetype
    QString extStr = selectedFilePath.split(".").last();

    DsTable::Record record = ds->tblExtensions->getRecord(extStr);
    QString extActPri = record.value("ext_act_pri").toString();

    // prepare command string and arguments
    QStringList rawCmd = extActPri.split(" ");
    QString prog = rawCmd.first();
    QStringList args = rawCmd.mid(1) << selectedFilePath;

    // execute open command
    QProcess *p = new QProcess(this);
    p->startDetached(prog, args);

    // push file to stack
    ds->setFile(selectedFilePath);
    ui->wfileinner->update();

    this->close();
}

void MainWindow::on_edit_file_clicked()
{
    // get currently selected file
    QModelIndex index = ui->wb_files->selectionModel()->selectedIndexes().at(0);
    QString selectedFilePath = filemodel->fileInfo(index).absoluteFilePath();

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
    ui->wfileinner->update();

    this->close();
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
    process->setWorkingDirectory(this->path);

    ds->setCommand(raw, this->path);
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

/**
 * @brief Perform SCM pull action at current path
 */
void MainWindow::on_actionScmPull_triggered()
{
    this->exec(QStringList() << "/usr/bin/git" << "pull");
}

/**
 * @brief Perform SCM add all + commit action
 */
void MainWindow::on_actionScmCommit_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Commit message"),
                                         tr("Commit message:"), QLineEdit::Normal, "", &ok);
    if (ok){
        if (!text.isEmpty()){
            QList <QStringList> commands;
            commands.append(QStringList() << "/usr/bin/git" << "add" << "--all");
            commands.append(QStringList() << "/usr/bin/git" << "commit" << "-am" << text);
            this->multiExec(commands);
        } else {
            QMessageBox::warning(this, "Abortig commit", "No commit message, aborting..", QMessageBox::Ok);
        }
    }
}

/**
 * @brief Perform SCM push
 */
void MainWindow::on_actionScmPush_triggered()
{
    this->exec(QStringList() << "/usr/bin/git" << "push" << "all" << "--all");
}

void MainWindow::onFileSelected(QItemSelection selected)
{
    QModelIndex index = selected.indexes().first();
    QString selectedFilePath = dirmodel->fileInfo(index).absoluteFilePath();
    ds->setGeneralValue("selected_file", selectedFilePath);

    ui->notesWidget->selectedFileChanged(selectedFilePath);
}

void MainWindow::wfInitBrowser()
{
    this->dirmodel = new QFileSystemModel(this);
    this->filemodel = new QFileSystemModel(this);

    this->dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    this->filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);

    ui->wb_folders->setModel(dirmodel);
    ui->wb_files->setModel(filemodel);

    ui->wb_folders->setRootIndex(dirmodel->setRootPath("/"));
    ui->wb_folders->hideColumn(3);
    ui->wb_folders->hideColumn(2);
    ui->wb_folders->hideColumn(1);

    if(ds->tblGeneral->contains("navigator_path"))
        this->setPath(ds->getGeneralValue("navigator_path"));
    else
        this->setPath(QDir::homePath());

    QItemSelectionModel *fileSelectionModel = ui->wb_files->selectionModel();
    connect(ui->wpc_root, SIGNAL(clicked()), this, SLOT(setRootPath()));
    connect(fileSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onFileSelected(QItemSelection)));
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
