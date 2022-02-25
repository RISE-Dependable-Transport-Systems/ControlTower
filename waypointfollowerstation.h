#ifndef WAYPOINTFOLLOWERSTATION_H
#define WAYPOINTFOLLOWERSTATION_H

#include <QObject>
#include <QSharedPointer>
#include <QPointF>
#include <QTimer>
#include "sdvp_qtcommon/vehiclestate.h"
#include "mavsdkvehicleconnection.h" // TODO: generalize

enum WayPointFollowerStationSTMstates {NONE, FOLLOW_ROUTE_INIT, FOLLOW_ROUTE_GOTO_BEGIN, FOLLOW_ROUTE_FOLLOWING, FOLLOW_ROUTE_FINISHED};
struct WayPointFollowerStationState {
    WayPointFollowerStationSTMstates stmState = NONE;
    PosPoint currentGoal;
    int currentWaypointIndex;
    double purePursuitRadius = 1.0;
    // Follow Route
    double overrideAltitude = 2.5; // TODO: require to be set (-1 initially...)
    int numWaypointsLookahead = 8;
    bool repeatRoute = false;
};

// Implementation of LASH FIRE use cases, based on a copy of waypointfollower.
// TODO: generalize and merge/replace waypointfollower? we need to see how this develops...
class WaypointFollowerStation : public QObject
{
    Q_OBJECT
public:
    WaypointFollowerStation();

    double getPurePursuitRadius() const;
    void setPurePursuitRadius(double value);

    bool getRepeatRoute() const;
    void setRepeatRoute(bool value);

    void setOverrideAltitude(double value);

    const PosPoint getCurrentGoal();

    void clearRoute();
    void addRoute(const QList<PosPoint>& route);
    void addWaypoint(const PosPoint &point);

    void startFollowingRoute(bool fromBeginning);
    bool isActive();
    void stop();
    void resetState();

    double getInterpolatedSpeed(const PosPoint &currentGoal, const PosPoint &lastWaypoint, const PosPoint &nextWaypoint);

    PosType getPosTypeUsed() const;
    void setPosTypeUsed(const PosType &posTypeUsed);

    void setVehicleConnection(const QSharedPointer<MavsdkVehicleConnection> &vehicleConnection);

signals:

private:
    void updateState();
    WayPointFollowerStationState mCurrentState;

    PosType mPosTypeUsed = PosType::simulated; // The type of position (Odom, GNSS, UWB, ...) that should be used for planning
    QSharedPointer<MavsdkVehicleConnection> mVehicleConnection;
    QList <PosPoint> mWaypointList;
    const unsigned mUpdateStatePeriod_ms = 50;
    QTimer mUpdateStateTimer;

};

#endif // WAYPOINTFOLLOWERSTATION_H
