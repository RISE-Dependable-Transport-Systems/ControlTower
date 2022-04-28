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

find_package(Qt5 COMPONENTS Widgets Core Network PrintSupport SerialPort REQUIRED)
find_package(MAVSDK REQUIRED)

if(ANDROID)
  add_library(ControlTower SHARED
    resources.qrc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    sdvp_qtcommon/userinterface/flyui.cpp
    sdvp_qtcommon/userinterface/flyui.h
    sdvp_qtcommon/userinterface/flyui.ui
    sdvp_qtcommon/userinterface/planui.cpp
    sdvp_qtcommon/userinterface/planui.h
    sdvp_qtcommon/userinterface/planui.ui
    sdvp_qtcommon/userinterface/traceui.cpp
    sdvp_qtcommon/userinterface/traceui.h
    sdvp_qtcommon/userinterface/traceui.ui
    sdvp_qtcommon/userinterface/ubloxbasestationui.cpp
    sdvp_qtcommon/userinterface/ubloxbasestationui.h
    sdvp_qtcommon/userinterface/ubloxbasestationui.ui
    sdvp_qtcommon/userinterface/map/routeplannermodule.cpp
    sdvp_qtcommon/userinterface/map/routeplannermodule.h
    sdvp_qtcommon/userinterface/map/tracemodule.cpp
    sdvp_qtcommon/userinterface/map/tracemodule.h
    sdvp_qtcommon/communication/vehicleconnections/mavsdkstation.h
    sdvp_qtcommon/communication/vehicleconnections/mavsdkstation.cpp
    sdvp_qtcommon/communication/vehicleconnections/vehicleconnection.cpp
    sdvp_qtcommon/communication/vehicleconnections/vehicleconnection.h
    sdvp_qtcommon/communication/vehicleconnections/mavsdkvehicleconnection.h
    sdvp_qtcommon/communication/vehicleconnections/mavsdkvehicleconnection.cpp
    sdvp_qtcommon/vehicles/objectstate.cpp
    sdvp_qtcommon/vehicles/objectstate.h
    sdvp_qtcommon/vehicles/vehiclestate.cpp
    sdvp_qtcommon/vehicles/vehiclestate.h
    sdvp_qtcommon/vehicles/copterstate.cpp
    sdvp_qtcommon/vehicles/copterstate.h
    sdvp_qtcommon/vehicles/controller/movementcontroller.cpp
    sdvp_qtcommon/vehicles/controller/movementcontroller.h
    sdvp_qtcommon/autopilot/waypointfollower.cpp
    sdvp_qtcommon/autopilot/waypointfollower.h
    sdvp_qtcommon/core/coordinatetransforms.h
    sdvp_qtcommon/userinterface/map/mapwidget.cpp
    sdvp_qtcommon/userinterface/map/mapwidget.h
    sdvp_qtcommon/userinterface/map/osmclient.cpp
    sdvp_qtcommon/userinterface/map/osmclient.h
    sdvp_qtcommon/userinterface/map/osmtile.cpp
    sdvp_qtcommon/userinterface/map/osmtile.h
    sdvp_qtcommon/core/pospoint.cpp
    sdvp_qtcommon/core/pospoint.h
    sdvp_qtcommon/sensors/gnss/rtcm3_simple.cpp
    sdvp_qtcommon/sensors/gnss/rtcm3_simple.h
    sdvp_qtcommon/sensors/gnss/ublox.cpp
    sdvp_qtcommon/sensors/gnss/ublox.h
    sdvp_qtcommon/sensors/gnss/ublox_basestation.cpp
    sdvp_qtcommon/sensors/gnss/ublox_basestation.h
  )
else()
  add_executable(ControlTower
    resources.qrc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    sdvp_qtcommon/userinterface/flyui.cpp
    sdvp_qtcommon/userinterface/flyui.h
    sdvp_qtcommon/userinterface/flyui.ui
    sdvp_qtcommon/userinterface/planui.cpp
    sdvp_qtcommon/userinterface/planui.h
    sdvp_qtcommon/userinterface/planui.ui
    sdvp_qtcommon/userinterface/traceui.cpp
    sdvp_qtcommon/userinterface/traceui.h
    sdvp_qtcommon/userinterface/traceui.ui
    sdvp_qtcommon/userinterface/ubloxbasestationui.cpp
    sdvp_qtcommon/userinterface/ubloxbasestationui.h
    sdvp_qtcommon/userinterface/ubloxbasestationui.ui
    sdvp_qtcommon/userinterface/map/routeplannermodule.cpp
    sdvp_qtcommon/userinterface/map/routeplannermodule.h
    sdvp_qtcommon/userinterface/map/tracemodule.cpp
    sdvp_qtcommon/userinterface/map/tracemodule.h
    sdvp_qtcommon/communication/vehicleconnections/mavsdkstation.h
    sdvp_qtcommon/communication/vehicleconnections/mavsdkstation.cpp
    sdvp_qtcommon/communication/vehicleconnections/vehicleconnection.cpp
    sdvp_qtcommon/communication/vehicleconnections/vehicleconnection.h
    sdvp_qtcommon/communication/vehicleconnections/mavsdkvehicleconnection.h
    sdvp_qtcommon/communication/vehicleconnections/mavsdkvehicleconnection.cpp
    sdvp_qtcommon/vehicles/objectstate.cpp
    sdvp_qtcommon/vehicles/objectstate.h
    sdvp_qtcommon/vehicles/vehiclestate.cpp
    sdvp_qtcommon/vehicles/vehiclestate.h
    sdvp_qtcommon/vehicles/copterstate.cpp
    sdvp_qtcommon/vehicles/copterstate.h
    sdvp_qtcommon/vehicles/controller/movementcontroller.cpp
    sdvp_qtcommon/vehicles/controller/movementcontroller.h
    sdvp_qtcommon/autopilot/waypointfollower.cpp
    sdvp_qtcommon/autopilot/waypointfollower.h
    sdvp_qtcommon/core/coordinatetransforms.h
    sdvp_qtcommon/userinterface/map/mapwidget.cpp
    sdvp_qtcommon/userinterface/map/mapwidget.h
    sdvp_qtcommon/userinterface/map/osmclient.cpp
    sdvp_qtcommon/userinterface/map/osmclient.h
    sdvp_qtcommon/userinterface/map/osmtile.cpp
    sdvp_qtcommon/userinterface/map/osmtile.h
    sdvp_qtcommon/core/pospoint.cpp
    sdvp_qtcommon/core/pospoint.h
    sdvp_qtcommon/sensors/gnss/rtcm3_simple.cpp
    sdvp_qtcommon/sensors/gnss/rtcm3_simple.h
    sdvp_qtcommon/sensors/gnss/ublox.cpp
    sdvp_qtcommon/sensors/gnss/ublox.h
    sdvp_qtcommon/sensors/gnss/ublox_basestation.cpp
    sdvp_qtcommon/sensors/gnss/ublox_basestation.h
  )
endif()

target_include_directories(ControlTower PRIVATE sdvp_qtcommon/)

target_link_libraries(ControlTower
    PRIVATE Qt5::Widgets
    PRIVATE Qt5::Network
    PRIVATE Qt5::PrintSupport
    PRIVATE Qt5::SerialPort
    PRIVATE MAVSDK::mavsdk
)
