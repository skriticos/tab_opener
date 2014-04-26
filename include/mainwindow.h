#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QShortcut>
#include <QFileSystemModel>
#include <QDebug>
#include <QtSql>

#include "datastore.h"

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
    void on_wpb_edit_toggled(bool checked);

    void on_wb_folders_activated(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QShortcut *shortEsc;
    QFileSystemModel *dirmodel, *filemodel;
    QHash<QString, QStringList> myFileExtOpenEdit;
};

#endif // MAINWINDOW_H
