EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 1900 1850 1    50   Input ~ 0
V33
Text GLabel 1500 3300 0    50   Input ~ 0
SDA_2
$Comp
L Connector:Conn_Coaxial J?
U 1 1 6150BF31
P 4350 2200
AR Path="/614F5619/6150BF31" Ref="J?"  Part="1" 
AR Path="/61506CC0/6150BF31" Ref="J11"  Part="1" 
F 0 "J11" H 4450 2175 50  0000 L CNN
F 1 "Link-out 1-0" H 4450 2084 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132134_Vertical" H 4350 2200 50  0001 C CNN
F 3 " ~" H 4350 2200 50  0001 C CNN
	1    4350 2200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6150BF37
P 4600 3400
AR Path="/614F5619/6150BF37" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6150BF37" Ref="#PWR0103"  Part="1" 
F 0 "#PWR0103" H 4600 3150 50  0001 C CNN
F 1 "GND" H 4605 3227 50  0000 C CNN
F 2 "" H 4600 3400 50  0001 C CNN
F 3 "" H 4600 3400 50  0001 C CNN
	1    4600 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6150BF3D
P 4350 2400
AR Path="/614F5619/6150BF3D" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6150BF3D" Ref="#PWR0104"  Part="1" 
F 0 "#PWR0104" H 4350 2150 50  0001 C CNN
F 1 "GND" H 4200 2350 50  0000 C CNN
F 2 "" H 4350 2400 50  0001 C CNN
F 3 "" H 4350 2400 50  0001 C CNN
	1    4350 2400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_Coaxial J?
U 1 1 6150BF43
P 4600 3200
AR Path="/614F5619/6150BF43" Ref="J?"  Part="1" 
AR Path="/61506CC0/6150BF43" Ref="J13"  Part="1" 
F 0 "J13" H 4700 3175 50  0000 L CNN
F 1 "Link-out 1-1" H 4700 3084 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132134_Vertical" H 4600 3200 50  0001 C CNN
F 3 " ~" H 4600 3200 50  0001 C CNN
	1    4600 3200
	1    0    0    -1  
$EndComp
$Comp
L Oscillator:Si5351A-B-GT U?
U 1 1 6150BF49
P 2000 3000
AR Path="/614F5619/6150BF49" Ref="U?"  Part="1" 
AR Path="/61506CC0/6150BF49" Ref="U6"  Part="1" 
F 0 "U6" H 1600 3700 50  0000 C CNN
F 1 "Si5351A-B-GT" H 2000 3700 50  0000 C CNN
F 2 "Package_SO:MSOP-10_3x3mm_P0.5mm" H 2000 2200 50  0001 C CNN
F 3 "https://www.silabs.com/documents/public/data-sheets/Si5351-B.pdf" H 1650 2900 50  0001 C CNN
	1    2000 3000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6150BF4F
P 2000 3500
AR Path="/614F5619/6150BF4F" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6150BF4F" Ref="#PWR0105"  Part="1" 
F 0 "#PWR0105" H 2000 3250 50  0001 C CNN
F 1 "GND" H 2005 3327 50  0000 C CNN
F 2 "" H 2000 3500 50  0001 C CNN
F 3 "" H 2000 3500 50  0001 C CNN
	1    2000 3500
	1    0    0    -1  
$EndComp
Text GLabel 1500 3200 0    50   Input ~ 0
SCL_2
$Comp
L Device:Crystal_GND2_Small Y?
U 1 1 6150BF56
P 1150 2800
AR Path="/614F5619/6150BF56" Ref="Y?"  Part="1" 
AR Path="/61506CC0/6150BF56" Ref="Y1"  Part="1" 
F 0 "Y1" V 1196 2669 50  0000 R CNN
F 1 "Crystal" V 1105 2669 50  0000 R CNN
F 2 "Crystal:Crystal_SMD_TXC_7M-4Pin_3.2x2.5mm_HandSoldering" H 1150 2800 50  0001 C CNN
F 3 "~" H 1150 2800 50  0001 C CNN
	1    1150 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	1150 2500 1500 2500
Wire Wire Line
	1500 2500 1500 2700
Wire Wire Line
	1150 3050 1500 3050
Wire Wire Line
	1500 3050 1500 2900
