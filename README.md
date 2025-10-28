# ControlTower
![image](https://user-images.githubusercontent.com/2404625/202209615-afe01d3f-0408-4ebe-8c7e-539b10d8b27b.png)
A ground control station based on [WayWise](https://github.com/RISE-Dependable-Transport-Systems/WayWise) and [MAVSDK](http://mavsdk.io/) that can control [PX4](https://px4.io/)-based drones and [WayWise](https://github.com/RISE-Dependable-Transport-Systems/WayWise)-based vehicles via MAVLINK.
ControlTower was initially developed during the [LASH FIRE](https://lashfire.eu/) EU project to investigate the use of drones for fire prevention on ships ([public report](https://lashfire.eu/media/2023/03/LASH-FIRE_D07.7_Development-and-onboard-assessment-of-drone-for-assistance-in-firefighting-resource-management-and-rescue-operations_V03.pdf)).

MAVSDK 2.0 is required (support for version 3.0 is not yet available). Pre-built releases can be found at https://github.com/mavlink/MAVSDK/releases. To instead build MAVSDK from source, simple [scripts can be found in the WayWise repository](https://github.com/RISE-Dependable-Transport-Systems/WayWise/tree/main/tools/build_MAVSDK).

## Installing Prerequisites (on Ubuntu 20.04/22.04) & Building
    # Installing MAVSDK
    sudo dpkg -i libmavsdk-dev*.deb
    
    sudo apt install git build-essential cmake qtcreator qtbase5-dev libqt5serialport5-dev qtmultimedia5-dev libqt5gamepad5-dev
    git clone --recursive git@github.com:RISE-Dependable-Transport-Systems/ControlTower.git
    cd ControlTower
    mkdir build && cd build
    cmake ..
    make -j4 

## Building for Windows using MXE
See: [doc/Building-for-Windows.md](https://github.com/RISE-Dependable-Transport-Systems/ControlTower/blob/main/doc/Building-for-Windows.md)


<img src="https://user-images.githubusercontent.com/2404625/202213271-a4006999-49d5-4e61-9f3d-867a469238d1.png" width="120" height="81" align="left" alt="EU logo" />
This project has received funding from the European Union’s Horizon 2020 and Horizon Europe research and innovation programmes, and the Digital Europe programme under grant agreement nº 814975, nº 101095835, 101069573 and nº 101100622. The results reflect only the authors' view and the Agency is not responsible
for any use that may be made of the information it contains.
