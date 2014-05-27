#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QWidget>

#include "datastore.h"
#include "configwidget.h"
#include "presetbutton.h"
#include "noteswidget.h"
#include "filebrowser.h"
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
    void initWidget(DataStore *ds);

private slots:
    void slotUpdatePresets();
    void slotEscPressed();
    void slotRequestClose(); // file opened with primary or secondary action, or terminal or file browser run

private:
    Ui::MainWindow *ui;
    QShortcut *shortEsc;
    DataStore *ds;
};

#endif // MAINWINDOW_H