$Comp
L Device:C C?
U 1 1 6150BF60
P 1750 1850
AR Path="/614F5619/6150BF60" Ref="C?"  Part="1" 
AR Path="/61506CC0/6150BF60" Ref="C26"  Part="1" 
F 0 "C26" H 1635 1804 50  0000 R CNN
F 1 "100n" H 1635 1895 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1788 1700 50  0001 C CNN
F 3 "~" H 1750 1850 50  0001 C CNN
	1    1750 1850
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6150BF66
P 1600 1850
AR Path="/614F5619/6150BF66" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6150BF66" Ref="#PWR0106"  Part="1" 
F 0 "#PWR0106" H 1600 1600 50  0001 C CNN
F 1 "GND" H 1605 1677 50  0000 C CNN
F 2 "" H 1600 1850 50  0001 C CNN
F 3 "" H 1600 1850 50  0001 C CNN
	1    1600 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 1850 1900 2100
Wire Wire Line
	1900 2100 2100 2100
Wire Wire Line
	2100 2100 2100 2500
Connection ~ 1900 2100
Wire Wire Line
	1900 2100 1900 2500
$Comp
L Device:R R?
U 1 1 6150BF7E
P 2650 2800
AR Path="/614F5619/6150BF7E" Ref="R?"  Part="1" 
AR Path="/61506CC0/6150BF7E" Ref="R39"  Part="1" 
F 0 "R39" V 2500 2950 50  0000 C CNN
F 1 "1k" V 2534 2800 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2580 2800 50  0001 C CNN
F 3 "~" H 2650 2800 50  0001 C CNN
	1    2650 2800
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT135:BFR92 Q?
U 1 1 6150BF84
P 3100 2000
AR Path="/614F5619/6150BF84" Ref="Q?"  Part="1" 
AR Path="/61506CC0/6150BF84" Ref="Q3"  Part="1" 
F 0 "Q3" H 3290 2046 50  0000 L CNN
F 1 "BFR92" H 3290 1955 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3300 1925 50  0001 L CIN
F 3 "" H 3100 2000 50  0001 L CNN
	1    3100 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 2000 2900 2800
$Comp
L Transistor_BJT135:BFR92 Q?
U 1 1 6150BF8B
P 3350 3000
AR Path="/614F5619/6150BF8B" Ref="Q?"  Part="1" 
AR Path="/61506CC0/6150BF8B" Ref="Q5"  Part="1" 
F 0 "Q5" H 3540 3046 50  0000 L CNN
F 1 "BFR92" H 3540 2955 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3550 2925 50  0001 L CIN
F 3 "" H 3350 3000 50  0001 L CNN
	1    3350 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 3000 3150 3000
$Comp
L Device:R R?
U 1 1 6150BF98
P 3400 2200
AR Path="/614F5619/6150BF98" Ref="R?"  Part="1" 
AR Path="/61506CC0/6150BF98" Ref="R35"  Part="1" 
F 0 "R35" H 3330 2154 50  0000 R CNN
F 1 "200" H 3330 2245 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3330 2200 50  0001 C CNN
F 3 "~" H 3400 2200 50  0001 C CNN
	1    3400 2200
	0    -1   -1   0   
$EndComp
Text GLabel 3200 1800 1    50   Input ~ 0
V33
Text GLabel 3450 2800 1    50   Input ~ 0
V33
$Comp
L Device:R R?
U 1 1 6150BFA1
P 3700 3200
AR Path="/614F5619/6150BFA1" Ref="R?"  Part="1" 
AR Path="/61506CC0/6150BFA1" Ref="R43"  Part="1" 
F 0 "R43" H 3630 3154 50  0000 R CNN
F 1 "200" H 3630 3245 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3630 3200 50  0001 C CNN
F 3 "~" H 3700 3200 50  0001 C CNN
	1    3700 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2900 2800 2800 2800
$Comp
L Device:R R?
U 1 1 6150BFB0
P 2650 3000
AR Path="/614F5619/6150BFB0" Ref="R?"  Part="1" 
AR Path="/61506CC0/6150BFB0" Ref="R41"  Part="1" 
F 0 "R41" V 2600 3200 50  0000 C CNN
F 1 "1k" V 2534 3000 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2580 3000 50  0001 C CNN
F 3 "~" H 2650 3000 50  0001 C CNN
	1    2650 3000
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 6150BFBC
P 2050 1850
AR Path="/614F5619/6150BFBC" Ref="C?"  Part="1" 
AR Path="/61506CC0/6150BFBC" Ref="C27"  Part="1" 
F 0 "C27" H 1935 1804 50  0000 R CNN
F 1 "100n" H 1935 1895 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2088 1700 50  0001 C CNN
F 3 "~" H 2050 1850 50  0001 C CNN
	1    2050 1850
	0    -1   -1   0   
