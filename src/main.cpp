#include "test.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(qApp->arguments().contains("--test"))
        Test().run();

    MainWindow w;
    w.show();

    return a.exec();
}
