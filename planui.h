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

private slots:
    void on_addRouteButton_clicked();

    void on_removeRouteButton_clicked();

    void on_currentRouteSpinBox_valueChanged(int value);

private:
    Ui::PlanUI *ui;
    QSharedPointer<RoutePlannerModule> mRoutePlanner;
};

#endif // PLANUI_H
