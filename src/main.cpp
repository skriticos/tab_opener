#include <QApplication>

#include "test.h"
#include "mainwindow.h"
#include "commandwidget.h"
#include "datastore.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(qApp->arguments().contains("--test")){
        Test *test = new Test();
        test->run();
        delete test;
    }

    DataStore *dataStore = new DataStore(&a);

    MainWindow mainWindow(dataStore);
    mainWindow.initWidget(dataStore);
    mainWindow.show();

    return a.exec();
}
