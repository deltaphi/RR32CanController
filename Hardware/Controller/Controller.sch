EESchema Schematic File Version 4
LIBS:Controller-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Mechanical:MountingHole H2
U 1 1 5C67C49F
P 10000 6300
F 0 "H2" H 10100 6346 50  0000 L CNN
F 1 "Case Screw" H 10100 6255 50  0000 L CNN
F 2 "MountingHole:MountingHole_5.3mm_M5" H 10000 6300 50  0001 C CNN
F 3 "~" H 10000 6300 50  0001 C CNN
	1    10000 6300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 5C67C5A6
P 10000 6050
F 0 "H1" H 10100 6096 50  0000 L CNN
F 1 "Case Screw" H 10100 6005 50  0000 L CNN
F 2 "MountingHole:MountingHole_5.3mm_M5" H 10000 6050 50  0001 C CNN
F 3 "~" H 10000 6050 50  0001 C CNN
	1    10000 6050
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H6
U 1 1 5C67C6B3
P 10000 5750
F 0 "H6" H 10100 5796 50  0000 L CNN
F 1 "PCB Screw" H 10100 5705 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 10000 5750 50  0001 C CNN
F 3 "~" H 10000 5750 50  0001 C CNN
	1    10000 5750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H5
U 1 1 5C67C6E9
P 10000 5500
F 0 "H5" H 10100 5546 50  0000 L CNN
F 1 "PCB Screw" H 10100 5455 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 10000 5500 50  0001 C CNN
F 3 "~" H 10000 5500 50  0001 C CNN
	1    10000 5500
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5C67C721
P 10000 5250
F 0 "H4" H 10100 5296 50  0000 L CNN
F 1 "PCB Screw" H 10100 5205 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 10000 5250 50  0001 C CNN
F 3 "~" H 10000 5250 50  0001 C CNN
	1    10000 5250
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5C67C753
P 10000 5000
F 0 "H3" H 10100 5046 50  0000 L CNN
F 1 "PCB Screw" H 10100 4955 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 10000 5000 50  0001 C CNN
F 3 "~" H 10000 5000 50  0001 C CNN
	1    10000 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5C688870
P 9650 6300
F 0 "#PWR0101" H 9650 6050 50  0001 C CNN
F 1 "GND" H 9655 6127 50  0000 C CNN
F 2 "" H 9650 6300 50  0001 C CNN
F 3 "" H 9650 6300 50  0001 C CNN
	1    9650 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 5100 9850 5100
Wire Wire Line
	9850 5100 9850 5350
Wire Wire Line
	9850 6300 9650 6300
Wire Wire Line
	10000 5850 9850 5850
Connection ~ 9850 5850
Wire Wire Line
	10000 5600 9850 5600
Connection ~ 9850 5600
Wire Wire Line
	9850 5600 9850 5850
Wire Wire Line
	10000 5350 9850 5350
Connection ~ 9850 5350
Wire Wire Line
	9850 5350 9850 5600
Wire Wire Line
	9850 5850 9850 6300
$Comp
L RF_Module:ESP32-WROOM-32D U1
U 1 1 5C688B86
P 3800 3200
F 0 "U1" H 3800 4778 50  0000 C CNN
F 1 "ESP32-WROOM-32D" H 3800 4687 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 3800 1700 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf" H 3500 3250 50  0001 C CNN
	1    3800 3200
	1    0    0    -1  
$EndComp
$Comp
L Connector:8P8C J1
U 1 1 5C688D10
P 1900 6750
F 0 "J1" H 1955 7417 50  0000 C CNN
F 1 "8P8C" H 1955 7326 50  0000 C CNN
F 2 "Connector_RJ:RJ45_Amphenol_54602-x08_Horizontal" V 1900 6775 50  0001 C CNN
F 3 "~" V 1900 6775 50  0001 C CNN
	1    1900 6750
	1    0    0    -1  
$EndComp
$EndSCHEMATC
