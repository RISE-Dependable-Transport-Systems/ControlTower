#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include <QThread>
#include "sdvp_qtcommon/pospoint.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mapWidget->setScaleFactor(0.1);
    ui->mapWidget->setSelectedObjectState(0);

    mMavsdkStation = QSharedPointer<MavsdkStation>::create();
    mMavsdkStation->startListeningUDP();

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &portInfo, ports) {
        if (portInfo.manufacturer().toLower().replace("-", "").contains("ublox")) {
            mUbloxBasestation.connectSerial(portInfo);
            qDebug() << "Connected to:" << portInfo.systemLocation();
        }
    }
    if (mUbloxBasestation.isSerialConnected()) {
        // Base position = ENU reference
        connect(&mUbloxBasestation, &UbloxBasestation::currentPosition, ui->mapWidget, &MapWidget::setEnuRef);
        connect(&mUbloxBasestation, &UbloxBasestation::rtcmData, mMavsdkStation.get(), &MavsdkStation::forwardRtcmData); // TODO: not fully implemented
    }
    connect(ui->mapWidget, &MapWidget::enuRefChanged, mMavsdkStation.get(), &MavsdkStation::setEnuReference);

    connect(mMavsdkStation.get(), &MavsdkStation::gotNewVehicleConnection, [&](QSharedPointer<MavsdkVehicleConnection> vehicleConnection){
        if (!mUbloxBasestation.isSerialConnected())
            // Vehicle home = ENU reference
            connect(vehicleConnection.get(), &MavsdkVehicleConnection::gotVehicleHomeLlh, ui->mapWidget, &MapWidget::setEnuRef);

        ui->mapWidget->addObjectState(vehicleConnection->getVehicleState());
    });
}

MainWindow::~MainWindow()
{
    // Allow MAVSDK to finish
    thread()->msleep(10);
    delete ui;
}