$EndComp
Connection ~ 1900 1850
$Comp
L power:GND #PWR?
U 1 1 6150BFC3
P 2200 1850
AR Path="/614F5619/6150BFC3" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6150BFC3" Ref="#PWR0107"  Part="1" 
F 0 "#PWR0107" H 2200 1600 50  0001 C CNN
F 1 "GND" H 2205 1677 50  0000 C CNN
F 2 "" H 2200 1850 50  0001 C CNN
F 3 "" H 2200 1850 50  0001 C CNN
	1    2200 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 2500 1150 2700
Wire Wire Line
	1150 2900 1150 3050
$Comp
L power:GND #PWR?
U 1 1 6150BFCB
P 1050 2800
AR Path="/614F5619/6150BFCB" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6150BFCB" Ref="#PWR0108"  Part="1" 
F 0 "#PWR0108" H 1050 2550 50  0001 C CNN
F 1 "GND" H 1055 2627 50  0000 C CNN
F 2 "" H 1050 2800 50  0001 C CNN
F 3 "" H 1050 2800 50  0001 C CNN
	1    1050 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 6150BFD1
P 3600 2350
AR Path="/614F5619/6150BFD1" Ref="R?"  Part="1" 
AR Path="/61506CC0/6150BFD1" Ref="R37"  Part="1" 
F 0 "R37" H 3530 2304 50  0000 R CNN
F 1 "68" H 3530 2395 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3530 2350 50  0001 C CNN
F 3 "~" H 3600 2350 50  0001 C CNN
	1    3600 2350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6150BFD7
P 3600 2500
AR Path="/614F5619/6150BFD7" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6150BFD7" Ref="#PWR0109"  Part="1" 
F 0 "#PWR0109" H 3600 2250 50  0001 C CNN
F 1 "GND" H 3450 2450 50  0000 C CNN
F 2 "" H 3600 2500 50  0001 C CNN
F 3 "" H 3600 2500 50  0001 C CNN
	1    3600 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 2200 4150 2200
$Comp
L Device:R R?
U 1 1 6150BFDE
P 4000 3350
AR Path="/614F5619/6150BFDE" Ref="R?"  Part="1" 
AR Path="/61506CC0/6150BFDE" Ref="R45"  Part="1" 
F 0 "R45" H 3930 3304 50  0000 R CNN
F 1 "68" H 3930 3395 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3930 3350 50  0001 C CNN
F 3 "~" H 4000 3350 50  0001 C CNN
	1    4000 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 3200 4000 3200
Wire Wire Line
	4300 3200 4400 3200
Wire Wire Line
	3450 3200 3550 3200
Wire Wire Line
	3200 2200 3250 2200
Wire Wire Line
	3550 2200 3600 2200
$Comp
L Device:C C?
U 1 1 6150BFFD
P 3850 2200
AR Path="/614F5619/6150BFFD" Ref="C?"  Part="1" 
AR Path="/61506CC0/6150BFFD" Ref="C30"  Part="1" 
F 0 "C30" H 3735 2154 50  0000 R CNN
F 1 "100n" H 3735 2245 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3888 2050 50  0001 C CNN
F 3 "~" H 3850 2200 50  0001 C CNN
	1    3850 2200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3600 2200 3700 2200
Connection ~ 3600 2200
$Comp
L Device:C C?
U 1 1 6150C005
P 4150 3200
AR Path="/614F5619/6150C005" Ref="C?"  Part="1" 
AR Path="/61506CC0/6150C005" Ref="C32"  Part="1" 
F 0 "C32" H 4035 3154 50  0000 R CNN
F 1 "100n" H 4035 3245 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4188 3050 50  0001 C CNN
F 3 "~" H 4150 3200 50  0001 C CNN
	1    4150 3200
	0    -1   -1   0   
$EndComp
Connection ~ 4000 3200
Text GLabel 6450 1850 1    50   Input ~ 0
V33
Text GLabel 6050 3300 0    50   Input ~ 0
SDA_1
$Comp
L Connector:Conn_Coaxial J?
U 1 1 61515132
P 8900 2200
AR Path="/614F5619/61515132" Ref="J?"  Part="1" 
AR Path="/61506CC0/61515132" Ref="J12"  Part="1" 
F 0 "J12" H 9000 2175 50  0000 L CNN
F 1 "Link-out 1-0" H 9000 2084 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132134_Vertical" H 8900 2200 50  0001 C CNN
F 3 " ~" H 8900 2200 50  0001 C CNN
	1    8900 2200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6151513C
