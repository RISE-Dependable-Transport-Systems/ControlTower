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

// TODO: make optional (set ENU from basestation, if connected)
    mTelemetry->subscribe_home([this](mavsdk::Telemetry::Position position) {
        mMap->setEnuRef(position.latitude_deg, position.longitude_deg, position.absolute_altitude_m);
        mMap->setTraceVehicle(0);
    });

    mTelemetry->subscribe_velocity_ned([this](mavsdk::Telemetry::VelocityNed velocity) {
        VehicleState::Velocity velocityCopter {velocity.east_m_s, velocity.north_m_s, -velocity.down_m_s};
        mCopterState->setVelocity(velocityCopter);
    });

    mTelemetry->subscribe_landed_state([this](mavsdk::Telemetry::LandedState landedState) {
       mCopterState->setLandedState(static_cast<CopterState::LandedState>(landedState));
    });

    // Set up action plugin
    mAction.reset(new mavsdk::Action(mSystem));

    // Set up MAVLINK passthrough to send rtcm data to drone (no plugin exists for this in MAVSDK v0.35.1)
    mMavlinkPassthrough.reset(new mavsdk::MavlinkPassthrough(mSystem));
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

bool MavsdkSystemConnector::processMouse(bool isPress, bool isRelease, bool isMove, bool isWheel, QPoint widgetPos, PosPoint mapPos, double wheelAngleDelta, Qt::KeyboardModifiers keyboardModifiers, Qt::MouseButtons mouseButtons, double scale)
{
    Q_UNUSED(isRelease);
    Q_UNUSED(isMove);
    Q_UNUSED(isWheel);
    Q_UNUSED(widgetPos);
    Q_UNUSED(wheelAngleDelta);
    Q_UNUSED(keyboardModifiers);
    Q_UNUSED(mouseButtons);
    Q_UNUSED(scale);

    bool eventWasHandled = false;

    // TODO: quick test, no sanity checks, result handling etc.
    if (isPress && keyboardModifiers == (Qt::ControlModifier | Qt::AltModifier)) {
        if (mouseButtons == Qt::MouseButton::LeftButton) {
        double Llh[3];

        double iLlh[3];
        mMap->getEnuRef(iLlh);

        double xyz[3] = {mapPos.getX(), mapPos.getY(), mCopterState->getPosition().getHeight()};
        MapWidget::enuToLlh(iLlh, xyz, Llh);


        // qDebug() << Llh[0] << Llh[1] <<  Llh[2];

        if (mCopterState->getLandedState() != CopterState::LandedState::InAir) {
            mAction->arm_async([](mavsdk::Action::Result ){});
            mAction->takeoff_async([](mavsdk::Action::Result ){});
        } else
            mAction->goto_location_async(Llh[0], Llh[1], Llh[2], 0, [](mavsdk::Action::Result ){});

        eventWasHandled = true;
        } else if (mouseButtons == Qt::MouseButton::MiddleButton)
            mAction->land_async([](mavsdk::Action::Result ){});
    }

    return eventWasHandled;
}

void MavsdkSystemConnector::forwardRtcmDataToSystem(const QByteArray &data, const int& type)
{
    Q_UNUSED(type);
    // TODO: not tested whether a real drone can get an RTK fix this way.

    if (mMavlinkPassthrough == nullptr)
        return;

    if (data.length() > MAVLINK_MSG_ID_GPS_RTCM_DATA_LEN) {
        qDebug() << "WARNING: unable to send rtcm" << type << "via MAVLINK, too big with" << data.length() << "bytes. Discarded.";
        return;
    }

    mavlink_gps_rtcm_data_t mavRtcmData;
    mavRtcmData.flags = 0u;
    mavRtcmData.len = data.length();
    std::copy(data.begin(), data.end(), std::begin(mavRtcmData.data));
    std::fill(std::begin(mavRtcmData.data) + mavRtcmData.len, std::end(mavRtcmData.data), 0);

    mavlink_message_t mavRtcmMsg;
    mavlink_msg_gps_rtcm_data_encode(mMavlinkPassthrough->get_our_sysid(), mMavlinkPassthrough->get_our_compid() /* TODO -> 220? */, &mavRtcmMsg, &mavRtcmData);

    if (mMavlinkPassthrough->send_message(mavRtcmMsg) != mavsdk::MavlinkPassthrough::Result::Success)
        qDebug() << "ERROR while forwarding rtcm via MAVLINK.";
}
