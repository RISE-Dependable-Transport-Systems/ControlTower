#include "mavsdkvehicleconnection.h"
#include "sdvp_qtcommon/copterstate.h"
#include <QDebug>

MavsdkVehicleConnection::MavsdkVehicleConnection(std::shared_ptr<mavsdk::System> system)
{
    mSystem = system;

    // TODO: create respective class depending on MAV_TYPE (but not accessible in MAVSDK?!)
    mVehicleType = MAV_TYPE::MAV_TYPE_QUADROTOR;
    switch (mVehicleType) {
    case MAV_TYPE_QUADROTOR:
        mVehicleState = QSharedPointer<CopterState>::create(mSystem->get_system_id());
        mVehicleState->setName("Copter " + QString::number(mSystem->get_system_id()));
        break;
    case MAV_TYPE_GROUND_ROVER:
        // TODO: car or diffdrive?
        qDebug() << "MAV_TYPE_GROUND_ROVER not implemented.";
        break;
    default:
        break;
    }

    // Set up telemetry plugin
    mTelemetry.reset(new mavsdk::Telemetry(mSystem));

    mTelemetry->subscribe_home([this](mavsdk::Telemetry::Position position) {
        llh_t llh = {position.latitude_deg, position.longitude_deg, position.absolute_altitude_m};
        xyz_t xyz = coordinateTransforms::llhToEnu(mEnuReference, llh);

        auto homePos = mVehicleState->getHomePosition();
        homePos.setX(xyz.x);
        homePos.setY(xyz.y);
        homePos.setHeight(xyz.z);
        mVehicleState->setHomePosition(homePos);

        emit gotVehicleHomeLlh({position.latitude_deg, position.longitude_deg, position.absolute_altitude_m});
    });

    mTelemetry->subscribe_position([this](mavsdk::Telemetry::Position position) {
        llh_t llh = {position.latitude_deg, position.longitude_deg, position.absolute_altitude_m};
        xyz_t xyz = coordinateTransforms::llhToEnu(mEnuReference, llh);

        auto pos = mVehicleState->getPosition();
        pos.setX(xyz.x);
        pos.setY(xyz.y);
        pos.setHeight(xyz.z);

        mVehicleState->setPosition(pos);
    });

    mTelemetry->subscribe_attitude_quaternion([this](mavsdk::Telemetry::Quaternion q) {
        auto pos = mVehicleState->getPosition();
        pos.setYaw(atan2f(q.w * q.z + q.x * q.y, 0.5 - (q.y * q.y + q.z * q.z))); // extract yaw from quaternion
        mVehicleState->setPosition(pos);
    });

    mTelemetry->subscribe_velocity_ned([this](mavsdk::Telemetry::VelocityNed velocity) {
        VehicleState::Velocity velocityCopter {velocity.east_m_s, velocity.north_m_s, -velocity.down_m_s};
        mVehicleState->setVelocity(velocityCopter);
    });

    if (mVehicleType == MAV_TYPE::MAV_TYPE_QUADROTOR)
        mTelemetry->subscribe_landed_state([this](mavsdk::Telemetry::LandedState landedState) {
           mVehicleState.dynamicCast<CopterState>()->setLandedState(static_cast<CopterState::LandedState>(landedState));
        });

    // Set up action plugin
    mAction.reset(new mavsdk::Action(mSystem));

    // Set up MAVLINK passthrough to send rtcm data to drone (no plugin exists for this in MAVSDK v1.0.8)
    // TODO: use to set home position on system?
    mMavlinkPassthrough.reset(new mavsdk::MavlinkPassthrough(mSystem));

}

QSharedPointer<VehicleState> MavsdkVehicleConnection::getVehicleState() const
{
    return mVehicleState;
}

void MavsdkVehicleConnection::setEnuReference(const llh_t &enuReference)
{
    // TODO: set on vehicle as well (impact on EKF?)
    mEnuReference = enuReference;
}

void MavsdkVehicleConnection::requestArm()
{
    mAction->arm_async([this](mavsdk::Action::Result res){
        if (res != mavsdk::Action::Result::Success)
            qDebug() << "Warning: MavsdkVehicleConnection's arm request failed.";
        else
            mIsArmed = true;
    });
}

void MavsdkVehicleConnection::requestTakeoff()
{
    if (mVehicleType == MAV_TYPE::MAV_TYPE_QUADROTOR) {
        mAction->takeoff_async([](mavsdk::Action::Result res){
            if (res != mavsdk::Action::Result::Success)
                qDebug() << "Warning: MavsdkVehicleConnection's takeoff request failed.";
            else
                qDebug() << "Taking off...";
        });
    } else
        qDebug() << "Warning: MavsdkVehicleConnection is trying to take off with an unknown/incompatible vehicle type, ignored.";
}

void MavsdkVehicleConnection::requestGotoLlh(const llh_t &llh)
{
    mAction->goto_location_async(llh.latitude, llh.longitude, llh.height, 0, [&llh](mavsdk::Action::Result res){
        if (res != mavsdk::Action::Result::Success)
            qDebug() << "Warning: MavsdkVehicleConnection's goto request failed.";
        else
            qDebug() << "Going to " << llh.latitude << llh.longitude << llh.height;
    });
}

void MavsdkVehicleConnection::requestGotoENU(const xyz_t &xyz, bool convertToGlobalBeforeSending)
{
    if (convertToGlobalBeforeSending) {
        llh_t llh = coordinateTransforms::enuToLlh(mEnuReference, xyz);
        requestGotoLlh(llh);
    } else {
        qDebug() << "MavsdkVehicleConnection::requestGotoENU: sending local coordinates to vehicle without converting not implemented.";
    }
}
