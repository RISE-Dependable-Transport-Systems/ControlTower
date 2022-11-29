cmake_minimum_required(VERSION 3.5)

project(ControlTower LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets Core Network PrintSupport SerialPort Multimedia MultimediaWidgets Gamepad REQUIRED)
find_package(MAVSDK REQUIRED)

add_executable(ControlTower
    resources.qrc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    WayWise/userinterface/driveui.cpp
    WayWise/userinterface/driveui.h
    WayWise/userinterface/driveui.ui
    WayWise/userinterface/flyui.cpp
    WayWise/userinterface/flyui.h
    WayWise/userinterface/flyui.ui
    WayWise/userinterface/planui.cpp
    WayWise/userinterface/planui.h
    WayWise/userinterface/planui.ui
    WayWise/userinterface/cameragimbalui.cpp
    WayWise/userinterface/cameragimbalui.h
    WayWise/userinterface/cameragimbalui.ui
    WayWise/userinterface/traceui.cpp
    WayWise/userinterface/traceui.h
    WayWise/userinterface/traceui.ui
    WayWise/userinterface/ubloxbasestationui.cpp
    WayWise/userinterface/ubloxbasestationui.h
    WayWise/userinterface/ubloxbasestationui.ui
    WayWise/userinterface/serialportdialog.cpp
    WayWise/userinterface/serialportdialog.h
    WayWise/userinterface/serialportdialog.ui
    WayWise/userinterface/routegeneratorui.cpp
    WayWise/userinterface/routegeneratorui.h
    WayWise/userinterface/routegeneratorui.ui
    WayWise/userinterface/routegeneratorzigzagui.cpp
    WayWise/userinterface/routegeneratorzigzagui.h
    WayWise/userinterface/routegeneratorzigzagui.ui
    WayWise/routeplanning/zigzagroutegenerator.cpp
    WayWise/routeplanning/zigzagroutegenerator.h
    WayWise/userinterface/map/routeplannermodule.cpp
    WayWise/userinterface/map/routeplannermodule.h
    WayWise/userinterface/map/tracemodule.cpp
    WayWise/userinterface/map/tracemodule.h
    WayWise/communication/vehicleconnections/mavsdkstation.h
    WayWise/communication/vehicleconnections/mavsdkstation.cpp
    WayWise/communication/vehicleconnections/vehicleconnection.h
    WayWise/communication/vehicleconnections/vehicleconnection.cpp
    WayWise/communication/vehicleconnections/mavsdkvehicleconnection.h
    WayWise/communication/vehicleconnections/mavsdkvehicleconnection.cpp
    WayWise/vehicles/objectstate.cpp
    WayWise/vehicles/objectstate.h
    WayWise/vehicles/vehiclestate.cpp
    WayWise/vehicles/vehiclestate.h
    WayWise/vehicles/copterstate.cpp
    WayWise/vehicles/copterstate.h
    WayWise/vehicles/carstate.cpp
    WayWise/vehicles/carstate.h
    WayWise/autopilot/waypointfollower.h
    WayWise/autopilot/gotowaypointfollower.cpp
    WayWise/autopilot/gotowaypointfollower.h
#    WayWise/vehicles/controller/movementcontroller.cpp
#    WayWise/vehicles/controller/movementcontroller.h
#    WayWise/autopilot/purepursuitwaypointfollower.cpp
#    WayWise/autopilot/purepursuitwaypointfollower.h
    WayWise/core/coordinatetransforms.h
    WayWise/userinterface/map/mapwidget.cpp
    WayWise/userinterface/map/mapwidget.h
    WayWise/userinterface/map/osmclient.cpp
    WayWise/userinterface/map/osmclient.h
    WayWise/userinterface/map/osmtile.cpp
    WayWise/userinterface/map/osmtile.h
    WayWise/core/pospoint.cpp
    WayWise/core/pospoint.h
    WayWise/sensors/camera/gimbal.h
    WayWise/sensors/camera/mavsdkgimbal.cpp
    WayWise/sensors/camera/mavsdkgimbal.h
    WayWise/sensors/gnss/rtcm3_simple.cpp
    WayWise/sensors/gnss/rtcm3_simple.h
    WayWise/sensors/gnss/ublox.cpp
    WayWise/sensors/gnss/ublox.h
    WayWise/sensors/gnss/ublox_basestation.cpp
    WayWise/sensors/gnss/ublox_basestation.h
)

target_include_directories(ControlTower PRIVATE WayWise/)

target_link_libraries(ControlTower
    PRIVATE Qt5::Widgets
    PRIVATE Qt5::Network
    PRIVATE Qt5::PrintSupport
    PRIVATE Qt5::SerialPort
    PRIVATE Qt5::Multimedia
    PRIVATE Qt5::MultimediaWidgets
    PRIVATE Qt5::Gamepad
    PRIVATE MAVSDK::mavsdk
)
