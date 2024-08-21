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

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();

    connect(&Logger::getInstance(), &Logger::logSent, this, &MainWindow::on_logSent);

    ui->mapWidget->setScaleFactor(0.05);
    ui->mapWidget->setSelectedObjectState(0);
    ui->mapWidget->addMapModule(ui->planUI->getRoutePlannerModule());
    ui->mapWidget->addMapModule(ui->traceUI->getTraceModule());
    ui->mapWidget->addMapModule(ui->flyUI->getGotoClickOnMapModule());
    ui->mapWidget->addMapModule(ui->cameraGimbalUI->getSetRoiByClickOnMapModule());
    ui->driveTab->setDisabled(true);
    ui->flyTab->setDisabled(true);

    mMavsdkStation = QSharedPointer<MavsdkStation>::create();
    connect(mMavsdkStation.get(), &MavsdkStation::gotNewVehicleConnection, [&](QSharedPointer<MavsdkVehicleConnection> vehicleConnection){
        // LASH FIRE use case: we are a moving base and only communicate llh to/from drone
        vehicleConnection->setConvertLocalPositionsToGlobalBeforeSending(true);

        // If basestation is not running: Vehicle's ENU reference -> ControlTower ENU reference (Basestation position -> ENU reference, otherwise)
        connect(vehicleConnection.get(), &MavsdkVehicleConnection::gotVehicleENUreferenceLlh, [this](const llh_t &enuReferenceLlh){
            static llh_t lastENUreferenceLlh;
            if (lastENUreferenceLlh.latitude != enuReferenceLlh.latitude || lastENUreferenceLlh.longitude != enuReferenceLlh.longitude)
                if (!ui->ubloxBasestationUI->isBasestationRunning()) {
                    ui->mapWidget->setEnuRef(enuReferenceLlh);
                    qDebug() << "Updated ENU reference with received GpsOrigin:" << enuReferenceLlh.latitude << enuReferenceLlh.longitude;
                    lastENUreferenceLlh = enuReferenceLlh;
                }
        });

        ui->mapWidget->addObjectState(vehicleConnection->getVehicleState());
        vehicleConnection->setEnuReference(ui->mapWidget->getEnuRef());
//        ui->mapWidget->setFollowObjectState(vehicleConnection->getVehicleState()->getId());

        updateVehicleIdComboBox();
    });

    connect(mMavsdkStation.get(), &MavsdkStation::disconnectOfVehicleConnection, [this](int systemId){
        ui->mapWidget->removeObjectState(systemId);
        if (ui->vehicleIdCombo->currentText().toInt() == systemId) {
            ui->driveUI->setCurrentVehicleConnection(nullptr);
            ui->flyUI->setCurrentVehicleConnection(nullptr);
            ui->driveTab->setDisabled(true);
            ui->flyTab->setDisabled(true);
            ui->planUI->setCurrentVehicleConnection(nullptr);
            ui->cameraGimbalUI->setVehicleConnection(nullptr);
            ui->cameraGimbalUI->setGimbal(nullptr);
        }

        updateVehicleIdComboBox();

    });
    connect(ui->ubloxBasestationUI, &UbloxBasestationUI::currentPosition, ui->mapWidget, &MapWidget::setEnuRef);
    connect(ui->mapWidget, &MapWidget::enuRefChanged, mMavsdkStation.get(), &MavsdkStation::setEnuReference);
    connect(ui->ubloxBasestationUI, &UbloxBasestationUI::rtcmData, mMavsdkStation.get(), &MavsdkStation::forwardRtcmData);
    connect(ui->mapWidget, &MapWidget::enuRefChanged, ui->planUI->getRouteGeneratorUI().get(), &RouteGeneratorUI::setEnuRef);
    connect(ui->vehicleIdCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::updateUiForCurrentVehicleIdComboBoxIndex);

    mMavsdkStation->startListeningUDP();
//    mMavsdkStation->startListeningSerial(); // Sik radio
//    mMavsdkStation->startListeningUDP(14550); // HereLink
//    mMavsdkStation->startListeningUDP(14541); // Use a new port per vehicleConnection
//    mMavsdkStation->startListeningUDP(14555); // PX4 simulators
}

