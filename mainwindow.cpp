#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include <QThread>
#include <QStyleFactory>
#include "core/pospoint.h"
#include "vehicles/copterstate.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
//    setDarkStyle();

    ui->setupUi(this);
    ui->mapWidget->setScaleFactor(0.05);
    ui->mapWidget->setSelectedObjectState(0);
    ui->mapWidget->addMapModule(ui->planUI->getRoutePlannerModule());
    ui->mapWidget->addMapModule(ui->traceUI->getTraceModule());
    ui->mapWidget->addMapModule(ui->flyUI->getGotoClickOnMapModule());
    ui->mapWidget->addMapModule(ui->cameraGimbalUI->getSetRoiByClickOnMapModule());

    mMavsdkStation = QSharedPointer<MavsdkStation>::create();
    connect(mMavsdkStation.get(), &MavsdkStation::gotNewVehicleConnection, [&](QSharedPointer<MavsdkVehicleConnection> vehicleConnection){
        // LASH FIRE use case: we are a moving base and only communicate llh to/from drone
        vehicleConnection->setConvertLocalPositionsToGlobalBeforeSending(true);

        // Note: single connection assumed for now
        // If basestation is not running: GpsOrigin -> ENU reference (Basestation position -> ENU reference, otherwise)
        connect(vehicleConnection.get(), &MavsdkVehicleConnection::gotVehicleGpsOriginLlh, [this](const llh_t &gpsOriginLlh){
            static bool enuRefUnset = true;
            if (!ui->ubloxBasestationUI->isBasestationRunning() && enuRefUnset) {
                ui->mapWidget->setEnuRef(gpsOriginLlh);
                enuRefUnset = false;
            }
            qDebug() << "Gps origin:" << gpsOriginLlh.latitude << gpsOriginLlh.longitude;
        });

        // If basestation is running: ENU reference -> vehicle home
        // TODO: this needs to be offset (we do not want to land on the GNSS antenna), e.g., by using home at take off and moving with ENU
//        connect(ui->ubloxBasestationUI, &UbloxBasestationUI::currentPosition, vehicleConnection.get(), &MavsdkVehicleConnection::setHomeLlh);

        ui->mapWidget->addObjectState(vehicleConnection->getVehicleState());
        vehicleConnection->setEnuReference(ui->mapWidget->getEnuRef());
//        ui->mapWidget->setFollowObjectState(vehicleConnection->getVehicleState()->getId());

        ui->cameraGimbalUI->setVehicleConnection(vehicleConnection); // Note: single connection assumed for now
        if (vehicleConnection->hasGimbal())
            ui->cameraGimbalUI->setGimbal(vehicleConnection->getGimbal());
        else // Gimbal might be available, but not detected yet
            connect(vehicleConnection.get(), &MavsdkVehicleConnection::detectedGimbal, [&](QSharedPointer<Gimbal> gimbal){
                ui->cameraGimbalUI->setGimbal(gimbal);
            });

        if (vehicleConnection->getVehicleType() == MAV_TYPE_GROUND_ROVER) {
            ui->tabWidget->removeTab(1); // remove flyUi
            ui->driveUI->setCurrentVehicleConnection(vehicleConnection); // Note: single connection assumed for now
        } else {
            ui->tabWidget->removeTab(0); // remove driveUi
            ui->flyUI->setCurrentVehicleConnection(vehicleConnection); // Note: single connection assumed for now
        }
        ui->traceUI->setCurrentTraceVehicle(vehicleConnection->getVehicleState()); // Note: single connection assumed for now
    });

    connect(ui->ubloxBasestationUI, &UbloxBasestationUI::currentPosition, ui->mapWidget, &MapWidget::setEnuRef);
    connect(ui->mapWidget, &MapWidget::enuRefChanged, mMavsdkStation.get(), &MavsdkStation::setEnuReference);
    connect(ui->ubloxBasestationUI, &UbloxBasestationUI::rtcmData, mMavsdkStation.get(), &MavsdkStation::forwardRtcmData);
    connect(ui->planUI, &PlanUI::routeDoneForUse, ui->flyUI, &FlyUI::gotRouteForAutopilot);
    connect(ui->planUI, &PlanUI::routeDoneForUse, ui->driveUI, &DriveUI::gotRouteForAutopilot);

    // TODO: for testing precland
    // should be ubloxrover (RELPOSNED) + offset -> landing target
//    connect(&mPreclandTestTimer, &QTimer::timeout, [&](){
//        static double x = 0.0;
//        if(ui->flyUI->getCurrentVehicleConnection() && ui->flyUI->getCurrentVehicleConnection()->getVehicleState().dynamicCast<CopterState>()->getLandedState() == CopterState::LandedState::InAir){
//            ui->flyUI->getCurrentVehicleConnection()->sendLandingTargetENU({x+=0.3, 1.0, 0.0});
//            if (x < 50.0)
//                ui->flyUI->getCurrentVehicleConnection()->requestGotoENU({x+0.5, 1.0, 30.0});
//            else if (x >= 50.0 && x <= 51.0) {
//                qDebug() << "Landing!";
//                ui->flyUI->getCurrentVehicleConnection()->requestPrecisionLanding();
//            }
//        } else if (ui->flyUI->getCurrentVehicleConnection() && ui->flyUI->getCurrentVehicleConnection()->getVehicleState().dynamicCast<CopterState>()->getLandedState() == CopterState::LandedState::Landing)
//            ui->flyUI->getCurrentVehicleConnection()->sendLandingTargetENU({x+=0.3, 1.0, 0.0});
//        else
//            x = 0.0;
//        qDebug() << x;
//    });
//    mPreclandTestTimer.start(200);

    mMavsdkStation->startListeningUDP();
//    mMavsdkStation->startListeningUDP(14550);

}

MainWindow::~MainWindow()
{
    // Allow MAVSDK to finish
    thread()->msleep(100);
    delete ui;
}

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

void MainWindow::on_AddSerialConnectionAction_triggered()
{
    if (mSerialPortDialog.isNull()) {
        mSerialPortDialog = QSharedPointer<SerialPortDialog>::create(this);
        connect(mSerialPortDialog.get(), &SerialPortDialog::selectedSerialPort, mMavsdkStation.get(), &MavsdkStation::startListeningSerial);
    }

    mSerialPortDialog->show();
}

void MainWindow::on_addUdpConnectionAction_triggered()
{
    bool ok;
    int i = QInputDialog::getInt(this, tr("Add UDP connection..."),
                                 tr("UDP port to listen on:"), 14550, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 1, &ok);
    if (ok)
        mMavsdkStation->startListeningUDP(i);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 0) // give keyboard to DriveUI for manual control when visible
        ui->driveUI->grabKeyboard();
    else
        ui->driveUI->releaseKeyboard();
}
