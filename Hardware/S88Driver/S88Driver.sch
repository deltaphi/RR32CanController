EESchema Schematic File Version 4
EELAYER 30 0
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
L 74xx:74LS367 U1
U 1 1 5E07A95D
P 2050 1700
F 0 "U1" H 2050 2581 50  0000 C CNN
F 1 "74LS367" H 2050 2490 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm_LongPads" H 2050 1700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS367" H 2050 1700 50  0001 C CNN
	1    2050 1700
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J2
U 1 1 5E07B5C5
P 3350 1500
F 0 "J2" H 3378 1476 50  0000 L CNN
F 1 "Conn_01x06_Female" H 3378 1385 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Horizontal" H 3350 1500 50  0001 C CNN
F 3 "~" H 3350 1500 50  0001 C CNN
	1    3350 1500
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 5E07CBC2
P 800 1500
F 0 "J1" H 908 1881 50  0000 C CNN
F 1 "Conn_01x06_Male" H 908 1790 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Vertical" H 800 1500 50  0001 C CNN
F 3 "~" H 800 1500 50  0001 C CNN
	1    800  1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 1400 1250 1400
Wire Wire Line
	1250 1400 1250 2400
Wire Wire Line
	1250 2400 1500 2400
Wire Wire Line
	2850 2400 2850 1400
Wire Wire Line
	2850 1400 3150 1400
Connection ~ 2050 2400
Wire Wire Line
	2050 2400 2850 2400
Wire Wire Line
	1000 1800 1200 1800
Wire Wire Line
	1200 1800 1200 1000
Wire Wire Line
	1200 1000 1450 1000
Wire Wire Line
	3100 1800 3150 1800
Connection ~ 2050 1000
Wire Wire Line
	2050 1000 2850 1000
Wire Wire Line
	1000 1300 1000 900 
Wire Wire Line
	1000 900  3150 900 
Wire Wire Line
	3150 900  3150 1300
Wire Wire Line
	1000 1500 1300 1500
Wire Wire Line
	1300 1500 1300 1300
Wire Wire Line
	1300 1300 1550 1300
Wire Wire Line
	1000 1600 1350 1600
Wire Wire Line
	1350 1600 1350 1400
Wire Wire Line
	1350 1400 1550 1400
Wire Wire Line
	1000 1700 1400 1700
Wire Wire Line
	1400 1700 1400 1500
Wire Wire Line
	1400 1500 1550 1500
Wire Wire Line
	1550 1600 1500 1600
Connection ~ 1500 2400
Wire Wire Line
	1500 2400 2050 2400
Wire Wire Line
	1550 1700 1500 1700
Wire Wire Line
	1500 1600 1500 1700
Connection ~ 1500 1700
Wire Wire Line
	1500 1700 1500 1800
Wire Wire Line
	1550 1800 1500 1800
Connection ~ 1500 1800
Wire Wire Line
	1500 1800 1500 2000
Wire Wire Line
	2550 1300 2800 1300
Wire Wire Line
	2800 1300 2800 1500
Wire Wire Line
	2800 1500 3150 1500
Wire Wire Line
	2550 1400 2750 1400
Wire Wire Line
	2750 1400 2750 1600
Wire Wire Line
	2750 1600 3150 1600
Wire Wire Line
	2550 1500 2700 1500
Wire Wire Line
	2700 1500 2700 1700
Wire Wire Line
	2700 1700 3150 1700
Wire Wire Line
	1550 2000 1500 2000
Connection ~ 1500 2000
Wire Wire Line
	1500 2000 1500 2400
Wire Wire Line
	1550 2100 1450 2100
Wire Wire Line
	1450 2100 1450 1000
Connection ~ 1450 1000
Wire Wire Line
	1450 1000 2050 1000
$Comp
L Device:C C1
U 1 1 5E084923
P 2850 1150
F 0 "C1" H 2965 1196 50  0000 L CNN
F 1 "100n" H 2965 1105 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 2888 1000 50  0001 C CNN
F 3 "~" H 2850 1150 50  0001 C CNN
	1    2850 1150
	1    0    0    -1  
$EndComp
Connection ~ 2850 1000
Wire Wire Line
	2850 1000 3100 1000
Wire Wire Line
	3100 1800 3100 1000
Wire Wire Line
	2850 1300 2850 1400
Connection ~ 2850 1400
$Comp
L power:VCC #PWR0101
U 1 1 5E0878A9
P 2850 800
F 0 "#PWR0101" H 2850 650 50  0001 C CNN
F 1 "VCC" H 2867 973 50  0000 C CNN
F 2 "" H 2850 800 50  0001 C CNN
F 3 "" H 2850 800 50  0001 C CNN
	1    2850 800 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 1000 2850 800 
$Comp
L power:GND #PWR0102
U 1 1 5E0889DB
P 2050 2400
F 0 "#PWR0102" H 2050 2150 50  0001 C CNN
F 1 "GND" H 2055 2227 50  0000 C CNN
F 2 "" H 2050 2400 50  0001 C CNN
F 3 "" H 2050 2400 50  0001 C CNN
	1    2050 2400
	1    0    0    -1  
$EndComp
Text Label 1300 900  0    50   ~ 0
S88_DATA
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 5E089C70
P 3000 2300
F 0 "H1" H 3100 2349 50  0000 L CNN
F 1 "MountingHole_Pad" H 3100 2258 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5_Pad_Via" H 3000 2300 50  0001 C CNN
F 3 "~" H 3000 2300 50  0001 C CNN
	1    3000 2300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 5E08A66B
P 3300 2300
F 0 "H2" H 3400 2349 50  0000 L CNN
F 1 "MountingHole_Pad" H 3400 2258 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5_Pad_Via" H 3300 2300 50  0001 C CNN
F 3 "~" H 3300 2300 50  0001 C CNN
	1    3300 2300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 5E08A86E
P 3600 2300
F 0 "H3" H 3700 2349 50  0000 L CNN
F 1 "MountingHole_Pad" H 3700 2258 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5_Pad_Via" H 3600 2300 50  0001 C CNN
F 3 "~" H 3600 2300 50  0001 C CNN
	1    3600 2300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 5E08AA20
P 3900 2300
F 0 "H4" H 4000 2349 50  0000 L CNN
F 1 "MountingHole_Pad" H 4000 2258 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5_Pad_Via" H 3900 2300 50  0001 C CNN
F 3 "~" H 3900 2300 50  0001 C CNN
	1    3900 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 2400 3000 2400
Connection ~ 2850 2400
Connection ~ 3000 2400
Wire Wire Line
	3000 2400 3300 2400
Connection ~ 3300 2400
Wire Wire Line
	3300 2400 3600 2400
Connection ~ 3600 2400
Wire Wire Line
	3600 2400 3900 2400
$EndSCHEMATC
