#include "commandwidget.h"
#include "ui_commandwidget.h"

CommandWidget::CommandWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CommandWidget)
{
    ui->setupUi(this);

    this->process = new QProcess();
    connect(this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(_slotStdoutReadReady()));
    connect(this->process, SIGNAL(readyReadStandardError()), this, SLOT(_slotStderrReadReady()));
    connect(this->process, SIGNAL(finished(int)), this, SLOT(_slotProcessFinished(int)));
    connect(this->process, SIGNAL(finished(int)), ui->btnExecCommand, SLOT(slotProcExecStopped()));
    connect(this->process, SIGNAL(started()), ui->btnExecCommand, SLOT(slotProcExecStarted()));
}

CommandWidget::~CommandWidget()
{
    delete ui;
}

bool CommandWidget::isExecuting()
{
    if(this->process->state() == QProcess::NotRunning)
        return false;
    else
        return true;
}

void CommandWidget::slotUpdateFolder(QString selectedFolder)
{
    this->workingDirectory = selectedFolder;
}

void CommandWidget::slotUpdateCmd(QString cmdStr)
{
    ui->inputCommand->setText(cmdStr);
    emit this->sigCmdChanged(cmdStr);
}

void CommandWidget::_slotExecCmd(QString cmdStr, bool multi)
{
    Util::ParsedCommand parsedCmd;
    QString wd;

    if(this->workingDirectory.isEmpty())
        wd = QDir::homePath();
    else
        wd = this->workingDirectory;

    parsedCmd = Util::parseCmdStr(cmdStr);

    if(!multi)
        ui->viewCommandOutput->clear();
    ui->viewCommandOutput->append("<span style='color:green'>executing [" + cmdStr +
                                  "] in " + wd + "</span>");

    this->process->setWorkingDirectory(wd);
    process->start(parsedCmd.program, parsedCmd.args);

    emit this->sigProcStarted();
}

bool CommandWidget::_slotExecMultiCmds(QStringList cmdList)
{
    for(int i=0; i<cmdList.size(); i++){
        this->_slotExecCmd(cmdList.at(i), true);
        this->process->waitForFinished();
        if(this->process->exitCode() != 0)
            return false;
    }
    return true;
}

void CommandWidget::slotScmPull()
{
    _slotExecCmd("git pull --all");
}

void CommandWidget::slotScmCommit(QString commitMsg)
{
    QStringList cmdList;
    cmdList << "git add --all";
    cmdList << "git commit -am \"" + commitMsg + "\"";
    _slotExecMultiCmds(cmdList);
}

void CommandWidget::slotScmPush()
{
    _slotExecCmd("git push --all");
}

void CommandWidget::on_btnExecCommand_clicked()
{
    if(this->process->state() == QProcess::NotRunning){
        if(!ui->inputCommand->text().isEmpty()){
            emit this->sigCmdExecuted(ui->inputCommand->text(), this->workingDirectory);
            this->_slotExecCmd(ui->inputCommand->text());
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
    emit this->sigCmdChanged(arg1);
}

void CommandWidget::_slotStdoutReadReady()
{
    QByteArray output = this->process->readAllStandardOutput();
    QString outputStr = QString(output).toHtmlEscaped();
    outputStr.replace("\n", "<br>");
    outputStr.replace(" ", "&nbsp;");
    ui->viewCommandOutput->append(outputStr);
}

void CommandWidget::_slotStderrReadReady()
{
    QByteArray output = this->process->readAllStandardError();
    QString outputStr = QString(output).toHtmlEscaped();
    outputStr.replace("\n", "<br>");
    outputStr.replace(" ", "&nbsp;");
    outputStr = "<span style='color:red'>" + outputStr + "</span>";
    ui->viewCommandOutput->append(outputStr);
}

void CommandWidget::_slotProcessFinished(int exitCode)
{
    ui->viewCommandOutput->append("<span style='color:gray'>[process finished with exit code: "
                                  + QString::number(exitCode) + "]</span>");
    emit this->sigProcStopped();
}

void CommandWidget::on_inputCommand_returnPressed()
{
    if(this->process->state() == QProcess::NotRunning)
        this->on_btnExecCommand_clicked();
}
