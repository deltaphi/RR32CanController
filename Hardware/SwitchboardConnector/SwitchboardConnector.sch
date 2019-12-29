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
Wire Wire Line
	1250 1000 1250 700 
Wire Wire Line
	1250 700  1750 700 
Wire Wire Line
	1750 700  1750 1000
Wire Wire Line
	1250 1100 1150 1100
Wire Wire Line
	1150 1100 1150 600 
Wire Wire Line
	1150 600  1850 600 
Wire Wire Line
	1850 600  1850 1100
Wire Wire Line
	1850 1100 1750 1100
Wire Wire Line
	1250 1200 1050 1200
Wire Wire Line
	1050 1200 1050 500 
Wire Wire Line
	1050 500  1950 500 
Wire Wire Line
	1950 500  1950 1200
Wire Wire Line
	1950 1200 1750 1200
Wire Wire Line
	1250 1500 1250 1750
Wire Wire Line
	1250 1750 1750 1750
Wire Wire Line
	1750 1750 1750 1500
Wire Wire Line
	1250 1400 1150 1400
Wire Wire Line
	1150 1400 1150 1850
Wire Wire Line
	1150 1850 1850 1850
Wire Wire Line
	1850 1850 1850 1400
Wire Wire Line
	1850 1400 1750 1400
Wire Wire Line
	1250 1300 1050 1300
Wire Wire Line
	1050 1300 1050 1950
Wire Wire Line
	1050 1950 1950 1950
Wire Wire Line
	1950 1950 1950 1300
Wire Wire Line
	1950 1300 1750 1300
$Comp
L Connector_Generic:Conn_02x06_Odd_Even J1
U 1 1 5E08DB1D
P 1450 1200
F 0 "J1" H 1500 1617 50  0000 C CNN
F 1 "Conn_02x06_Odd_Even" H 1500 1526 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x06_P2.54mm_Vertical" H 1450 1200 50  0001 C CNN
F 3 "~" H 1450 1200 50  0001 C CNN
	1    1450 1200
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J2
U 1 1 5E08914C
P 2300 1200
F 0 "J2" H 2328 1176 50  0000 L CNN
F 1 "Conn_01x06_Female" H 2328 1085 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Vertical" H 2300 1200 50  0001 C CNN
F 3 "~" H 2300 1200 50  0001 C CNN
	1    2300 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 1000 2100 1000
Connection ~ 1750 1000
Wire Wire Line
	1850 1100 2100 1100
Connection ~ 1850 1100
Wire Wire Line
	1950 1200 2100 1200
Connection ~ 1950 1200
Wire Wire Line
	1950 1300 2100 1300
Connection ~ 1950 1300
Wire Wire Line
	1850 1400 2100 1400
Connection ~ 1850 1400
Wire Wire Line
	1750 1500 2100 1500
Connection ~ 1750 1500
$EndSCHEMATC
