#ifndef MAVSDKVEHICLECONNECTION_H
#define MAVSDKVEHICLECONNECTION_H

#include <QObject>
#include <QSharedPointer>
#include "sdvp_qtcommon/vehiclestate.h"
#include "sdvp_qtcommon/coordinatetransforms.h"
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>

class MavsdkVehicleConnection : public QObject
{
    Q_OBJECT
public:
    explicit MavsdkVehicleConnection(std::shared_ptr<mavsdk::System> system);
    QSharedPointer<VehicleState> getVehicleState() const;
    void setEnuReference(const llh_t &enuReference);
    void requestArm();
    void requestTakeoff();
    void requestGotoLlh(const llh_t &llh);

signals:
    void gotVehicleHomeLlh(const llh_t &homePositionLlh);

private:
    MAV_TYPE mVehicleType;
    llh_t mEnuReference;
    QSharedPointer<VehicleState> mVehicleState;
    std::shared_ptr<mavsdk::System> mSystem;
    std::shared_ptr<mavsdk::Telemetry> mTelemetry;
    std::shared_ptr<mavsdk::Action> mAction;
    std::shared_ptr<mavsdk::MavlinkPassthrough> mMavlinkPassthrough;
    QSharedPointer<QTimer> mPosTimer;
    bool mIsArmed;

};

#endif // MAVSDKVEHICLECONNECTION_H
