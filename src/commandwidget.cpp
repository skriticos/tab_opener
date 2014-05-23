#include "commandwidget.h"
#include "ui_commandwidget.h"

CommandWidget::CommandWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CommandWidget)
{
    ui->setupUi(this);

    this->process = new QProcess();
    connect(this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(onStdoutReadReady()));
    connect(this->process, SIGNAL(readyReadStandardError()), this, SLOT(onStderrReadReady()));
    connect(this->process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    connect(this->process, SIGNAL(finished(int)), ui->btnExecCommand, SLOT(processExecutionStopped()));
    connect(this->process, SIGNAL(started()), ui->btnExecCommand, SLOT(processExecutionStarted()));
}

CommandWidget::~CommandWidget()
{
    delete ui;
}

void CommandWidget::initCommandWidget(DataStore *ds)
{
    this->ds = ds;
    if(ds->tblGeneral->contains("current_command")
            && !ds->getGeneralValue("current_command").isEmpty()){
        ui->inputCommand->setText(ds->getGeneralValue("current_command"));
    }
}

bool CommandWidget::processExecuting()
{
    if(this->process->state() == QProcess::NotRunning)
        return false;
    else
        return true;
}

void CommandWidget::selectedFolderChanged(QString selectedFolder)
{
    this->workingDirectory = selectedFolder;
}

void CommandWidget::historyCommandChanged(QString cmdStr)
{
    ui->inputCommand->setText(cmdStr);
}

void CommandWidget::execCmd(QString cmdStr)
{
    Util::ParsedCommand parsedCmd;
    QString wd;

    if(this->workingDirectory.isEmpty())
        wd = QDir::homePath();
    else
        wd = this->workingDirectory;

    parsedCmd = Util::parseCmdStr(cmdStr);

    ui->viewCommandOutput->clear();
    ui->viewCommandOutput->append("<span style='color:green'>executing [" + cmdStr +
                                  "] in " + wd + "</span>");

    this->process->setWorkingDirectory(wd);
    process->start(parsedCmd.program, parsedCmd.args);

    emit this->processExecutionStarted();
}

bool CommandWidget::execMultiCmds(QStringList cmdList)
{
    for(int i=0; i<cmdList.size(); i++){
        this->execCmd(cmdList.at(i));
        this->process->waitForFinished();
        if(this->process->exitCode() != 0)
            return false;
    }
    return true;
}

void CommandWidget::on_btnExecCommand_clicked()
{
    if(this->process->state() == QProcess::NotRunning){
        if(!ui->inputCommand->text().isEmpty()){
            emit this->manualCommandExecuted(ui->inputCommand->text());
            this->execCmd(ui->inputCommand->text());
        }
    } else { // process already running, aborting
        this->process->kill();
        this->process->waitForFinished();
        ui->viewCommandOutput->append("<span style='color:red'>[process killed on user request]</span>");
    }
}

void CommandWidget::on_btnClearCommand_clicked()
{
    ui->inputCommand->clear();
}

void CommandWidget::on_inputCommand_textChanged(const QString &arg1)
{
    emit this->commandChanged(arg1);
}

void CommandWidget::onStdoutReadReady()
{
    QByteArray output = this->process->readAllStandardOutput();
    QString outputStr = QString(output).toHtmlEscaped();
    outputStr.replace("\n", "<br>");
    outputStr.replace(" ", "&nbsp;");
    ui->viewCommandOutput->append(outputStr);
}

void CommandWidget::onStderrReadReady()
{
    QByteArray output = this->process->readAllStandardError();
    QString outputStr = QString(output).toHtmlEscaped();
    outputStr.replace("\n", "<br>");
    outputStr.replace(" ", "&nbsp;");
    outputStr = "<span style='color:red'>" + outputStr + "</span>";
    ui->viewCommandOutput->append(outputStr);
}

void CommandWidget::onProcessFinished(int exitCode)
{
    ui->viewCommandOutput->append("<span style='color:gray'>[process finished with exit code: "
                                  + QString::number(exitCode) + "]</span>");
    emit this->processExecutionStopped();
}

void CommandWidget::on_inputCommand_returnPressed()
{
    if(this->process->state() == QProcess::NotRunning)
        this->on_btnExecCommand_clicked();
}
