#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userinterface/flyui.h"
#include <mavsdk/plugins/telemetry/telemetry.h>
#include "communication/vehicleconnections/mavsdkstation.h"
#include "sensors/gnss/ublox_basestation.h"
#include "userinterface/serialportdialog.h"
#include "connecttogazebo.h"
#include "communication/TCPClient.h"


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
    void on_actionConnectToGazebo_triggered();

    void on_addUdpConnectionAction_triggered();

private:
    Ui::MainWindow *ui;
    QSharedPointer<MavsdkStation> mMavsdkStation;
    QSharedPointer<TCPClient> mTcpClient;

    UbloxBasestation mUbloxBasestation;
    QTimer mPreclandTestTimer;
    QSharedPointer<SerialPortDialog> mSerialPortDialog;
    QSharedPointer<ConnectToGazebo> mGazeboDialog;

    

    void newMavsdkSystem();
    void setDarkStyle();
};
#endif // MAINWINDOW_H
