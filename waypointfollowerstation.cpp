#include "waypointfollowerstation.h"
#include <cmath>
#include <QDebug>
#include <QLineF>

WaypointFollowerStation::WaypointFollowerStation(QSharedPointer<MavsdkVehicleConnection> vehicleConnection)
{
    mVehicleConnection = vehicleConnection;
    connect(&mUpdateStateTimer, &QTimer::timeout, this, &WaypointFollowerStation::updateState);
}

void WaypointFollowerStation::clearRoute()
{
    mWaypointList.clear();
}

void WaypointFollowerStation::addWaypoint(const PosPoint &point)
{
    mWaypointList.append(point);
}

void WaypointFollowerStation::startFollowingRoute(bool fromBeginning)
{
    if (fromBeginning)
        mCurrentState.stmState = FOLLOW_ROUTE_INIT;


    mUpdateStateTimer.start(mUpdateStatePeriod_ms);
}

bool WaypointFollowerStation::isActive()
{
    return mUpdateStateTimer.isActive();
}

void WaypointFollowerStation::stop()
{
    mUpdateStateTimer.stop();
    // TODO: hover relative to moving base on stop?
}

void WaypointFollowerStation::resetState()
{
    mUpdateStateTimer.stop();

    mCurrentState.stmState = WayPointFollowerStationSTMstates::NONE;
    mCurrentState.currentWaypointIndex = mWaypointList.size();
}

double WaypointFollowerStation::getCurvatureToPointInENU(QSharedPointer<VehicleState> vehicleState, const QPointF &point, PosType vehiclePosType)
{
    // vehicleState and point assumed in ENU frame
    const PosPoint vehiclePos = vehicleState->getPosition(vehiclePosType);

    // 1. transform point to vehicle frame, TODO: general transform in vehicleState?
    QPointF pointInVehicleFrame;
    // translate
    pointInVehicleFrame.setX(point.x()-vehiclePos.getX());
    pointInVehicleFrame.setY(point.y()-vehiclePos.getY());
    // rotate
    double currYaw_rad = vehiclePos.getYaw() * M_PI / 180.0;
    const double newX = cos(currYaw_rad)*pointInVehicleFrame.x() - sin(currYaw_rad)*pointInVehicleFrame.y();
    const double newY = sin(currYaw_rad)*pointInVehicleFrame.x() + cos(currYaw_rad)*pointInVehicleFrame.y();
    pointInVehicleFrame.setX(newX);
    pointInVehicleFrame.setY(newY);

    return getCurvatureToPointInVehicleFrame(pointInVehicleFrame);
}

double WaypointFollowerStation::getCurvatureToPointInENU(const QPointF &point)
{
    return getCurvatureToPointInENU(mVehicleConnection->getVehicleState(), point, mPosTypeUsed);
}

double WaypointFollowerStation::getCurvatureToPointInVehicleFrame(const QPointF &point)
{
    // calc steering angle (pure pursuit)
    double distanceSquared = pow(point.x(), 2) + pow(point.y(), 2);
    double steeringAngleProportional = (2*point.y()) / distanceSquared;

    return -steeringAngleProportional;
}

// TODO: utility function, move to a more central place
QVector<QPointF> findIntersectionsBetweenCircleAndLine(QPair<QPointF,double> circle, QLineF line) {
    QVector<QPointF> intersections;

    double maxX = line.x1();
    double minX = line.x2();
    double maxY = line.y1();
    double minY = line.y2();
    if (maxX < minX) {
        maxX = line.x2();
        minX = line.x1();
    }
    if (maxY < minY) {
        maxY = line.y2();
        minY = line.y1();
    }

    double a = line.dx() * line.dx() + line.dy() * line.dy();
    double b = 2 * (line.dx() * (line.x1() - circle.first.x()) + line.dy() * (line.y1() - circle.first.y()));
    double c = (line.x1() - circle.first.x()) * (line.x1() - circle.first.x()) + (line.y1() - circle.first.y()) * (line.y1() - circle.first.y()) - circle.second * circle.second;

    double det = b * b - 4 * a * c;

    if ((a <= 1e-6) || (det < 0.0)) {
//         qDebug() << "No real solutions.";
    } else if (det == 0) {
//         qDebug() << "One solution.";
        double t = -b / (2 * a);
        double x = line.x1() + t * line.dx();
        double y = line.y1() + t * line.dy();

        if (x >= minX && x <= maxX &&
                y >= minY && y <= maxY)
            intersections.append(QPointF(x, y));
    } else {
//         qDebug() << "Two solutions.";
        double t = (-b + sqrtf(det)) / (2 * a);
        double x = line.x1() + t * line.dx();
        double y = line.y1() + t * line.dy();

        if (x >= minX && x <= maxX &&
                y >= minY && y <= maxY)
            intersections.append(QPointF(x, y));

        t = (-b - sqrtf(det)) / (2 * a);
        x = line.x1() + t * line.dx();
        y = line.y1() + t * line.dy();

        if (x >= minX && x <= maxX &&
                y >= minY && y <= maxY)
            intersections.append(QPointF(x, y));
    }

    return intersections;
}

