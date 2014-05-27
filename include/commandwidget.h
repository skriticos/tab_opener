#ifndef COMMANDWIDGET_H
#define COMMANDWIDGET_H

#include <QWidget>

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

    void initCommandWidget(DataStore *ds);
    bool isExecuting();

signals:
    void sigProcStarted();        // used to disable all cmd related controls (scm)
    void sigProcStopped();        // used to enable all cmd related controls (scm)
    void sigCmdExecuted(QString); // used to update cmd history
    void sigCmdChanged(QString);  // used to update notes

public slots:
    void slotUpdateFolder(QString selectedFolder);
    void slotUpdateCmd(QString cmdStr);
    void slotExecCmd(QString cmdStr, bool multi = false);
    bool slotExecMultiCmds(QStringList cmdList);

private slots:
    void on_btnExecCommand_clicked();
    void on_btnClearCommand_clicked();
    void on_inputCommand_returnPressed();
    void on_inputCommand_textChanged(const QString &arg1);

    void _slotStdoutReadReady();
    void _slotStderrReadReady();
    void _slotProcessFinished(int exitCode);

private:
    QString workingDirectory;
    QProcess *process;

    Ui::CommandWidget *ui;
    DataStore *ds;
};

#endif // COMMANDWIDGET_H
