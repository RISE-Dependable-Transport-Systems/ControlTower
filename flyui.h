#ifndef FLYUI_H
#define FLYUI_H

#include <QWidget>
#include <QSharedPointer>
#include "mavsdkvehicleconnection.h"

namespace Ui {
class FlyUI;
}

class FlyUI : public QWidget
{
    Q_OBJECT

public:
    explicit FlyUI(QWidget *parent = nullptr);
    ~FlyUI();

    void setCurrentVehicleConnection(const QSharedPointer<MavsdkVehicleConnection> &currentVehicleConnection);

private slots:
    void on_takeoffButton_clicked();

    void on_armButton_clicked();

    void on_disarmButton_clicked();

    void on_returnToHomeButton_clicked();

    void on_landButton_clicked();

    void on_gotoButton_clicked();

    void on_getPositionButton_clicked();

private:
    Ui::FlyUI *ui;
    QSharedPointer<MavsdkVehicleConnection> mCurrentVehicleConnection;
};

#endif // FLYUI_H
