# Building for Windows using MXE

MXE allows you to cross-compile Windows applications under Linux. More information: https://mxe.cc/

## Setting up MXE

    sudo mkdir /opt/mxe && sudo chown $USER /opt/mxe
    git clone https://github.com/mxe/mxe.git /opt/mxe

Create a settings.mk file in /opt/mxe with the following contents:

    JOBS := 4
    MXE_TARGETS := x86_64-w64-mingw32.static
    MXE_USE_CCACHE=

Build MXE, this will take a while:

`make qtbase qtserialport qtgamepad qtmultimedia cmake`

Make sure MXE is in $PATH by adding the following lines to ~/.bashrc (and starting a new shell session afterwards):

    export PATH=/opt/mxe/usr/bin:$PATH
    export MXE_TARGETS='x86_64-w64-mingw32.static'

## Build MAVSDK using MXE

    cd ~/src # create first if it does not exist
    git clone --recursive https://github.com/mavlink/MAVSDK.git
    mkdir -p ~/src/MAVSDK/build/win

**TODO:** All following steps (including building ControlTower) are will lead to a working \*.exe file, but are hackish and will throw errors.
Some hackish fix for building with MXE is to add the following lines to $USER/src/MAVSDKsrc/CMakeLists.txt right after "set(CMAKE_CXX_STANDARD_REQUIRED ON)". Make sure to replace "marvind" with your username:

    set(CURL_LIBRARY /home/marvind/src/MAVSDK/build/win/third_party/install/lib)
    set(CURL_INCLUDE_DIR /home/marvind/src/MAVSDK/build/win/third_party/install/include)
    set(JsonCpp_LIBRARY /home/marvind/src/MAVSDK/build/win/third_party/install/lib)
    set(JsonCpp_INCLUDE_DIR /home/marvind/src/MAVSDK/build/win/third_party/install/include)
    set(TINYXML2_LIBRARY /home/marvind/src/MAVSDK/build/win/third_party/install/lib)
    set(TINYXML2_INCLUDE_DIR /home/marvind/src/MAVSDK/build/win/third_party/install/include)

Another fix for building with MXE (a headerfile that needs to be written lowercase):

`find ~/src/MAVSDK/src/mavsdk \( -type d -name .git -prune \) -o -type f -print0 | xargs -0 sed -i 's/Ws2tcpip\.h/ws2tcpip\.h/g'`

Now run CMake:

    cd ~/src/MAVSDK/build/win
    x86_64-w64-mingw32.static-cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ../..
    make -j5
    cd src/mavsdk
    make install

## Build ControlTower using MXE

    cd ~/src
    git clone --recursive git@github.com:RISE-Dependable-Transport-Systems/ControlTower.git
    mkdir -p ControlTower/win_build
    cd ControlTower/win_build

A few lines need to be changed in ControlTower's CMakeLists.txt (in ~/src/ControlTower, as a result from errors during the MAVSDK build), this is the diff:

    diff --git a/CMakeLists.txt b/CMakeLists.txt
    index 1ed40f8..f523623 100644
    --- a/CMakeLists.txt
    +++ b/CMakeLists.txt
    @@ -25,7 +25,7 @@ set(CMAKE_CXX_STANDARD_REQUIRED ON)
     #endif()
     
     find_package(Qt5 COMPONENTS Widgets Core Network PrintSupport SerialPort Multimedia MultimediaWidgets Gamepad REQUIRED)
    -find_package(MAVSDK REQUIRED)
    +#find_package(MAVSDK REQUIRED)
     
     add_executable(ControlTower
         resources.qrc
    @@ -108,6 +108,7 @@ add_executable(ControlTower
     )
     
     target_include_directories(ControlTower PRIVATE WayWise/)
    +target_include_directories(ControlTower PRIVATE /opt/mxe/usr/x86_64-w64-mingw32.static/include/mavsdk)
     
     target_link_libraries(ControlTower
         PRIVATE Qt5::Widgets
    @@ -117,5 +118,6 @@ target_link_libraries(ControlTower
         PRIVATE Qt5::Multimedia
         PRIVATE Qt5::MultimediaWidgets
         PRIVATE Qt5::Gamepad
    -    PRIVATE MAVSDK::mavsdk
    +    PRIVATE mavsdk
    +    PRIVATE /home/marvind/src/MAVSDK/build/win/third_party/install/lib/libjsoncpp.a
     )

Now you should be ready to build ControlTower (fingers crossed!):

    make -j5

When everything succeeds, you get a ControlTower.exe that can be run under Windows.
