#include "test.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(qApp->arguments().contains("--test")){
        Test *test = new Test();
        test->run();
        delete test;
    }


    MainWindow w;
    w.show();

    return a.exec();
}
