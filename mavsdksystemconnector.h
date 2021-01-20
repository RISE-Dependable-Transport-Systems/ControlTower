#ifndef MAVSDKSYSTEMCONNECTOR_H
#define MAVSDKSYSTEMCONNECTOR_H

#include <QObject>
#include <QWidget>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include "sdvp_qtcommon/mapwidget.h"
#include "sdvp_qtcommon/copterstate.h"
#include "sdvp_qtcommon/coordinatetransforms.h"

// Connects MAVSDK "System" to the SDVP Map. Positions are always handled in lat, lon, because GNSS RTK moving base is the main use case
class MavsdkSystemConnector : public QWidget, public MapModule
{
    Q_OBJECT
public:
    MavsdkSystemConnector() = delete;
    MavsdkSystemConnector(std::shared_ptr<mavsdk::System> system, llh_t const *enuReference);

    // MapModule interface
    void processPaint(QPainter &painter, int width, int height, bool highQuality, QTransform drawTrans, QTransform txtTrans, double scale);
    bool processMouse(bool isPress, bool isRelease, bool isMove, bool isWheel, QPoint widgetPos, PosPoint mapPos, double wheelAngleDelta, Qt::KeyboardModifiers keyboardModifiers, Qt::MouseButtons mouseButtons, double scale);

    void forwardRtcmDataToSystem(const QByteArray& data, const int &type);

    QSharedPointer<CopterState> getCopterState() const;

signals:
    void systemHomeLlh(const llh_t &homePositionLlh);

private:
    llh_t const *mEnuReference;
    QSharedPointer<CopterState> mCopterState;
    std::shared_ptr<mavsdk::System> mSystem;
    std::shared_ptr<mavsdk::Telemetry> mTelemetry;
    std::shared_ptr<mavsdk::Action> mAction;
    std::shared_ptr<mavsdk::MavlinkPassthrough> mMavlinkPassthrough;
};

#endif // MAVSDKSYSTEMCONNECTOR_H
