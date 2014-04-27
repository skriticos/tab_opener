#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QShortcut>
#include <QFileSystemModel>
#include <QDebug>

#include "datastore.h"
#include "configwidget.h"
#include "charmbutton.h"

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
    void on_wpb_edit_toggled(bool checked);
    void on_wb_folders_activated(const QModelIndex &index);
    void on_config_presets_clicked();
    void charmClicked(QString path);

private:
    Ui::MainWindow *ui;
    QString path;
    QList<QPushButton> charmButtons;
    QShortcut *shortEsc;
    QFileSystemModel *dirmodel, *filemodel;
    ConfigWidget *wconfig;
    DataStore *ds;

    CharmButton *cbutton;
};

#endif // MAINWINDOW_H
