#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QShortcut>
#include <QFileSystemModel>
#include <QDebug>
#include <QLabel>
#include <QProcess>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

#include "datastore.h"
#include "configwidget.h"
#include "charmbutton.h"
#include "presetbutton.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void showSystray();
    ~MainWindow();

private slots:
    void setPresets();
    void setRootPath();
    void setPath(QString path);
    void setCommand(QString cmd, QString path);
    void on_config_presets_clicked();
    void charmClicked(QString path);
    void on_wpb_home_clicked();
    void on_wb_folders_clicked(const QModelIndex &index);
    void notesChanged();
    void on_wpb_terminal_clicked();
    void on_wpb_folder_clicked();
    void on_view_file_clicked();
    void on_edit_file_clicked();

    void on_werb_exec_clicked();
    void on_wer_cmd_returnPressed();
    void onMyStdoutReadyRead();
    void onMyStderrReadReady();
    void onMyProcessFinished(int exitCode);

    void systrayActivate(QSystemTrayIcon::ActivationReason a);

private:
    Ui::MainWindow *ui;
    QString path;
    QList<QPushButton> charmButtons;
    QShortcut *shortEsc;
    QFileSystemModel *dirmodel, *filemodel;
    ConfigWidget *wconfig;
    DataStore *ds;
    QSystemTrayIcon *sysTray;

    QList<CharmButton*> cblist;
    QList<QLabel*> cllist;
    QProcess *guiCmdProcess;
    bool processRunnning;
};

#endif // MAINWINDOW_H
