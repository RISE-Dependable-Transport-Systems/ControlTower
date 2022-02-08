#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include "sdvp_qtcommon/pospoint.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mapWidget->setScaleFactor(0.1);

    ui->mapWidget->setSelectedObjectState(0);

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

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &portInfo, ports) {
        if (portInfo.manufacturer().toLower().replace("-", "").contains("ublox")) {
            mUbloxBasestation.connectSerial(portInfo);
            qDebug() << "Connected to:" << portInfo.systemLocation();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newMavsdkSystem()
{
    qDebug() << "Got system.";

    // Note: assumes only one system exists
    mMavsdkSystemConnector.reset(new MavsdkSystemConnector(mMavsdk.systems().at(0), ui->mapWidget->getEnuRef()));

    // Make sure direct connection is used, i.e., slot is called directly like a function. Problems with threading otherwise.
    connect(&mUbloxBasestation, &UbloxBasestation::rtcmData, mMavsdkSystemConnector.get(), &MavsdkSystemConnector::forwardRtcmDataToSystem, Qt::DirectConnection);

    if (mUbloxBasestation.isSerialConnected())
        // Base position = ENU reference
        connect(&mUbloxBasestation, &UbloxBasestation::currentPosition, ui->mapWidget, &MapWidget::setEnuRef, Qt::DirectConnection);
    else
        // System home = ENU reference
        connect(mMavsdkSystemConnector.get(), &MavsdkSystemConnector::systemHomeLlh, ui->mapWidget, &MapWidget::setEnuRef, Qt::DirectConnection);
    connect(ui->mapWidget, &MapWidget::enuRefChanged, mMavsdkSystemConnector.get(), &MavsdkSystemConnector::setEnuReference, Qt::DirectConnection);

    // Register system as receiver of input events from map and make copter visible on map
    ui->mapWidget->addMapModule(mMavsdkSystemConnector);
    ui->mapWidget->addObjectState(mMavsdkSystemConnector->getCopterState());
}
