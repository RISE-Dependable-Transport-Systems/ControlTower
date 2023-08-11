#include "mainwindow.h"
#include <QApplication>
#include "WayWise/logger/groundstationlogger.h"

int main(int argc, char *argv[])
{
    GroundStationLogger::init();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    bool state = a.exec();

    GroundStationLogger::shutDown();
    return state;
}