P 9150 3400
AR Path="/614F5619/6151513C" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6151513C" Ref="#PWR0110"  Part="1" 
F 0 "#PWR0110" H 9150 3150 50  0001 C CNN
F 1 "GND" H 9155 3227 50  0000 C CNN
F 2 "" H 9150 3400 50  0001 C CNN
F 3 "" H 9150 3400 50  0001 C CNN
	1    9150 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 61515146
P 8900 2400
AR Path="/614F5619/61515146" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/61515146" Ref="#PWR0111"  Part="1" 
F 0 "#PWR0111" H 8900 2150 50  0001 C CNN
F 1 "GND" H 8750 2350 50  0000 C CNN
F 2 "" H 8900 2400 50  0001 C CNN
F 3 "" H 8900 2400 50  0001 C CNN
	1    8900 2400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_Coaxial J?
U 1 1 61515150
P 9150 3200
AR Path="/614F5619/61515150" Ref="J?"  Part="1" 
AR Path="/61506CC0/61515150" Ref="J14"  Part="1" 
F 0 "J14" H 9250 3175 50  0000 L CNN
F 1 "Link-out 1-1" H 9250 3084 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132134_Vertical" H 9150 3200 50  0001 C CNN
F 3 " ~" H 9150 3200 50  0001 C CNN
	1    9150 3200
	1    0    0    -1  
$EndComp
$Comp
L Oscillator:Si5351A-B-GT U?
U 1 1 6151515A
P 6550 3000
AR Path="/614F5619/6151515A" Ref="U?"  Part="1" 
AR Path="/61506CC0/6151515A" Ref="U7"  Part="1" 
F 0 "U7" H 6150 3700 50  0000 C CNN
F 1 "Si5351A-B-GT" H 6550 3700 50  0000 C CNN
F 2 "Package_SO:MSOP-10_3x3mm_P0.5mm" H 6550 2200 50  0001 C CNN
F 3 "https://www.silabs.com/documents/public/data-sheets/Si5351-B.pdf" H 6200 2900 50  0001 C CNN
	1    6550 3000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 61515164
P 6550 3500
AR Path="/614F5619/61515164" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/61515164" Ref="#PWR0112"  Part="1" 
F 0 "#PWR0112" H 6550 3250 50  0001 C CNN
F 1 "GND" H 6555 3327 50  0000 C CNN
F 2 "" H 6550 3500 50  0001 C CNN
F 3 "" H 6550 3500 50  0001 C CNN
	1    6550 3500
	1    0    0    -1  
$EndComp
Text GLabel 6050 3200 0    50   Input ~ 0
SCL_1
$Comp
L Device:Crystal_GND2_Small Y?
U 1 1 6151516F
P 5700 2800
AR Path="/614F5619/6151516F" Ref="Y?"  Part="1" 
AR Path="/61506CC0/6151516F" Ref="Y2"  Part="1" 
F 0 "Y2" V 5746 2669 50  0000 R CNN
F 1 "Crystal" V 5655 2669 50  0000 R CNN
F 2 "Crystal:Crystal_SMD_TXC_7M-4Pin_3.2x2.5mm_HandSoldering" H 5700 2800 50  0001 C CNN
F 3 "~" H 5700 2800 50  0001 C CNN
	1    5700 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	5700 2500 6050 2500
Wire Wire Line
	6050 2500 6050 2700
Wire Wire Line
	5700 3050 6050 3050
Wire Wire Line
	6050 3050 6050 2900
$Comp
L Device:C C?
U 1 1 6151517D
P 6300 1850
AR Path="/614F5619/6151517D" Ref="C?"  Part="1" 
AR Path="/61506CC0/6151517D" Ref="C28"  Part="1" 
F 0 "C28" H 6185 1804 50  0000 R CNN
F 1 "100n" H 6185 1895 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6338 1700 50  0001 C CNN
F 3 "~" H 6300 1850 50  0001 C CNN
	1    6300 1850
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 61515187
P 6150 1850
AR Path="/614F5619/61515187" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/61515187" Ref="#PWR0113"  Part="1" 
F 0 "#PWR0113" H 6150 1600 50  0001 C CNN
F 1 "GND" H 6155 1677 50  0000 C CNN
F 2 "" H 6150 1850 50  0001 C CNN
F 3 "" H 6150 1850 50  0001 C CNN
	1    6150 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 1850 6450 2100
