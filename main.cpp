#include "mainwindow.h"
#include <QApplication>
#include "WayWise/logger/logger.h"

int main(int argc, char *argv[])
{
    Logger::initGroundStation();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
