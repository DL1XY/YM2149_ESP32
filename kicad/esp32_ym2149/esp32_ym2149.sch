EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "YM2149 ESP32"
Date ""
Rev ""
Comp "DL1XY"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ESP32_Dev:ESP32-DEVKITC-32D U?
U 1 1 5E7F7B07
P 3300 3050
F 0 "U?" H 3300 4217 50  0000 C CNN
F 1 "ESP32-DEVKITC-32D" H 3300 4126 50  0000 C CNN
F 2 "MODULE_ESP32-DEVKITC-32D" H 3300 3050 50  0001 L BNN
F 3 "4" H 3300 3050 50  0001 L BNN
F 4 "Espressif Systems" H 3300 3050 50  0001 L BNN "Feld4"
	1    3300 3050
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5E8056D1
P 7800 2900
F 0 "#PWR?" H 7800 2750 50  0001 C CNN
F 1 "+5V" H 7815 3073 50  0000 C CNN
F 2 "" H 7800 2900 50  0001 C CNN
F 3 "" H 7800 2900 50  0001 C CNN
	1    7800 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 1400 7150 1400
$Comp
L power:+5V #PWR?
U 1 1 5E80A1FD
P 7800 1400
F 0 "#PWR?" H 7800 1250 50  0001 C CNN
F 1 "+5V" H 7815 1573 50  0000 C CNN
F 2 "" H 7800 1400 50  0001 C CNN
F 3 "" H 7800 1400 50  0001 C CNN
	1    7800 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR?
U 1 1 5E80A976
P 5200 1400
F 0 "#PWR?" H 5200 1150 50  0001 C CNN
F 1 "GNDREF" H 5205 1227 50  0000 C CNN
F 2 "" H 5200 1400 50  0001 C CNN
F 3 "" H 5200 1400 50  0001 C CNN
	1    5200 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 1400 5450 1400
$Comp
L Device:C C?
U 1 1 5E80C2AC
P 7150 1250
F 0 "C?" H 7265 1296 50  0000 L CNN
F 1 "10nF" H 7265 1205 50  0000 L CNN
F 2 "" H 7188 1100 50  0001 C CNN
F 3 "~" H 7150 1250 50  0001 C CNN
	1    7150 1250
	1    0    0    -1  
$EndComp
Connection ~ 7150 1400
Wire Wire Line
	7150 1400 7800 1400
Wire Wire Line
	7150 1100 5200 1100
Wire Wire Line
	5200 1100 5200 1400
Connection ~ 5200 1400
$Comp
L power:GNDREF #PWR?
U 1 1 5E80FFBF
P 8100 3000
F 0 "#PWR?" H 8100 2750 50  0001 C CNN
F 1 "GNDREF" H 8105 2827 50  0000 C CNN
F 2 "" H 8100 3000 50  0001 C CNN
F 3 "" H 8100 3000 50  0001 C CNN
	1    8100 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5E811FCB
P 7950 3000
F 0 "C?" H 8065 3046 50  0000 L CNN
F 1 "10nF" H 8065 2955 50  0000 L CNN
F 2 "" H 7988 2850 50  0001 C CNN
F 3 "~" H 7950 3000 50  0001 C CNN
	1    7950 3000
	0    -1   -1   0   
$EndComp
Text GLabel 4100 3650 2    50   Output ~ 0
CLOCK
Text GLabel 7100 4100 2    50   Input ~ 0
CLOCK
Text GLabel 4100 2650 2    50   Output ~ 0
DA3
Text GLabel 7100 2300 2    50   Input ~ 0
DA3
Text GLabel 7100 2150 2    50   Input ~ 0
DA2
Text GLabel 4100 3150 2    50   Output ~ 0
DA2
Text GLabel 4100 3250 2    50   Output ~ 0
DA1
Text GLabel 7100 2000 2    50   Input ~ 0
DA1
Text GLabel 4100 3350 2    50   Output ~ 0
DA0
Text GLabel 7100 1850 2    50   Input ~ 0
DA0
Text GLabel 2500 2850 0    50   Output ~ 0
DA7
Text GLabel 2500 2950 0    50   Output ~ 0
DA6
Text GLabel 2500 3050 0    50   Output ~ 0
DA5
Text GLabel 2500 3150 0    50   Output ~ 0
DA4
Text GLabel 2500 3250 0    50   Output ~ 0
BDIR
Text GLabel 2500 3350 0    50   Output ~ 0
BC1
Text GLabel 7100 2450 2    50   Input ~ 0
DA4
Text GLabel 7100 2600 2    50   Input ~ 0
DA5
Text GLabel 7100 2750 2    50   Input ~ 0
DA6
Text GLabel 7100 2900 2    50   Input ~ 0
DA7
Text GLabel 7100 3050 2    50   Input ~ 0
BC1
Text GLabel 7100 3350 2    50   Input ~ 0
BDIR
Wire Wire Line
	7800 3500 7800 3200
$Comp
L DL1XY_AUDIOCHIPS:YM2149 U?
U 1 1 5E7F9386
P 6300 2800
F 0 "U?" H 6275 4539 50  0000 C CNN
F 1 "YM2149" H 6275 4410 100 0000 C CNN
F 2 "" H 6300 2150 50  0001 C CNN
F 3 "" H 6300 2150 50  0001 C CNN
	1    6300 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 3200 7800 3200
Wire Wire Line
	7100 3800 7800 3800
Wire Wire Line
	7800 2900 7800 3000
Connection ~ 7800 3200
Wire Wire Line
	7100 3500 7800 3500
Wire Wire Line
	7800 3800 7800 3650
Connection ~ 7800 3500
Wire Wire Line
	7100 3650 7800 3650
Connection ~ 7800 3650
Wire Wire Line
	7800 3650 7800 3500
Connection ~ 7800 3000
Wire Wire Line
	7800 3000 7800 3200
Text GLabel 5450 1700 0    50   Output ~ 0
AUDIO_CH_B
Text GLabel 5450 1850 0    50   Output ~ 0
AUDIO_CH_A
Text GLabel 7100 1700 2    50   Output ~ 0
AUDIO_CH_C
NoConn ~ 7100 1550
NoConn ~ 5450 1550
NoConn ~ 5450 2000
NoConn ~ 5450 2150
NoConn ~ 5450 2300
NoConn ~ 5450 2450
NoConn ~ 5450 2600
NoConn ~ 5450 2750
NoConn ~ 5450 2900
NoConn ~ 5450 3050
NoConn ~ 5450 3200
NoConn ~ 5450 3350
NoConn ~ 5450 3500
NoConn ~ 5450 3650
NoConn ~ 5450 3800
NoConn ~ 5450 3950
NoConn ~ 5450 4100
NoConn ~ 5450 4250
NoConn ~ 7100 4250
Text GLabel 4100 3550 2    50   Output ~ 0
RESET
Text GLabel 7100 3950 2    50   Input ~ 0
RESET
$EndSCHEMATC
