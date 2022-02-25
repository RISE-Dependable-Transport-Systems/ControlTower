#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include <QThread>
#include <QStyleFactory>
#include "sdvp_qtcommon/pospoint.h"

#include "routeplannermodule.h"

void MainWindow::setDarkStyle()
{
    // based on https://stackoverflow.com/a/45634644
    // set style
    qApp->setStyle(QStyleFactory::create("Fusion"));

    // modify palette to dark
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window,QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
    darkPalette.setColor(QPalette::Base,QColor(42,42,42));
    darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
    darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
    darkPalette.setColor(QPalette::ToolTipText,Qt::white);
    darkPalette.setColor(QPalette::Text,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
    darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
    darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
    darkPalette.setColor(QPalette::Button,QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
    darkPalette.setColor(QPalette::BrightText,Qt::red);
    darkPalette.setColor(QPalette::Link,QColor(42,130,218));
    darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
    darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
    darkPalette.setColor(QPalette::HighlightedText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

    qApp->setPalette(darkPalette);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
//    setDarkStyle();

    ui->setupUi(this);
    ui->mapWidget->setScaleFactor(0.1);
    ui->mapWidget->setSelectedObjectState(0);
    ui->mapWidget->addMapModule(ui->planUI->getRoutePlanner());

    mMavsdkStation = QSharedPointer<MavsdkStation>::create();
    mMavsdkStation->startListeningUDP();
    connect(mMavsdkStation.get(), &MavsdkStation::gotNewVehicleConnection, [&](QSharedPointer<MavsdkVehicleConnection> vehicleConnection){
        // LASH FIRE use case: we are a moving base and only communicate llh to/from drone
        vehicleConnection->setConvertLocalPositionsToGlobalBeforeSending(true);

        if (!mUbloxBasestation.isSerialConnected())
            // Vehicle home = ENU reference
            connect(vehicleConnection.get(), &MavsdkVehicleConnection::gotVehicleHomeLlh, ui->mapWidget, &MapWidget::setEnuRef);

        ui->mapWidget->addObjectState(vehicleConnection->getVehicleState());
        ui->flyUI->setCurrentVehicleConnection(vehicleConnection);
    });
    // TODO: refactor, where should this be done?
    connect(ui->planUI, &PlanUI::routeDoneForUse, ui->flyUI, &FlyUI::gotRouteForAutopilot);

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

}

MainWindow::~MainWindow()
{
    // Allow MAVSDK to finish
    thread()->msleep(10);
    delete ui;
}
