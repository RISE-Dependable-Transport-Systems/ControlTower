#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include <QThread>
#include <QStyleFactory>
#include "core/pospoint.h"
#include "vehicles/copterstate.h"
#include "WayWise/logger/logger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
//    setDarkStyle();

    Logger::initGroundStation();

    ui->setupUi(this);
    ui->logBrowser->hide();

    connect(&Logger::getInstance(), &Logger::logSent, this, &MainWindow::on_logSent);

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
            static llh_t lastGpsOriginLlh;
            if (lastGpsOriginLlh.latitude != gpsOriginLlh.latitude || lastGpsOriginLlh.longitude != gpsOriginLlh.longitude)
                if (!ui->ubloxBasestationUI->isBasestationRunning()) {
                    ui->mapWidget->setEnuRef(gpsOriginLlh);
                    qDebug() << "Updated ENU reference with received GpsOrigin:" << gpsOriginLlh.latitude << gpsOriginLlh.longitude;
                    lastGpsOriginLlh = gpsOriginLlh;
                }
        });

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
            ui->planUI->setCurrentVehicleConnection(vehicleConnection);
        } else {
            ui->tabWidget->removeTab(0); // remove driveUi
            ui->flyUI->setCurrentVehicleConnection(vehicleConnection); // Note: single connection assumed for now
        }
        ui->traceUI->setCurrentTraceVehicle(vehicleConnection->getVehicleState()); // Note: single connection assumed for now
    });

    connect(mMavsdkStation.get(), &MavsdkStation::disconnectOfVehicleConnection, ui->mapWidget, &MapWidget::removeObjectState);

    connect(ui->ubloxBasestationUI, &UbloxBasestationUI::currentPosition, ui->mapWidget, &MapWidget::setEnuRef);
    connect(ui->mapWidget, &MapWidget::enuRefChanged, mMavsdkStation.get(), &MavsdkStation::setEnuReference);
    connect(ui->ubloxBasestationUI, &UbloxBasestationUI::rtcmData, mMavsdkStation.get(), &MavsdkStation::forwardRtcmData);
    connect(ui->mapWidget, &MapWidget::enuRefChanged, ui->planUI->getRouteGeneratorUI().get(), &RouteGeneratorUI::setEnuRef);
    connect(ui->planUI, &PlanUI::routeDoneForUse, ui->flyUI, &FlyUI::gotRouteForAutopilot);
    connect(ui->planUI, &PlanUI::routeDoneForUse, ui->driveUI, &DriveUI::gotRouteForAutopilot);

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

void MainWindow::on_showLogsOutputAction_triggered()
{
    if(ui->logBrowser->isHidden()) {
        ui->logBrowser->show();
        ui->showLogsOutputAction->setText("Hide output");
    } else {
        ui->logBrowser->hide();
        ui->showLogsOutputAction->setText("Show output");
    }
}

void MainWindow::on_logSent(const QString& message)
{
    ui->logBrowser->append(message);
}
