#ifndef COMMANDWIDGET_H
#define COMMANDWIDGET_H

#include <QtWidgets>

#include "datastore.h"
#include "util.h"

namespace Ui {
    class CommandWidget;
}

class CommandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommandWidget(QWidget *parent = 0);
    ~CommandWidget();

    bool isExecuting();

signals:
    void sigProcStarted();        // used to disable all cmd related controls (scm)
    void sigProcStopped();        // used to enable all cmd related controls (scm)
    void sigCmdExecuted(QString, QString); // used to update cmd history
    void sigCmdChanged(QString);  // used to update notes

public slots:
    void slotUpdateFolder(QString selectedFolder);
    void slotUpdateCmd(QString cmdStr);

    void slotScmPull();
    void slotScmCommit(QString commitMsg);
    void slotScmPush();

private slots:
    void on_btnExecCommand_clicked();
    void on_btnClearCommand_clicked();
    void on_inputCommand_returnPressed();
    void on_inputCommand_textChanged(const QString &arg1);

    void _slotStdoutReadReady();
    void _slotStderrReadReady();
    void _slotProcessFinished(int exitCode);

private:
    void _slotExecCmd(QString cmdStr, bool multi = false);
    bool _slotExecMultiCmds(QStringList cmdList);

    QString workingDirectory;
    QProcess *process;

    Ui::CommandWidget *ui;
};

#endif // COMMANDWIDGET_H
