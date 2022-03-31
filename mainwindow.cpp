#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include <QThread>
#include <QStyleFactory>
#include "sdvp_qtcommon/pospoint.h"

#include "routeplannermodule.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
//    setDarkStyle();

    ui->setupUi(this);
    ui->mapWidget->setScaleFactor(0.05);
    ui->mapWidget->setSelectedObjectState(0);
    ui->mapWidget->addMapModule(ui->planUI->getRoutePlanner());

    mMavsdkStation = QSharedPointer<MavsdkStation>::create();
    connect(mMavsdkStation.get(), &MavsdkStation::gotNewVehicleConnection, [&](QSharedPointer<MavsdkVehicleConnection> vehicleConnection){
        // LASH FIRE use case: we are a moving base and only communicate llh to/from drone
        vehicleConnection->setConvertLocalPositionsToGlobalBeforeSending(true);

        // If basestation is not running: Vehicle home = ENU reference
        // Note: single connection assumed for now
        connect(vehicleConnection.get(), &MavsdkVehicleConnection::gotVehicleHomeLlh, [this](const llh_t &homePositionLlh){
            if (!ui->ubloxBasestationUI->isBasestationRunning())
                ui->mapWidget->setEnuRef(homePositionLlh);
        });


        ui->mapWidget->addObjectState(vehicleConnection->getVehicleState());
        vehicleConnection->setEnuReference(ui->mapWidget->getEnuRef());
        ui->flyUI->setCurrentVehicleConnection(vehicleConnection); // Note: single connection assumed for now
//        ui->mapWidget->setFollowObjectState(vehicleConnection->getVehicleState()->getId());
    });

    connect(ui->ubloxBasestationUI, &UbloxBasestationUI::currentPosition, ui->mapWidget, &MapWidget::setEnuRef);
    connect(ui->ubloxBasestationUI, &UbloxBasestationUI::rtcmData, mMavsdkStation.get(), &MavsdkStation::forwardRtcmData);
    connect(ui->mapWidget, &MapWidget::enuRefChanged, mMavsdkStation.get(), &MavsdkStation::setEnuReference);
    connect(ui->planUI, &PlanUI::routeDoneForUse, ui->flyUI, &FlyUI::gotRouteForAutopilot);

    mMavsdkStation->startListeningUDP();
}

MainWindow::~MainWindow()
{
    // Allow MAVSDK to finish
    thread()->msleep(10);
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
