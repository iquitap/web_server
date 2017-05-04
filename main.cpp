#include "mainwindow.h"
#include <QApplication>

int startsig = 0;
int stopsig = 0;
int nomove = 1;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