Wire Wire Line
	6450 2100 6650 2100
Wire Wire Line
	6650 2100 6650 2500
Connection ~ 6450 2100
Wire Wire Line
	6450 2100 6450 2500
$Comp
L Device:R R?
U 1 1 61515196
P 7200 2800
AR Path="/614F5619/61515196" Ref="R?"  Part="1" 
AR Path="/61506CC0/61515196" Ref="R40"  Part="1" 
F 0 "R40" V 7050 2950 50  0000 C CNN
F 1 "1k" V 7084 2800 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7130 2800 50  0001 C CNN
F 3 "~" H 7200 2800 50  0001 C CNN
	1    7200 2800
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT135:BFR92 Q?
U 1 1 615151A0
P 7650 2000
AR Path="/614F5619/615151A0" Ref="Q?"  Part="1" 
AR Path="/61506CC0/615151A0" Ref="Q4"  Part="1" 
F 0 "Q4" H 7840 2046 50  0000 L CNN
F 1 "BFR92" H 7840 1955 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 7850 1925 50  0001 L CIN
F 3 "" H 7650 2000 50  0001 L CNN
	1    7650 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 2000 7450 2800
$Comp
L Transistor_BJT135:BFR92 Q?
U 1 1 615151AB
P 7900 3000
AR Path="/614F5619/615151AB" Ref="Q?"  Part="1" 
AR Path="/61506CC0/615151AB" Ref="Q6"  Part="1" 
F 0 "Q6" H 8090 3046 50  0000 L CNN
F 1 "BFR92" H 8090 2955 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8100 2925 50  0001 L CIN
F 3 "" H 7900 3000 50  0001 L CNN
	1    7900 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 3000 7700 3000
$Comp
L Device:R R?
U 1 1 615151B6
P 7950 2200
AR Path="/614F5619/615151B6" Ref="R?"  Part="1" 
AR Path="/61506CC0/615151B6" Ref="R36"  Part="1" 
F 0 "R36" H 7880 2154 50  0000 R CNN
F 1 "200" H 7880 2245 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7880 2200 50  0001 C CNN
F 3 "~" H 7950 2200 50  0001 C CNN
	1    7950 2200
	0    -1   -1   0   
$EndComp
Text GLabel 7750 1800 1    50   Input ~ 0
V33
Text GLabel 8000 2800 1    50   Input ~ 0
V33
$Comp
L Device:R R?
U 1 1 615151C2
P 8250 3200
AR Path="/614F5619/615151C2" Ref="R?"  Part="1" 
AR Path="/61506CC0/615151C2" Ref="R44"  Part="1" 
F 0 "R44" H 8180 3154 50  0000 R CNN
F 1 "200" H 8180 3245 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8180 3200 50  0001 C CNN
F 3 "~" H 8250 3200 50  0001 C CNN
	1    8250 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7450 2800 7350 2800
$Comp
L Device:R R?
U 1 1 615151CD
P 7200 3000
AR Path="/614F5619/615151CD" Ref="R?"  Part="1" 
AR Path="/61506CC0/615151CD" Ref="R42"  Part="1" 
F 0 "R42" V 7150 3200 50  0000 C CNN
F 1 "1k" V 7084 3000 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7130 3000 50  0001 C CNN
F 3 "~" H 7200 3000 50  0001 C CNN
	1    7200 3000
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 615151D7
P 6600 1850
AR Path="/614F5619/615151D7" Ref="C?"  Part="1" 
AR Path="/61506CC0/615151D7" Ref="C29"  Part="1" 
F 0 "C29" H 6485 1804 50  0000 R CNN
F 1 "100n" H 6485 1895 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6638 1700 50  0001 C CNN
F 3 "~" H 6600 1850 50  0001 C CNN
	1    6600 1850
	0    -1   -1   0   
$EndComp
Connection ~ 6450 1850
$Comp
L power:GND #PWR?
U 1 1 615151E2
P 6750 1850
AR Path="/614F5619/615151E2" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/615151E2" Ref="#PWR0114"  Part="1" 
F 0 "#PWR0114" H 6750 1600 50  0001 C CNN
F 1 "GND" H 6755 1677 50  0000 C CNN
F 2 "" H 6750 1850 50  0001 C CNN
F 3 "" H 6750 1850 50  0001 C CNN
	1    6750 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 2500 5700 2700
