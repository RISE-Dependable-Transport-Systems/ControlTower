#ifndef PLANUI_H
#define PLANUI_H

#include <QWidget>
#include <QSharedPointer>
#include "routeplannermodule.h"

namespace Ui {
class PlanUI;
}

class PlanUI : public QWidget
{
    Q_OBJECT

public:
    explicit PlanUI(QWidget *parent = nullptr);
    ~PlanUI();

    QSharedPointer<RoutePlannerModule> getRoutePlanner() const;

signals:
    void routeDoneForUse(const QList<PosPoint>& route);

private slots:
    void on_addRouteButton_clicked();

    void on_removeRouteButton_clicked();

    void on_currentRouteSpinBox_valueChanged(int value);

    void on_sendToAutopilotButton_clicked();

private:
    Ui::PlanUI *ui;
    QSharedPointer<RoutePlannerModule> mRoutePlanner;
};

#endif // PLANUI_H