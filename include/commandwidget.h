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
    bool processExecuting();

public slots:
    void selectedFolderChanged(QString selectedFolder);
    void historyCommandChanged(QString cmdStr);
    void execCmd(QString cmdStr, bool multi = false);
    bool execMultiCmds(QStringList cmdList);

private slots:
    void on_btnExecCommand_clicked();
    void on_btnClearCommand_clicked();
    void on_inputCommand_textChanged(const QString &arg1);

    void onStdoutReadReady();
    void onStderrReadReady();
    void onProcessFinished(int exitCode);

    void on_inputCommand_returnPressed();

signals:
    void processExecutionStarted();      // used to disable all cmd related controls (scm)
    void processExecutionStopped();      // used to enable all cmd related controls (scm)
    void manualCommandExecuted(QString); // used to update cmd history
    void commandChanged(QString);        // used to update notes

private:
    QString workingDirectory;
    QProcess *process;

    Ui::CommandWidget *ui;
    DataStore *ds;
};

#endif // COMMANDWIDGET_H
