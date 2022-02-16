#ifndef ROUTEPLANNERMODULE_H
#define ROUTEPLANNERMODULE_H

#include "sdvp_qtcommon/mapwidget.h"

class RoutePlannerModule : public MapModule
{
public:
    RoutePlannerModule();

    // MapModule interface
    virtual void processPaint(QPainter &painter, int width, int height, bool highQuality, QTransform drawTrans, QTransform txtTrans, double scale) override;
    virtual bool processMouse(bool isPress, bool isRelease, bool isMove, bool isWheel, QPoint widgetPos, PosPoint mapPos, double wheelAngleDelta, Qt::KeyboardModifiers keyboardModifiers, Qt::MouseButtons mouseButtons, double scale) override;

private:
    typedef enum {
        Default,
        Inactive,
        LAST
    } RoutePointType;

    struct {
        int currentRouteIndex = 0;
        bool drawRouteText = true;
    } mPlannerState;

    void drawRoute(QPainter &painter, QTransform drawTrans, QTransform txtTrans, bool highQuality, double scaleFactor, const QList<PosPoint> &route, int routeID, bool isSelected, bool drawAnnotations);
    void drawCircleFast(QPainter &painter, QPointF center, double radius, int type);
    void addRoutePoint(double px, double py, double speed, QTime time);

    QList<QPixmap> mPixmaps;
    QList<QList<PosPoint> > mRoutes;
    int getClosestPoint(PosPoint p, QList<PosPoint> points, double &dist);
};

#endif // ROUTEPLANNERMODULE_H
