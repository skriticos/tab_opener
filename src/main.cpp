#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    bool hidden = false;
    for (int i=0; i<argc; i++)
        if (QString(argv[i]) == "--hidden")
            hidden = true;
    if (!hidden)
        w.show();
    w.showSystray();

    return a.exec();
}
