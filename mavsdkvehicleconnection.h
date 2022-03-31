#ifndef MAVSDKVEHICLECONNECTION_H
#define MAVSDKVEHICLECONNECTION_H

#include <QSharedPointer>
#include "sdvp_qtcommon/vehicleconnection.h"
#include "sdvp_qtcommon/vehiclestate.h"
#include "sdvp_qtcommon/coordinatetransforms.h"
#include "sdvp_qtcommon/waypointfollower.h"
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>

class MavsdkVehicleConnection : public VehicleConnection
{
    Q_OBJECT
public:
    explicit MavsdkVehicleConnection(std::shared_ptr<mavsdk::System> system);
    void setEnuReference(const llh_t &enuReference);
    void setHomeLlh(const llh_t &homeLlh);
    void requestArm();
    void requestDisarm();
    void requestTakeoff();
    void requestLanding();
    void requestReturnToHome();
    void requestGotoLlh(const llh_t &llh);
    virtual void requestGotoENU(const xyz_t &xyz) override;
    void inputRtcmData(const QByteArray &rtcmData);

    void setConvertLocalPositionsToGlobalBeforeSending(bool convertLocalPositionsToGlobalBeforeSending);

    void setWaypointFollower(const QSharedPointer<WaypointFollower> &waypointFollower);
    QSharedPointer<WaypointFollower> getWaypointFollower() const;
    bool hasWaypointFollower();

signals:
    void gotVehicleHomeLlh(const llh_t &homePositionLlh);

private:
    MAV_TYPE mVehicleType;
    llh_t mEnuReference;
    bool mConvertLocalPositionsToGlobalBeforeSending = false;
    std::shared_ptr<mavsdk::System> mSystem;
    std::shared_ptr<mavsdk::Telemetry> mTelemetry;
    std::shared_ptr<mavsdk::Action> mAction;
    std::shared_ptr<mavsdk::MavlinkPassthrough> mMavlinkPassthrough;
    QSharedPointer<QTimer> mPosTimer;
    QSharedPointer<WaypointFollower> mWaypointFollower;
};

#endif // MAVSDKVEHICLECONNECTION_H
