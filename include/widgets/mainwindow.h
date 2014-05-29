#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtWidgets>

#include "datastore.h"
#include "configwidget.h"
#include "presetbutton.h"
#include "noteswidget.h"
#include "filebrowserwidget.h"
#include "commandwidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(DataStore *ds, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotEscPressed();
    void slotRequestClose(); // file opened with primary or secondary action, or terminal or file browser run
    void slotInvalidExtension();

private:
    Ui::MainWindow *ui;

    ConfigWidget *configWidget;
};

#endif // MAINWINDOW_H
