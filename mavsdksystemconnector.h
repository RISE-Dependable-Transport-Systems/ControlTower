#ifndef MAVSDKSYSTEMCONNECTOR_H
#define MAVSDKSYSTEMCONNECTOR_H

#include <QObject>
#include <QWidget>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action/action.h>
#include "sdvp_qtcommon/mapwidget.h"
#include "sdvp_qtcommon/copterstate.h"

// Connects MAVSDK "System" to the SDVP Map. Positions are always handled in lat, lon, because GNSS RTK moving base is the main use case
class MavsdkSystemConnector : public QWidget, public MapModule
{
    Q_OBJECT
public:
    MavsdkSystemConnector() = delete;
    MavsdkSystemConnector(std::shared_ptr<mavsdk::System> system, MapWidget *map);

    // MapModule interface
    void processPaint(QPainter &painter, int width, int height, bool highQuality, QTransform drawTrans, QTransform txtTrans, double scale);
    bool processMouse(bool isPress, bool isRelease, bool isMove, bool isWheel, QPoint widgetPos, PosPoint mapPos, double wheelAngleDelta, Qt::KeyboardModifiers keyboardModifiers, Qt::MouseButtons mouseButtons, double scale);

private:
    // TODO try to clean up pointer type mess...
    QSharedPointer<CopterState> mCopterState;
    MapWidget *mMap;
    std::shared_ptr<mavsdk::System> mSystem;
    std::shared_ptr<mavsdk::Telemetry> mTelemetry;
    std::shared_ptr<mavsdk::Action> mAction;
};

#endif // MAVSDKSYSTEMCONNECTOR_H