void WaypointFollowerStation::updateState()
{
    QPointF currentVehiclePosition = mVehicleConnection->getVehicleState()->getPosition(mPosTypeUsed).getPoint();

    switch (mCurrentState.stmState) {
    case NONE:
        qDebug() << "WARNING: WayPointFollower running uninitialized statemachine.";
        break;

    // FOLLOW_ROUTE: waypoints describe a route to be followed waypoint by waypoint
    case FOLLOW_ROUTE_INIT:
        currentVehiclePosition = mVehicleConnection->getVehicleState()->getPosition(mPosTypeUsed).getPoint();

        if (mWaypointList.size()) {
            mCurrentState.currentWaypointIndex = 0;
            mCurrentState.currentGoal = mWaypointList.at(0);
            mCurrentState.stmState = FOLLOW_ROUTE_GOTO_BEGIN;
        } else
            mCurrentState.stmState = FOLLOW_ROUTE_FINISHED;
        break;

    case FOLLOW_ROUTE_GOTO_BEGIN: {
        currentVehiclePosition = mVehicleConnection->getVehicleState()->getPosition(mPosTypeUsed).getPoint();

        // draw straight line to first point and apply purePursuitRadius to find intersection
        QLineF carToStartLine(currentVehiclePosition, mWaypointList.at(0).getPoint());
        QVector<QPointF> intersections = findIntersectionsBetweenCircleAndLine(QPair<QPointF, double>(currentVehiclePosition, mCurrentState.purePursuitRadius), carToStartLine);

        if (intersections.size())
            mVehicleConnection->requestGotoENU(xyz_t {intersections[0].x(), intersections[0].y(), mCurrentState.overrideAltitude}); // TODO: use altitude from route and interpolate
        else // first waypoint within circle -> start route
            mCurrentState.stmState = FOLLOW_ROUTE_FOLLOWING;
    } break;

    case FOLLOW_ROUTE_FOLLOWING: {
        currentVehiclePosition = mVehicleConnection->getVehicleState()->getPosition(mPosTypeUsed).getPoint();
        QPointF currentWaypointPoint = mWaypointList.at(mCurrentState.currentWaypointIndex).getPoint();

        if (QLineF(currentVehiclePosition, currentWaypointPoint).length() < mCurrentState.purePursuitRadius) // consider previous waypoint as reached
            mCurrentState.currentWaypointIndex++;

        if (mCurrentState.currentWaypointIndex == mWaypointList.size() && !mCurrentState.repeatRoute)
                mCurrentState.stmState = FOLLOW_ROUTE_FINISHED;
        else {
            // --- Calculate current goal on route (which lies between two waypoints)
            // 1. Find intersection between circle around vehicle and route
            // look a number of points ahead and jump forward on route, if applicable
            // and take care of index wrap in case route is repeated
            QList<PosPoint> lookAheadWaypoints;
            if (mCurrentState.repeatRoute) {
                lookAheadWaypoints = mWaypointList.mid(mCurrentState.currentWaypointIndex - 1, mCurrentState.numWaypointsLookahead);

                const int lookaheadWaypointEndIndex = mCurrentState.currentWaypointIndex + mCurrentState.numWaypointsLookahead - 1;
                if (lookaheadWaypointEndIndex > mWaypointList.size()) // index wraparound
                    lookAheadWaypoints.append(mWaypointList.mid(0, lookaheadWaypointEndIndex % mWaypointList.size()));
                else if (mCurrentState.currentWaypointIndex == 0) // restarting from end to beginning
                    lookAheadWaypoints.prepend(mWaypointList.last());
            } else
                lookAheadWaypoints = mWaypointList.mid(mCurrentState.currentWaypointIndex - 1,  mCurrentState.numWaypointsLookahead);

            QVector<QPointF> intersections;
            for (int i = lookAheadWaypoints.size() - 1; i > 0; i--) { // step backwards through lookahead window until intersection is found
                QPointF iWaypoint = lookAheadWaypoints.at(i).getPoint();
                QLineF iLineSegment(lookAheadWaypoints.at(i-1).getPoint(), iWaypoint);

                intersections = findIntersectionsBetweenCircleAndLine(QPair<QPointF, double>(currentVehiclePosition, mCurrentState.purePursuitRadius), iLineSegment);
                if (intersections.size() > 0) {
                    mCurrentState.currentWaypointIndex = (i + mCurrentState.currentWaypointIndex - 1) % mWaypointList.size();
                    currentWaypointPoint = iWaypoint;
                    break;
                }
            }

            // 2. Set Goal depending on number of intersections found
            int previousWaypointIndex = mCurrentState.currentWaypointIndex - 1 >= 0 ? mCurrentState.currentWaypointIndex - 1 : mWaypointList.size() - 1;
            switch (intersections.size()) {
            case 0:
                // We seem to have left the route (e.g., because of high speed), reuse previous goal to  get back to route
                break;
            case 1:
                mCurrentState.currentGoal.setX(intersections[0].x());
                mCurrentState.currentGoal.setY(intersections[0].y());
                mCurrentState.currentGoal.setSpeed(getInterpolatedSpeed(mCurrentState.currentGoal, mWaypointList.at(previousWaypointIndex), mWaypointList.at(mCurrentState.currentWaypointIndex)));

                break;
            case 2:
                // Take intersection closest to current waypoint (most progress)
                if (QLineF(intersections[0], currentWaypointPoint).length()
                        < QLineF(intersections[1], currentWaypointPoint).length()) {
                    mCurrentState.currentGoal.setX(intersections[0].x());
                    mCurrentState.currentGoal.setY(intersections[0].y());
                    mCurrentState.currentGoal.setSpeed(getInterpolatedSpeed(mCurrentState.currentGoal, mWaypointList.at(previousWaypointIndex), mWaypointList.at(mCurrentState.currentWaypointIndex)));

                }
                else {
                    mCurrentState.currentGoal.setX(intersections[1].x());
                    mCurrentState.currentGoal.setY(intersections[1].y());
                    mCurrentState.currentGoal.setSpeed(getInterpolatedSpeed(mCurrentState.currentGoal, mWaypointList.at(previousWaypointIndex), mWaypointList.at(mCurrentState.currentWaypointIndex)));

                }
                break;
            default:
                break;
            }

            // 3. Determine closest waypoint to vehicle, it determines attributes
            PosPoint closestWaypoint;
            if (QLineF(currentVehiclePosition, mWaypointList.at(previousWaypointIndex).getPoint()).length()
                    < QLineF(currentVehiclePosition, mWaypointList.at(mCurrentState.currentWaypointIndex).getPoint()).length())
                closestWaypoint = mWaypointList.at(previousWaypointIndex);
            else
                closestWaypoint = mWaypointList.at(mCurrentState.currentWaypointIndex);

            // 4. Update control for current goal
            mVehicleConnection->requestGotoENU(xyz_t {mCurrentState.currentGoal.getY(), mCurrentState.currentGoal.getY(), mCurrentState.overrideAltitude}); // TODO: use altitude from route and interpolate
//            mMovementController->setDesiredAttributes(closestWaypoint.getAttributes());
        }
    } break;

    case FOLLOW_ROUTE_FINISHED:
        // TODO: hover relative to moving base?
        mUpdateStateTimer.stop();
        mCurrentState.stmState = NONE;
        mCurrentState.currentWaypointIndex = mWaypointList.size();
        break;

    default:
        break;
    }
}

