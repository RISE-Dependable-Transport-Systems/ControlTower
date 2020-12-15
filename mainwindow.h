#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include "sdvp_qtcommon/copterstate.h"

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
    QSharedPointer<CopterState> mCopterState;
    mavsdk::Mavsdk mMavsdk;
    std::shared_ptr<mavsdk::System> mSystem;
    std::shared_ptr<mavsdk::Telemetry> mTelemetry;

    void newMavsdkSystem();
};
#endif // MAINWINDOW_H
