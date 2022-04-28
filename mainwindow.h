#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "flyui.h"
#include <mavsdk/plugins/telemetry/telemetry.h>
#include "mavsdkstation.h"
#include "sensors/gnss/ublox_basestation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSharedPointer<MavsdkStation> mMavsdkStation;
    UbloxBasestation mUbloxBasestation;
    QTimer mPreclandTestTimer;

    void newMavsdkSystem();
    void setDarkStyle();
};
#endif // MAINWINDOW_H
