#include "mavsdksystemconnector.h"

MavsdkSystemConnector::MavsdkSystemConnector(std::shared_ptr<mavsdk::System> system, MapWidget *map)
{
    mSystem = system;
    mMap = map;
    mCopterState.reset(new CopterState);

    mCopterState->setName("Copter " + QString::number(mSystem->get_system_id()));
    mMap->addVehicle(mCopterState);
    mMap->addMapModule(QSharedPointer<MapModule>(this));

    // Set up telemetry plugin
    mTelemetry.reset(new mavsdk::Telemetry(mSystem));

    mTelemetry->subscribe_position([this](mavsdk::Telemetry::Position position) {
        double Llh[3] = {position.latitude_deg, position.longitude_deg, position.absolute_altitude_m};

        double iLlh[3];
        mMap->getEnuRef(iLlh);

        double xyz[3];
        MapWidget::llhToEnu(iLlh, Llh, xyz);

        auto pos = mCopterState->getPosition();
        pos.setX(xyz[0]);
        pos.setY(xyz[1]);
        pos.setHeight(position.relative_altitude_m);
        mCopterState->setPosition(pos);
    });

    mTelemetry->subscribe_attitude_quaternion([this](mavsdk::Telemetry::Quaternion q) {
        auto pos = mCopterState->getPosition();
        pos.setYaw(atan2f(q.w * q.z + q.x * q.y, 0.5 - (q.y * q.y + q.z * q.z))); // extract yaw from quaternion
        mCopterState->setPosition(pos);
    });

    mTelemetry->subscribe_home([this](mavsdk::Telemetry::Position position) {
        mMap->setEnuRef(position.latitude_deg, position.longitude_deg, position.absolute_altitude_m);
        mMap->setTraceVehicle(0);
    });

    mTelemetry->subscribe_velocity_ned([this](mavsdk::Telemetry::VelocityNed velocity) {
        VehicleState::Velocity velocityCopter {velocity.east_m_s, velocity.north_m_s, -velocity.down_m_s};
        mCopterState->setVelocity(velocityCopter);
    });

    // Set up action plugin
    mAction.reset(new mavsdk::Action(mSystem));
}

void MavsdkSystemConnector::processPaint(QPainter &painter, int width, int height, bool highQuality, QTransform drawTrans, QTransform txtTrans, double scale)
{
    Q_UNUSED(painter);
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(highQuality);
    Q_UNUSED(drawTrans);
    Q_UNUSED(txtTrans);
    Q_UNUSED(scale);

}

bool MavsdkSystemConnector::processMouse(bool isPress, bool isRelease, bool isMove, bool isWheel, QPoint widgetPos, PosPoint mapPos, double wheelAngleDelta, bool ctrl, bool shift, bool ctrlShift, bool leftButton, bool rightButton, double scale)
{
    Q_UNUSED(isRelease);
    Q_UNUSED(isMove);
    Q_UNUSED(isWheel);
    Q_UNUSED(widgetPos);
    Q_UNUSED(wheelAngleDelta);
    Q_UNUSED(ctrl);
    Q_UNUSED(shift);
    Q_UNUSED(ctrlShift);
    Q_UNUSED(leftButton);
    Q_UNUSED(rightButton);
    Q_UNUSED(scale);

    // TODO: quick test, no sanity checks etc.
    if (isPress) {
        double Llh[3];

        double iLlh[3];
        mMap->getEnuRef(iLlh);

        double xyz[3] = {mapPos.getX(), mapPos.getY(), mCopterState->getPosition().getHeight()};
        MapWidget::enuToLlh(iLlh, xyz, Llh);


        qDebug() << Llh[0] << Llh[1] <<  Llh[2];
        mAction->goto_location_async(Llh[0], Llh[1], Llh[2], 0, [](mavsdk::Action::Result result){});
    }

}
