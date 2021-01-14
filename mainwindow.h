#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include "mavsdksystemconnector.h"
#include "sdvp_qtcommon/gnss/ublox_basestation.h"

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
    mavsdk::Mavsdk mMavsdk;
    QSharedPointer<MavsdkSystemConnector> mMavsdkSystemConnector;
    UbloxBasestation mUbloxBasestation;

    void newMavsdkSystem();
};
#endif // MAINWINDOW_H
