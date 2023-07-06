# ControlTower
![image](https://user-images.githubusercontent.com/2404625/202209615-afe01d3f-0408-4ebe-8c7e-539b10d8b27b.png)
A ground control station based on [WayWise](https://github.com/RISE-Dependable-Transport-Systems/WayWise) and [MAVSDK](http://mavsdk.io/) that can control [PX4](https://px4.io/)-based drones and [WayWise](https://github.com/RISE-Dependable-Transport-Systems/WayWise)-based vehicles via MAVLINK.
ControlTower was initially developed during the [LASH FIRE](https://lashfire.eu/) EU project to investigate the use of drones for fire prevention on ships ([public report](https://lashfire.eu/media/2023/03/LASH-FIRE_D07.7_Development-and-onboard-assessment-of-drone-for-assistance-in-firefighting-resource-management-and-rescue-operations_V03.pdf)).

MAVSDK commit 926b067 or newer is require for building, which will probably become MAVSDK 2.0. For the time being you need to build MAVSDK yourself or use the *.deb (Ubuntu 20.04/22.04) provided in the main folder of this repository.

## Installing Prerequisites (on Ubuntu 20.04/22.04) & Building
    sudo apt install git build-essential cmake qtcreator qtbase5-dev libqt5serialport5-dev qtmultimedia5-dev libqt5gamepad5-dev
    git clone --recursive git@github.com:RISE-Dependable-Transport-Systems/ControlTower.git
    
    # In case you do not want to build MAVSDK (main branch) yourself:
    sudo dpkg -i ControlTower/libmavsdk-dev_*_amd64.deb && sudo ln -s /usr/local/lib/libmavsdk.so.1.4.0 /usr/local/lib/libmavsdk.so.1
    
    mkdir build && cd build
    cmake ../ControlTower
    cmake --build . --parallel 
    
## Building for Windows using MXE
See: [doc/Building-for-Windows.md](https://github.com/RISE-Dependable-Transport-Systems/ControlTower/blob/main/doc/Building-for-Windows.md)
    
## Funded by
<img src="https://user-images.githubusercontent.com/2404625/202213271-a4006999-49d5-4e61-9f3d-867a469238d1.png" width="120" height="81" align="left" alt="EU logo" />
This project has received funding from the European Union’s Horizon 2020 research and innovation programme under grant agreement nº 814975. The results reflect only the author's view and the Agency is not responsible
for any use that may be made of the information it contains.
