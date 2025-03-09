| **GPIO Port** | **Pins**  | **Function**               | **Description**                   
|--------------|----------|---------------------------|-----------------------------------
| **GPIO N**   | PN0, PN1 | LEDs                      | Controls LEDs on port N          
| **GPIO F**   | PF0, PF4 | LEDs                      | Controls LEDs on port F          
| **GPIO J**   | PJ0, PJ1 | Buttons                   | Reads input buttons              
| **GPIO A**   | PA2      | SPI (SSI0 CLK)            | SPI bus clock                    
| **GPIO A**   | PA3      | SPI (SSI0 FSS)            | Frame Select (Chip Select)       
| **GPIO A**   | PA4      | SPI (SSI0 XDAT0 - MISO)   | SPI data input (MISO not used)                   
| **GPIO A**   | PA5      | SPI (SSI0 XDAT1 - MOSI)   | SPI data output                  
| **GPIO A**   | PA0      | UART0 RX                  | UART data reception              
| **GPIO A**   | PA1      | UART0 TX                  | UART data transmission           
| **GPIO B**   | PB2      | RST                       | ST7789 RST Pin           
| **GPIO B**   | PB3      | DC                        | ST7789 DC Pin           
-----------------------------------------------------------------------------------------------------------------------------------------------------

1. System Initialization:
The microcontroller's clock is configured to 120 MHz.
The GPIO, SSI (SPI), and UART peripherals are enabled and configured.
The LED and button pins are configured as outputs and inputs, respectively.
Interrupts are configured and enabled on the button pins to detect state changes (presses).
The ST7789 display is initialized.

2. Main Loop Execution:
The system enters an infinite loop, where it waits for interrupt events triggered by the buttons.

3. Button Interrupt Handling:

3.1 Upon pressing Button 1:
The system checks if it is on or off. If off, it turns on and displays the first color. If already on, it advances to the next color in the list.
The selected color is sent to the ST7789 display via SPI.
A "Color updated" message is sent via UART for debugging purposes.

3.2 Upon pressing Button 2:
If the system is on, the displayed color reverses to the previous color in the list.
The updated color is sent to the ST7789 display via SPI.
A "Color reversed" message is sent via UART.
The Leds are updated based on the color selected.
A debounce routine is executed to prevent multiple button activations.

4. Communication via SPI and UART:
The SPI protocol is used to transmit color data to the ST7789 display.
The UART interface is used to send status and debugging messages to an external device.

5. Button Debouncing:
A debounce delay is implemented to ensure accurate detection of a single button press, preventing multiple and unwanted readings.
----------------------------------------------------------------------------------------------------------------------------------------------------------

2.4" LCD Pin description - GWMTF18430

|Convert Board  |Display  	|EK-TM4C1294XL
| 12   		| 1-LEDK      	| X8-4-GND           
| 11   		| 2-LEDA      	| X8-1-3V3           
| 10   		| 3-GND       	| X8-4-GND      
| 9   		| 4-SDO       	| NC         
| 8   		| 5-SDA       	| GPIO A-PA5           
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
