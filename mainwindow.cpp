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

    mCopterState.reset(new CopterState());
    mCopterState->setId(0);
    ui->mapWidget->addVehicle(mCopterState);
    ui->mapWidget->setSelectedVehicle(0);

    QString connection_url = "udp://:14540";
    mavsdk::ConnectionResult connection_result;

    mMavsdk.subscribe_on_new_system([this](){newMavsdkSystem();});

    connection_result = mMavsdk.add_any_connection(connection_url.toStdString());
    qDebug() << "Waiting to discover system..." ;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newMavsdkSystem()
{
    mSystem = mMavsdk.systems().at(0);
    mTelemetry.reset(new mavsdk::Telemetry(mSystem));

    mTelemetry->subscribe_position([this](mavsdk::Telemetry::Position position) {
        static bool first = true;
        if (first) {
            ui->mapWidget->setEnuRef(position.latitude_deg, position.longitude_deg, position.absolute_altitude_m);
            first = false;
        }

        double Llh[3] = {position.latitude_deg, position.longitude_deg, position.absolute_altitude_m};

        double iLlh[3];
        ui->mapWidget->getEnuRef(iLlh);

        double xyz[3];
        MapWidget::llhToEnu(iLlh, Llh, xyz);

        auto pos = mCopterState->getPosition();
        pos.setX(xyz[0]);
        pos.setY(xyz[1]);
        mCopterState->setPosition(pos);
    });
}