PosType WaypointFollowerStation::getPosTypeUsed() const
{
    return mPosTypeUsed;
}

void WaypointFollowerStation::setPosTypeUsed(const PosType &posTypeUsed)
{
    mPosTypeUsed = posTypeUsed;
}

double WaypointFollowerStation::getPurePursuitRadius() const
{
    return mCurrentState.purePursuitRadius;
}

void WaypointFollowerStation::setPurePursuitRadius(double value)
{
    mCurrentState.purePursuitRadius = value;
}

bool WaypointFollowerStation::getRepeatRoute() const
{
    return mCurrentState.repeatRoute;
}

void WaypointFollowerStation::setRepeatRoute(bool value)
{
    mCurrentState.repeatRoute = value;
}

const PosPoint WaypointFollowerStation::getCurrentGoal()
{
    return mCurrentState.currentGoal;
}

double WaypointFollowerStation::getInterpolatedSpeed(const PosPoint &currentGoal, const PosPoint &lastWaypoint, const PosPoint &nextWaypoint)
{
    // Linear interpolation
    double distanceToNextWaypoint = currentGoal.getDistanceTo(nextWaypoint);
    double distanceBetweenWaypoints = lastWaypoint.getDistanceTo(nextWaypoint);
    double x = distanceBetweenWaypoints - distanceToNextWaypoint;

    return lastWaypoint.getSpeed() + (nextWaypoint.getSpeed()-lastWaypoint.getSpeed())*(x/distanceBetweenWaypoints);
}

void WaypointFollowerStation::setOverrideAltitude(double value)
{
    mCurrentState.overrideAltitude = value;
}
