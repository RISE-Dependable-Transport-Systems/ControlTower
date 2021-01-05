#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "sdvp_qtcommon/pospoint.h"

// TODO: this is a first test, all the mavsdk communication should be contained in a separate class eventually

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
    ui->mapWidget->setTraceVehicle(0);

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
    mSystem = mMavsdk.systems().at(0);
    mCopterState->setName("Copter " + QString::number(mSystem->get_system_id()));
    mTelemetry.reset(new mavsdk::Telemetry(mSystem));

    mTelemetry->subscribe_position([this](mavsdk::Telemetry::Position position) {
        static bool first = true;
        if (first) { // set reference for ENU position to first position received
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
        pos.setHeight(position.relative_altitude_m);
        mCopterState->setPosition(pos);
    });

    mTelemetry->subscribe_attitude_quaternion([this](mavsdk::Telemetry::Quaternion q) {
        auto pos = mCopterState->getPosition();
        pos.setYaw(atan2f(q.w * q.z + q.x * q.y, 0.5 - (q.y * q.y + q.z * q.z))); // extract yaw from quaternion
        mCopterState->setPosition(pos);
    });
}
