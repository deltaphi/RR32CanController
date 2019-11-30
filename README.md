# RR32CanSwitchboard
Sketch for a Switchboard talking the Maerklin CAN protocol based on the ESP32 NodeMCU-Board.

## Software

This project is built using PlatformIO. It makes use of some features that may
not be easily portable to other Arduino-supported MCUs.

### Library Notes

This project makes use of several libraries. Versioned links to those libraries
are available as git submodules. Note that for seom libraries, using forks from
the same source as this project may be required.

Note that the CAN driver from the lib directory *must not* be present when you
want to compile using the ESP32-IDF driver. Otherwise, the Controller will just
attempt to sit there without sending queued messages or receiving any messages
transmitted on the bus.

### WiFi

This sketch only supports runtime-selection of CAN connection vs. WiFi
connection. The WiFi connection information (SSID, Password, IP of CAN-Gateway)
must be hardcoded.

To support hardcoding, a template file "_wificonfig.h" is provided. Copy the
template to "wificonfig.h" and adjust the configuration information to your
needs.

### Menuing

#### General concept

General: Shift + Direction Change enters the menu (cycles through menus) or leaves the menu without applying any changes.

Menu Levels:

* Level 0/Default: Loco Control
* Level 1: Loco List
* Level 2: Settings Menu

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

#### Encoder Acceleration for Engine Control

Formula for acceleration curve: y = m*x + c. x is the time since the last rotation in the same direction,
y is the number of actual increments to apply.

The acceleration is linear. It should be set up so that there is a cutoff at 500ms (no acceleration when
increments are more than half a second apart) and that an "immediate" half rotation (e.g., within 50ms)
hits the opposite end of engine velocity.

Thus, boundary conditions for an encoder with 24 increments per rotaton are:
1 Tick = m*500ms + c
kMaxEngineVelocity Ticks / 12 increments = m * (50ms / 12 increments) + c

Formulas for calculating m and c:

m = (y1 - y2)/(x1 - x2) = (1 - (kMaxEngineVelocity/12))/(500 - (50/12)) = -0.16

c = y1 - m*x1 = 84.03


#### Settings Menu

Menu Entries:

* Turnout mapping
* Action list mapping
* Default connection settings
* WiFi settings

##### Turnout Mapping

When in the Turnout mapping menu:

* Press a button (red or green) for a turnout
* Display shows number of button and whether red or green was pressed (line 1)
* Display shows human-readable number of currently mapped turnout
* Use encoder to adjust the number of the turnout
* pressing the encoder stores the turnout mapping (visual feedback possible?)
* pressing another turnout button switches to settings for that button
* pressing shift + encoder leaves the menu.

Possible extension: Use the function buttons to increase/decrease in steps of 10


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