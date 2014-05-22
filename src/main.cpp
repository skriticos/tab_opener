#include "test.h"
#include "mainwindow.h"
#include <QApplication>

#include "commandwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(qApp->arguments().contains("--test")){
        Test *test = new Test();
        test->run();
        delete test;
    }


    MainWindow w;
    // w.show();
    CommandWidget w2;
    w2.show();

    return a.exec();
}
