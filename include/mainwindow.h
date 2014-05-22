#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QShortcut>
#include <QFileSystemModel>
#include <QDebug>
#include <QLabel>
#include <QProcess>
#include <QMessageBox>
#include <QInputDialog>

#include "dstable.h"
#include "datastore.h"
#include "configwidget.h"
#include "charmbutton.h"
#include "presetbutton.h"
#include "noteswidget.h"
#include "filebrowser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void exec(QStringList cmd);
    void multiExec(QList<QStringList> commands);

    ~MainWindow();

private slots:
    void updatePresets();

    void setCommand(QString cmd, QString path);

    void on_werb_exec_clicked();
    void on_wer_cmd_returnPressed();
    void onMyStdoutReadyRead();
    void onMyStderrReadReady();
    void onMyProcessFinished(int exitCode);

    void on_btnClearCommand_clicked();
    void on_wer_cmd_textEdited(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QShortcut *shortEsc;
    DataStore *ds;

    QProcess *guiCmdProcess;
    bool processRunnning;
    bool isInit;
};

#endif // MAINWINDOW_H
