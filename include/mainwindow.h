#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QShortcut>
#include <QFileSystemModel>
#include <QDebug>
#include <QLabel>

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
    ~MainWindow();

private slots:
    void setPresets();
    void setRootPath();
    void setPath(QString path);
    void on_config_presets_clicked();
    void charmClicked(QString path);
    void on_wpb_home_clicked();
    void on_wb_folders_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QString path;
    QList<QPushButton> charmButtons;
    QShortcut *shortEsc;
    QFileSystemModel *dirmodel, *filemodel;
    ConfigWidget *wconfig;
    DataStore *ds;

    QList<CharmButton*> cblist;
    QList<QLabel*> cllist;
};

#endif // MAINWINDOW_H
