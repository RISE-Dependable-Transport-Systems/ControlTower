#include "flyform.h"
#include "ui_flyform.h"

FlyForm::FlyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlyForm)
{
    ui->setupUi(this);
}

FlyForm::~FlyForm()
{
    delete ui;
}

void FlyForm::setCurrentVehicleConnection(const QSharedPointer<MavsdkVehicleConnection> &currentVehicleConnection)
{
    mCurrentVehicleConnection = currentVehicleConnection;
}

void FlyForm::on_takeoffButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestTakeoff();
    }
}

void FlyForm::on_armButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestArm();
    }
}

void FlyForm::on_disarmButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestDisarm();
    }
}

void FlyForm::on_returnToHomeButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestReturnToHome();
    }
}

void FlyForm::on_landButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestLanding();
    }
}

void FlyForm::on_gotoButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestGotoENU({ui->gotoXSpinBox->value(), ui->gotoYSpinBox->value(),
                                                   mCurrentVehicleConnection->getVehicleState()->getPosition().getHeight()});
    }
}

void FlyForm::on_setAltitudeButton_clicked()
{
    if (mCurrentVehicleConnection) {
        mCurrentVehicleConnection->requestGotoENU({mCurrentVehicleConnection->getVehicleState()->getPosition().getX(),
                                                   mCurrentVehicleConnection->getVehicleState()->getPosition().getY(),
                                                   ui->gotoZSpinBox->value()});
    }
}
