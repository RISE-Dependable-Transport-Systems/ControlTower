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
    flyui.cpp
    flyui.h
    flyui.ui
    planui.cpp
    planui.h
    planui.ui
    traceui.cpp
    traceui.h
    traceui.ui
    ubloxbasestationui.cpp
    ubloxbasestationui.h
    ubloxbasestationui.ui
    routeplannermodule.cpp
    routeplannermodule.h
    tracemodule.cpp
    tracemodule.h
    mavsdkstation.h
    mavsdkstation.cpp
    sdvp_qtcommon/vehicleconnection.cpp
    sdvp_qtcommon/vehicleconnection.h
    mavsdkvehicleconnection.h
    mavsdkvehicleconnection.cpp
    sdvp_qtcommon/objectstate.cpp
    sdvp_qtcommon/objectstate.h
    sdvp_qtcommon/vehiclestate.cpp
    sdvp_qtcommon/vehiclestate.h
    sdvp_qtcommon/copterstate.cpp
    sdvp_qtcommon/copterstate.h
    sdvp_qtcommon/movementcontroller.cpp
    sdvp_qtcommon/movementcontroller.h
    sdvp_qtcommon/waypointfollower.cpp
    sdvp_qtcommon/waypointfollower.h
    sdvp_qtcommon/coordinatetransforms.h
    sdvp_qtcommon/mapwidget.cpp
    sdvp_qtcommon/mapwidget.h
    sdvp_qtcommon/osmclient.cpp
    sdvp_qtcommon/osmclient.h
    sdvp_qtcommon/osmtile.cpp
    sdvp_qtcommon/osmtile.h
    sdvp_qtcommon/pospoint.cpp
    sdvp_qtcommon/pospoint.h
    sdvp_qtcommon/gnss/rtcm3_simple.cpp
    sdvp_qtcommon/gnss/rtcm3_simple.h
    sdvp_qtcommon/gnss/ublox.cpp
    sdvp_qtcommon/gnss/ublox.h
    sdvp_qtcommon/gnss/ublox_basestation.cpp
    sdvp_qtcommon/gnss/ublox_basestation.h
  )
else()
  add_executable(ControlTower
    resources.qrc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    flyui.cpp
    flyui.h
    flyui.ui
    planui.cpp
    planui.h
    planui.ui
    traceui.cpp
    traceui.h
    traceui.ui
    ubloxbasestationui.cpp
    ubloxbasestationui.h
    ubloxbasestationui.ui
    routeplannermodule.cpp
    routeplannermodule.h
    tracemodule.cpp
    tracemodule.h
    mavsdkstation.h
    mavsdkstation.cpp
    sdvp_qtcommon/vehicleconnection.cpp
    sdvp_qtcommon/vehicleconnection.h
    mavsdkvehicleconnection.h
    mavsdkvehicleconnection.cpp
    sdvp_qtcommon/objectstate.cpp
    sdvp_qtcommon/objectstate.h
    sdvp_qtcommon/vehiclestate.cpp
    sdvp_qtcommon/vehiclestate.h
    sdvp_qtcommon/copterstate.cpp
    sdvp_qtcommon/copterstate.h
    sdvp_qtcommon/movementcontroller.cpp
    sdvp_qtcommon/movementcontroller.h
    sdvp_qtcommon/waypointfollower.cpp
    sdvp_qtcommon/waypointfollower.h
    sdvp_qtcommon/coordinatetransforms.h
    sdvp_qtcommon/mapwidget.cpp
    sdvp_qtcommon/mapwidget.h
    sdvp_qtcommon/osmclient.cpp
    sdvp_qtcommon/osmclient.h
    sdvp_qtcommon/osmtile.cpp
    sdvp_qtcommon/osmtile.h
    sdvp_qtcommon/pospoint.cpp
    sdvp_qtcommon/pospoint.h
    sdvp_qtcommon/gnss/rtcm3_simple.cpp
    sdvp_qtcommon/gnss/rtcm3_simple.h
    sdvp_qtcommon/gnss/ublox.cpp
    sdvp_qtcommon/gnss/ublox.h
    sdvp_qtcommon/gnss/ublox_basestation.cpp
    sdvp_qtcommon/gnss/ublox_basestation.h
  )
endif()

target_link_libraries(ControlTower
    PRIVATE Qt5::Widgets
    PRIVATE Qt5::Network
    PRIVATE Qt5::PrintSupport
    PRIVATE Qt5::SerialPort
    PRIVATE MAVSDK::mavsdk
)
