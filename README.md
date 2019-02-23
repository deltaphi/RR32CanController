# RR32CanSwitchboard
Sketch for a Switchboard talking the Maerklin CAN protocol based on the ESP32 NodeMCU-Board

## Software

This project is built using PlatformIO. It makes use of some features that may not be easily portable to other Arduino-supported MCUs.

### Unit Testing

This project includes a CMake project.
The CMake project is purely used to run unit tests for some parts of the code on your development machine.
You do not need to deal with unit tests (or the CMake project) unless you want to actively develop the software.
To use the CMake project, create an out-of-tree build directoy (in-tree build will confuse the Automagic of PlatformIO).
Unit testing should be possible using any C++14 capable compiler on any Desktop Operating System.
It has been tested successefully using MSVC 2017 Community Edition.

## Hardware

### PCB

There is a KiCAD PCB design included for turning this project into a handheld device.
The PCB is designed for the [Strapubox 2090 case](https://www.reichelt.de/kunststoffgehaeuse-129-x-40-x-26-mm-sp-2090-sw-p33828.html?&trstct=pos_0).