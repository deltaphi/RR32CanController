# RR32CanSwitchboard
Sketch for a Switchboard talking the Maerklin CAN protocol based on the ESP32 NodeMCU-Board.

## Software

This project is built using PlatformIO. It makes use of some features that may not be easily portable to other Arduino-supported MCUs.

Note that the CAN driver from the lib directory *must not* be present when you want to compile using the ESP32-IDF driver. Otherwise, the Controller will just attempt to sit there without sending queued messages or receiving any messages transmitted on the bus.

### Menuing

#### General concept

General: Shift + Direction Change enters the menu or leaves the menu without applying any changes.

#### Menuing for Accessories

Menuing is required to map turnout input buttons to turnouts or action lists.

TODO: Design.

#### Menuing for Engine Control

Menuing is required for selecting an engine to control. This involves the following usecases:

* Select locally known engine
    * Show list of locally known Engines
    * Select engine
    * Selected Engine is now the active engine

* Select locally unknown engine
    * Verify that at least one local slot for an engine is open
    * If not:
        * Promt the user to remove an engine from the local database to be able to download a new one.
        * Show list of locally known Engines
        * Select an engine.
        * Selected Engine is marked for replacement
    * If yes:
        * Empty engine slot with lowest index is marked for replacement
    * Download list of Engines & total number of known engines from the master controller.
    * Either: Select engine from the master list
        * Details of Selected engine are downloaded.
    * Either: Scroll up/down beyond the currently known set of engine names.
        * If the engine name is unknown, download list of engines from the master controller.
    * Once the download is complete, the selected engine is now the active engine

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