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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initWidget(DataStore *ds);

private slots:
    void updatePresets();
    void onEscPressed();

private:
    Ui::MainWindow *ui;
    QShortcut *shortEsc;
    DataStore *ds;
};

#endif // MAINWINDOW_H
