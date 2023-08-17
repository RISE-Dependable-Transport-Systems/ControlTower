# Building for Windows using MXE

MXE allows you to cross-compile Windows applications under Linux. More information: https://mxe.cc/

Instructions below were tested under Ubuntu 22.04.

## Setting up MXE
Make sure requirements are installed:

    sudo apt install autoconf automake autopoint bash bison bzip2 flex g++ g++-multilib gettext git gperf intltool \
    libc6-dev-i386 libgdk-pixbuf2.0-dev libltdl-dev libgl-dev libpcre3-dev libssl-dev libtool-bin libxml-parser-perl \
    lzip make openssl p7zip-full patch perl python3 python3-distutils python3-mako python3-pkg-resources python-is-python3 \
    ruby sed unzip wget xz-utils

Create install folder and clone sources:

    sudo mkdir /opt/mxe && sudo chown $USER /opt/mxe
    git clone https://github.com/mxe/mxe.git /opt/mxe

Create a settings.mk file in /opt/mxe with the following contents:

    JOBS := 4
    MXE_TARGETS := x86_64-w64-mingw32.static
    MXE_USE_CCACHE=

Build MXE, this will take a while:

`make qtbase qtserialport qtgamepad qtmultimedia cmake curl jsoncpp tinyxml2`

Make sure MXE is in $PATH by adding the following lines to ~/.bashrc (and starting a new shell session afterwards):

    export PATH=/opt/mxe/usr/bin:$PATH
    export MXE_TARGETS='x86_64-w64-mingw32.static'

## Build MAVSDK using MXE

    cd ~/src # create first if it does not exist
    git clone --recursive https://github.com/mavlink/MAVSDK.git
    mkdir -p ~/src/MAVSDK/build/win

A quickfix for building with MXE (a headerfile that needs to be written lowercase instead of uppercase):

`find ~/src/MAVSDK/src/mavsdk \( -type d -name .git -prune \) -o -type f -print0 | xargs -0 sed -i 's/Ws2tcpip\.h/ws2tcpip\.h/g'`

Now cmake should run successfully. Then you are ready to build and install MAVSDK:

    cd ~/src/MAVSDK/build/win
    x86_64-w64-mingw32.static-cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DSUPERBUILD=OFF ../..
    make -j5
    make install

## Build ControlTower using MXE

    cd ~/src
    git clone --recursive git@github.com:RISE-Dependable-Transport-Systems/ControlTower.git
    mkdir -p ControlTower/win_build
    cd ControlTower/win_build

A few lines in MAVSDK's provided cmake file at /opt/mxe/usr/x86_64-w64-mingw32.static/lib/cmake/MAVSDK/MAVSDKConfig.cmake need to be changed (after every install/update of MAVSDK). Here is the diff (add five lines for Threads and PkgConfig packages, disable the if statement):

    3,9c3
    < find_package(Threads)
    < find_package(PkgConfig)
    < pkg_check_modules(PC_CURL QUIET IMPORTED_TARGET GLOBAL libcurl)
    < pkg_check_modules(PC_JSONCPP QUIET IMPORTED_TARGET GLOBAL jsoncpp)
    < pkg_check_modules(PC_TINYXML2 QUIET IMPORTED_TARGET GLOBAL tinyxml2)
    < 
    < if(OFF)
    ---
    > if(NOT OFF)

Now cmake and make can run as usual (for MXE builds):

    x86_64-w64-mingw32.static-cmake ..
    make -j5

When everything succeeds, you get a ControlTower.exe that can be run under Windows.
