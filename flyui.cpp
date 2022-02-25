#include "flyui.h"
#include "ui_flyui.h"

FlyUI::FlyUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlyUI)
{
    ui->setupUi(this);
}

FlyUI::~FlyUI()
{
    delete ui;
}

void FlyUI::setCurrentVehicleConnection(const QSharedPointer<MavsdkVehicleConnection> &currentVehicleConnection)
{
    mCurrentVehicleConnection = currentVehicleConnection;
}

void FlyUI::on_takeoffButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestTakeoff();
    }
}

void FlyUI::on_armButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestArm();
    }
}

void FlyUI::on_disarmButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestDisarm();
    }
}

void FlyUI::on_returnToHomeButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestReturnToHome();
    }
}

void FlyUI::on_landButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestLanding();
    }
}

void FlyUI::on_gotoButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestGotoENU({ui->gotoXSpinBox->value(), ui->gotoYSpinBox->value(), ui->gotoZSpinBox->value()});
    }
}

void FlyUI::on_getPositionButton_clicked()
{
    if (mCurrentVehicleConnection) {
        ui->gotoXSpinBox->setValue(mCurrentVehicleConnection->getVehicleState()->getPosition().getX());
        ui->gotoYSpinBox->setValue(mCurrentVehicleConnection->getVehicleState()->getPosition().getY());
        ui->gotoZSpinBox->setValue(mCurrentVehicleConnection->getVehicleState()->getPosition().getHeight());
    }
}

void FlyUI::on_apRestartButton_clicked()
{
    if (mCurrentVehicleConnection->hasWaypointFollower())
        mCurrentVehicleConnection->getWaypointFollower()->startFollowingRoute(true);
}

void FlyUI::on_apStartButton_clicked()
{
    if (mCurrentVehicleConnection->hasWaypointFollower())
        mCurrentVehicleConnection->getWaypointFollower()->startFollowingRoute(false);
}

void FlyUI::on_apPauseButton_clicked()
{
    if (mCurrentVehicleConnection->hasWaypointFollower())
        mCurrentVehicleConnection->getWaypointFollower()->stop();
}

void FlyUI::on_apStopButton_clicked()
{
    if (mCurrentVehicleConnection->hasWaypointFollower()) {
        mCurrentVehicleConnection->getWaypointFollower()->stop();
        mCurrentVehicleConnection->getWaypointFollower()->resetState();
    }
}

void FlyUI::gotRouteForAutopilot(const QList<PosPoint> &route)
{
    if (!mCurrentVehicleConnection->hasWaypointFollower())
        mCurrentVehicleConnection->setWaypointFollower(QSharedPointer<WaypointFollower>::create(mCurrentVehicleConnection, PosType::defaultPosType));

    mCurrentVehicleConnection->getWaypointFollower()->clearRoute();
    mCurrentVehicleConnection->getWaypointFollower()->addRoute(route);
}

