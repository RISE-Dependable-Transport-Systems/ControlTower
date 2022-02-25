#ifndef FLYUI_H
#define FLYUI_H

#include <QWidget>
#include <QSharedPointer>
#include "mavsdkvehicleconnection.h"
#include "waypointfollowerstation.h"

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

    void setCurrentWaypointFollower(const QSharedPointer<WaypointFollowerStation> &currentWaypointFollower);

private slots:
    void on_takeoffButton_clicked();

    void on_armButton_clicked();

    void on_disarmButton_clicked();

    void on_returnToHomeButton_clicked();

    void on_landButton_clicked();

    void on_gotoButton_clicked();

    void on_getPositionButton_clicked();

    void on_apRestartButton_clicked();

    void on_apStartButton_clicked();

    void on_apPauseButton_clicked();

    void on_apStopButton_clicked();

private:
    Ui::FlyUI *ui;
    QSharedPointer<MavsdkVehicleConnection> mCurrentVehicleConnection;
    // TODO: only suppoert for station-basesd autopilot for now.
    QSharedPointer<WaypointFollowerStation> mCurrentWaypointFollower;
};

#endif // FLYUI_H
