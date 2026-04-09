#include "mainwindow.h"
#include "trekview.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TrekView w;
    w.show();
    // TrekBox2DPhysics phy;
    return a.exec();
}
