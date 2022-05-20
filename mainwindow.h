#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userinterface/flyui.h"
#include <mavsdk/plugins/telemetry/telemetry.h>
#include "communication/vehicleconnections/mavsdkstation.h"
#include "sensors/gnss/ublox_basestation.h"
#include "userinterface/serialportdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_AddSerialConnectionAction_triggered();

private:
    Ui::MainWindow *ui;
    QSharedPointer<MavsdkStation> mMavsdkStation;
    UbloxBasestation mUbloxBasestation;
    QTimer mPreclandTestTimer;
    QSharedPointer<SerialPortDialog> mSerialPortDialog;

    void newMavsdkSystem();
    void setDarkStyle();
};
#endif // MAINWINDOW_H