Wire Wire Line
	5700 2900 5700 3050
$Comp
L power:GND #PWR?
U 1 1 615151EE
P 5600 2800
AR Path="/614F5619/615151EE" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/615151EE" Ref="#PWR0115"  Part="1" 
F 0 "#PWR0115" H 5600 2550 50  0001 C CNN
F 1 "GND" H 5605 2627 50  0000 C CNN
F 2 "" H 5600 2800 50  0001 C CNN
F 3 "" H 5600 2800 50  0001 C CNN
	1    5600 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 615151F8
P 8150 2350
AR Path="/614F5619/615151F8" Ref="R?"  Part="1" 
AR Path="/61506CC0/615151F8" Ref="R38"  Part="1" 
F 0 "R38" H 8080 2304 50  0000 R CNN
F 1 "68" H 8080 2395 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8080 2350 50  0001 C CNN
F 3 "~" H 8150 2350 50  0001 C CNN
	1    8150 2350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 61515202
P 8150 2500
AR Path="/614F5619/61515202" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/61515202" Ref="#PWR0116"  Part="1" 
F 0 "#PWR0116" H 8150 2250 50  0001 C CNN
F 1 "GND" H 8000 2450 50  0000 C CNN
F 2 "" H 8150 2500 50  0001 C CNN
F 3 "" H 8150 2500 50  0001 C CNN
	1    8150 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2200 8700 2200
$Comp
L Device:R R?
U 1 1 6151520D
P 8550 3350
AR Path="/614F5619/6151520D" Ref="R?"  Part="1" 
AR Path="/61506CC0/6151520D" Ref="R46"  Part="1" 
F 0 "R46" H 8480 3304 50  0000 R CNN
F 1 "68" H 8480 3395 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8480 3350 50  0001 C CNN
F 3 "~" H 8550 3350 50  0001 C CNN
	1    8550 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 3200 8550 3200
Wire Wire Line
	8850 3200 8950 3200
Wire Wire Line
	8000 3200 8100 3200
Wire Wire Line
	7750 2200 7800 2200
Wire Wire Line
	8100 2200 8150 2200
$Comp
L Device:C C?
U 1 1 6151521C
P 8400 2200
AR Path="/614F5619/6151521C" Ref="C?"  Part="1" 
AR Path="/61506CC0/6151521C" Ref="C31"  Part="1" 
F 0 "C31" H 8285 2154 50  0000 R CNN
F 1 "100n" H 8285 2245 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8438 2050 50  0001 C CNN
F 3 "~" H 8400 2200 50  0001 C CNN
	1    8400 2200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8150 2200 8250 2200
Connection ~ 8150 2200
$Comp
L Device:C C?
U 1 1 61515228
P 8700 3200
AR Path="/614F5619/61515228" Ref="C?"  Part="1" 
AR Path="/61506CC0/61515228" Ref="C33"  Part="1" 
F 0 "C33" H 8585 3154 50  0000 R CNN
F 1 "100n" H 8585 3245 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8738 3050 50  0001 C CNN
F 3 "~" H 8700 3200 50  0001 C CNN
	1    8700 3200
	0    -1   -1   0   
$EndComp
Connection ~ 8550 3200
$Comp
L power:GND #PWR?
U 1 1 6161FD3A
P 4000 3500
AR Path="/614F5619/6161FD3A" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/6161FD3A" Ref="#PWR0121"  Part="1" 
F 0 "#PWR0121" H 4000 3250 50  0001 C CNN
F 1 "GND" H 3850 3450 50  0000 C CNN
F 2 "" H 4000 3500 50  0001 C CNN
F 3 "" H 4000 3500 50  0001 C CNN
	1    4000 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 616226F1
P 8550 3500
AR Path="/614F5619/616226F1" Ref="#PWR?"  Part="1" 
AR Path="/61506CC0/616226F1" Ref="#PWR0122"  Part="1" 
F 0 "#PWR0122" H 8550 3250 50  0001 C CNN
F 1 "GND" H 8555 3327 50  0000 C CNN
F 2 "" H 8550 3500 50  0001 C CNN
F 3 "" H 8550 3500 50  0001 C CNN
	1    8550 3500
	1    0    0    -1  
$EndComp
$EndSCHEMATC
