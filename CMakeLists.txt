cmake_minimum_required(VERSION 3.5)

project(ControlTower LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

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

set(WAYWISE ${CMAKE_SOURCE_DIR}/WayWise)

add_executable(ControlTower
    resources.qrc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${WAYWISE}/logger/logger.cpp
    ${WAYWISE}/userinterface/driveui.cpp
    ${WAYWISE}/userinterface/driveui.h
    ${WAYWISE}/userinterface/driveui.ui
    ${WAYWISE}/userinterface/flyui.cpp
    ${WAYWISE}/userinterface/flyui.h
    ${WAYWISE}/userinterface/flyui.ui
    ${WAYWISE}/userinterface/planui.cpp
    ${WAYWISE}/userinterface/planui.h
    ${WAYWISE}/userinterface/planui.ui
    ${WAYWISE}/userinterface/cameragimbalui.cpp
    ${WAYWISE}/userinterface/cameragimbalui.h
    ${WAYWISE}/userinterface/cameragimbalui.ui
    ${WAYWISE}/userinterface/traceui.cpp
    ${WAYWISE}/userinterface/traceui.h
    ${WAYWISE}/userinterface/traceui.ui
    ${WAYWISE}/userinterface/ubloxbasestationui.cpp
    ${WAYWISE}/userinterface/ubloxbasestationui.h
    ${WAYWISE}/userinterface/ubloxbasestationui.ui
    ${WAYWISE}/userinterface/serialportdialog.cpp
    ${WAYWISE}/userinterface/serialportdialog.h
    ${WAYWISE}/userinterface/serialportdialog.ui
    ${WAYWISE}/userinterface/routegeneratorui.cpp
    ${WAYWISE}/userinterface/routegeneratorui.h
    ${WAYWISE}/userinterface/routegeneratorui.ui
    ${WAYWISE}/userinterface/routegeneratorzigzagui.cpp
    ${WAYWISE}/userinterface/routegeneratorzigzagui.h
    ${WAYWISE}/userinterface/routegeneratorzigzagui.ui
    ${WAYWISE}/routeplanning/zigzagroutegenerator.cpp
    ${WAYWISE}/routeplanning/zigzagroutegenerator.h
    ${WAYWISE}/routeplanning/routeutils.cpp
    ${WAYWISE}/routeplanning/routeutils.h
    ${WAYWISE}/userinterface/map/routeplannermodule.cpp
    ${WAYWISE}/userinterface/map/routeplannermodule.h
    ${WAYWISE}/userinterface/map/tracemodule.cpp
    ${WAYWISE}/userinterface/map/tracemodule.h
    ${WAYWISE}/userinterface/vehicleparameterui.h
    ${WAYWISE}/userinterface/vehicleparameterui.cpp
    ${WAYWISE}/userinterface/vehicleparameterui.ui
    ${WAYWISE}/communication/vehicleconnections/mavsdkstation.h
    ${WAYWISE}/communication/vehicleconnections/mavsdkstation.cpp
    ${WAYWISE}/communication/vehicleconnections/vehicleconnection.h
    ${WAYWISE}/communication/vehicleconnections/vehicleconnection.cpp
    ${WAYWISE}/communication/vehicleconnections/mavsdkvehicleconnection.h
    ${WAYWISE}/communication/vehicleconnections/mavsdkvehicleconnection.cpp
    ${WAYWISE}/communication/parameterserver.cpp
    ${WAYWISE}/vehicles/objectstate.cpp
    ${WAYWISE}/vehicles/objectstate.h
    ${WAYWISE}/vehicles/vehiclestate.cpp
    ${WAYWISE}/vehicles/vehiclestate.h
    ${WAYWISE}/vehicles/copterstate.cpp
    ${WAYWISE}/vehicles/copterstate.h
    ${WAYWISE}/vehicles/carstate.cpp
    ${WAYWISE}/vehicles/carstate.h
    ${WAYWISE}/vehicles/truckstate.cpp
    ${WAYWISE}/vehicles/truckstate.h
    ${WAYWISE}/vehicles/trailerstate.cpp
    ${WAYWISE}/vehicles/trailerstate.h
    ${WAYWISE}/autopilot/waypointfollower.h
    ${WAYWISE}/autopilot/gotowaypointfollower.cpp
    ${WAYWISE}/autopilot/gotowaypointfollower.h
    ${WAYWISE}/autopilot/followpoint.h
    ${WAYWISE}/autopilot/followpoint.cpp
    ${WAYWISE}/vehicles/controller/movementcontroller.cpp
    ${WAYWISE}/vehicles/controller/movementcontroller.h
#    ${WAYWISE}/autopilot/purepursuitwaypointfollower.cpp
#    ${WAYWISE}/autopilot/purepursuitwaypointfollower.h
    ${WAYWISE}/core/coordinatetransforms.h
    ${WAYWISE}/userinterface/map/mapwidget.cpp
    ${WAYWISE}/userinterface/map/mapwidget.h
    ${WAYWISE}/userinterface/map/osmclient.cpp
    ${WAYWISE}/userinterface/map/osmclient.h
    ${WAYWISE}/userinterface/map/osmtile.cpp
    ${WAYWISE}/userinterface/map/osmtile.h
    ${WAYWISE}/core/pospoint.cpp
    ${WAYWISE}/core/pospoint.h
    ${WAYWISE}/sensors/camera/gimbal.h
    ${WAYWISE}/sensors/camera/mavsdkgimbal.cpp
    ${WAYWISE}/sensors/camera/mavsdkgimbal.h
    ${WAYWISE}/sensors/gnss/rtcm3_simple.cpp
    ${WAYWISE}/sensors/gnss/rtcm3_simple.h
    ${WAYWISE}/sensors/gnss/ublox.cpp
    ${WAYWISE}/sensors/gnss/ublox.h
    ${WAYWISE}/sensors/gnss/ublox_basestation.cpp
    ${WAYWISE}/sensors/gnss/ublox_basestation.h
)

target_include_directories(ControlTower PRIVATE ${CMAKE_SOURCE_DIR} ${WAYWISE})

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
