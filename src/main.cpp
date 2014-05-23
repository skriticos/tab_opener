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


    // MainWindow w;
    // w.show();
    CommandWidget w2;
    w2.initCommandWidget(dataStore);
    w2.show();

    return a.exec();
}
