2.4" LCD Pin description - GWMTF18430

|Convert Board  |Display  	|EK-TM4C1294XL
| 12   		| 1-LEDK      	| X8-4-GND           
| 11   		| 2-LEDA      	| X8-1-3V3           
| 10   		| 3-GND       	| X8-4-GND      
| 9   		| 4-SDO       	| NC         
| 8   		| 5-SDA       	| GPIO A-PA4           
| 7   		| 6-A0        	| GPIO B-PB3 
| 6   		| 7-SCL       	| GPIO A-PA2           
| 5   		| 8-CS        	| GPIO A-PA3           
| 4   		| 9-RST       	| GPIO B-PB2
| 3  		| 10-VCC       	| X8-1-3V3 
| 2  		| 11-VCC(ID)   	| X8-1-3V3 
| 1  		| 12-GND       	| X8-4-GND
   
Backlight
If = 80mA
Vf = 2.7~3.4V
VCC = 3.3V

Memory Map File
==============================================================================


      Code (inc. data)   RO Data    RW Data    ZI Data      Debug   

      9016        226       2024       1644        520      46601   Grand Totals
      9016        226       2024         24        520      46601   ELF Image Totals (compressed)
      9016        226       2024         24          0          0   ROM Totals

==============================================================================

    Total RO  Size (Code + RO Data)                11040 (  10.78kB)
    Total RW  Size (RW Data + ZI Data)              2164 (   2.11kB)
    Total ROM Size (Code + RO Data + RW Data)      11064 (  10.80kB)

==============================================================================

Imporvments

- Number variables: Controle type (8,16,32, signed, unsigned) and control memory location storage (constant,static), volatile, 