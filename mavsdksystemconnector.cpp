#include "mavsdksystemconnector.h"

MavsdkSystemConnector::MavsdkSystemConnector(std::shared_ptr<mavsdk::System> system, llh_t const *enuReference)
{
    mSystem = system;
    mEnuReference = enuReference;
    mCopterState.reset(new CopterState);

    mCopterState->setName("Copter " + QString::number(mSystem->get_system_id()));

    // Set up telemetry plugin
    mTelemetry.reset(new mavsdk::Telemetry(mSystem));

    mTelemetry->subscribe_position([this](mavsdk::Telemetry::Position position) {
        llh_t llh = {position.latitude_deg, position.longitude_deg, position.absolute_altitude_m};
        xyz_t xyz = coordinateTransforms::llhToEnu(*mEnuReference, llh);

        auto pos = mCopterState->getPosition();
        pos.setX(xyz.x);
        pos.setY(xyz.y);
        pos.setHeight(xyz.z);

        mCopterState->setPosition(pos);
    });

    mTelemetry->subscribe_attitude_quaternion([this](mavsdk::Telemetry::Quaternion q) {
        auto pos = mCopterState->getPosition();
        pos.setYaw(atan2f(q.w * q.z + q.x * q.y, 0.5 - (q.y * q.y + q.z * q.z))); // extract yaw from quaternion
        mCopterState->setPosition(pos);
    });

    mTelemetry->subscribe_home([this](mavsdk::Telemetry::Position position) {
        llh_t llh = {position.latitude_deg, position.longitude_deg, position.absolute_altitude_m};
        xyz_t xyz = coordinateTransforms::llhToEnu(*mEnuReference, llh);

        auto homePos = mCopterState->getHomePosition();
        homePos.setX(xyz.x);
        homePos.setY(xyz.y);
        homePos.setHeight(xyz.z);
        mCopterState->setHomePosition(homePos);

        emit systemHomeLlh({position.latitude_deg, position.longitude_deg, position.absolute_altitude_m});
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
    // TODO: use to set home position on system?
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
            xyz_t xyz = {mapPos.getX(), mapPos.getY(), mCopterState->getPosition().getHeight()};
            llh_t llh = coordinateTransforms::enuToLlh(*mEnuReference, xyz);

            if (mCopterState->getLandedState() != CopterState::LandedState::InAir) {
                mAction->arm_async([](mavsdk::Action::Result ){});
                mAction->takeoff_async([](mavsdk::Action::Result ){});
            } else
                mAction->goto_location_async(llh.latitude, llh.longitude, llh.height, 0, [](mavsdk::Action::Result ){});

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
    // TODO: needs adjustments and support for fragmented messages, see:
    //       https://github.com/mavlink/qgroundcontrol/blob/aba881bf8e3f2fdbf63ef0689a3bf0432f597759/src/GPS/RTCM/RTCMMavlink.cc#L24

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

QSharedPointer<CopterState> MavsdkSystemConnector::getCopterState() const
{
    return mCopterState;
}
