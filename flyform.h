#ifndef FLYFORM_H
#define FLYFORM_H

#include <QWidget>
#include <QSharedPointer>
#include "mavsdkvehicleconnection.h"

namespace Ui {
class FlyForm;
}

class FlyForm : public QWidget
{
    Q_OBJECT

public:
    explicit FlyForm(QWidget *parent = nullptr);
    ~FlyForm();

    void setCurrentVehicleConnection(const QSharedPointer<MavsdkVehicleConnection> &currentVehicleConnection);

private slots:
    void on_takeoffButton_clicked();

    void on_armButton_clicked();

    void on_disarmButton_clicked();

    void on_returnToHomeButton_clicked();

    void on_landButton_clicked();

    void on_gotoButton_clicked();

    void on_setAltitudeButton_clicked();

private:
    Ui::FlyForm *ui;
    QSharedPointer<MavsdkVehicleConnection> mCurrentVehicleConnection;
};

#endif // FLYFORM_H
