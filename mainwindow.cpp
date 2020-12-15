#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <mavsdk/mavsdk.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mapWidget->setDrawRouteText(false);
    ui->mapWidget->setScaleFactor(0.1);

    mCopterState.reset(new CopterState());
    mCopterState->setId(0);
    ui->mapWidget->addVehicle(mCopterState);
    ui->mapWidget->setSelectedVehicle(0);

    mavsdk::Mavsdk mavsdk;
    std::string connection_url;


}

MainWindow::~MainWindow()
{
    delete ui;
}

