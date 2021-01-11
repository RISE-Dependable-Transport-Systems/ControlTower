#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "sdvp_qtcommon/pospoint.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mapWidget->setDrawRouteText(false);
    ui->mapWidget->setScaleFactor(0.1);

    ui->mapWidget->setSelectedVehicle(0);

    QString connection_url = "udp://:14540"; // TODO: currently hardcoded for use with simulator
    mavsdk::ConnectionResult connection_result;

    mMavsdk.subscribe_on_new_system([this](){newMavsdkSystem();});

    connection_result = mMavsdk.add_any_connection(connection_url.toStdString());
    if (connection_result == mavsdk::ConnectionResult::Success)
        qDebug() << "Connected.";
    else {
        qDebug() << "Failed to connect.";
        exit(1);
    }

    qDebug() << "Waiting to discover system..." ;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newMavsdkSystem()
{
    // TODO assumes only one system exists
    mMavsdkSystemConnector.reset(new MavsdkSystemConnector(mMavsdk.systems().at(0), ui->mapWidget));
}