MainWindow::~MainWindow()
{
    // Allow MAVSDK to finish
    thread()->msleep(100);
    delete ui;
}

void MainWindow::updateVehicleIdComboBox()
{
    // Keep ui in sync with vehicleconnections maintained by MavsdkStation
    auto currentVehicleId = QString(ui->vehicleIdCombo->currentText()).isEmpty() ? 0 : ui->vehicleIdCombo->currentText();

    ui->vehicleIdCombo->clear();
    for (const auto& vehicleConnection : mMavsdkStation->getVehicleConnectionList())
        ui->vehicleIdCombo->addItem(QString::number(vehicleConnection->getVehicleState()->getId()),
                                    QVariant::fromValue(vehicleConnection));

    ui->vehicleIdCombo->setCurrentIndex(ui->vehicleIdCombo->findText(currentVehicleId) < 0 ? 0 : ui->vehicleIdCombo->findText(currentVehicleId));
}

void MainWindow::updateUiForCurrentVehicleIdComboBoxIndex(int index) {
    if (ui->vehicleIdCombo->count() == 0)
        return;

    QSharedPointer<MavsdkVehicleConnection> vehicleConnection = ui->vehicleIdCombo->itemData(index).value<QSharedPointer<MavsdkVehicleConnection>>();
    if (vehicleConnection->getVehicleType() == MAV_TYPE_GROUND_ROVER) {
        ui->driveUI->setCurrentVehicleConnection(vehicleConnection);
        ui->driveTab->setDisabled(false);
        ui->flyTab->setDisabled(true);
        ui->traceUI->setCurrentTraceVehicle(vehicleConnection->getVehicleState());
        disconnect(ui->planUI, &PlanUI::routeDoneForUse, ui->driveUI, &DriveUI::gotRouteForAutopilot);
        disconnect(ui->planUI, &PlanUI::routeDoneForUse, ui->flyUI, &FlyUI::gotRouteForAutopilot);
        connect(ui->planUI, &PlanUI::routeDoneForUse, ui->driveUI, &DriveUI::gotRouteForAutopilot);
//            ui->mapWidget->setFollowObjectState(systemId);
    } else { // not rover -> drone
        ui->flyUI->setCurrentVehicleConnection(vehicleConnection);
        ui->flyTab->setDisabled(false);
        ui->driveTab->setDisabled(true);
        ui->traceUI->setCurrentTraceVehicle(vehicleConnection->getVehicleState());
        disconnect(ui->planUI, &PlanUI::routeDoneForUse, ui->flyUI, &FlyUI::gotRouteForAutopilot);
        disconnect(ui->planUI, &PlanUI::routeDoneForUse, ui->driveUI, &DriveUI::gotRouteForAutopilot);
        connect(ui->planUI, &PlanUI::routeDoneForUse, ui->flyUI, &FlyUI::gotRouteForAutopilot);
//            ui->mapWidget->setFollowObjectState(systemId);
    }

    ui->planUI->setCurrentVehicleConnection(vehicleConnection);

    // TODO: revise late detection of gimbal (somewhat messy)
    ui->cameraGimbalUI->setVehicleConnection(vehicleConnection);
    disconnect(vehicleConnection.get(), &MavsdkVehicleConnection::detectedGimbal, nullptr, nullptr);
    if (vehicleConnection->hasGimbal())
        ui->cameraGimbalUI->setGimbal(vehicleConnection->getGimbal());
    else // Gimbal might be detected some time after the system
        connect(vehicleConnection.get(), &MavsdkVehicleConnection::detectedGimbal, [&](QSharedPointer<Gimbal> gimbal){
            ui->cameraGimbalUI->setGimbal(gimbal);
        });

    ui->mapWidget->setSelectedObjectState(vehicleConnection->getVehicleState()->getId());
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

void MainWindow::focusInEvent(QFocusEvent *event) {
    QMainWindow::focusInEvent(event);
    if (ui->tabWidget->currentIndex() == 0)
        ui->driveUI->grabKeyboard();
}

void MainWindow::focusOutEvent(QFocusEvent *event) {
    QMainWindow::focusOutEvent(event);
    ui->driveUI->releaseKeyboard();
}
