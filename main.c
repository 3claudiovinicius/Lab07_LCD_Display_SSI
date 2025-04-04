#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c1294ncpdt.h"
#include "st7789.h"
#include "st7789_font.h"

// Definições de pinos
#define BUTTON1_PIN GPIO_PIN_0
#define BUTTON2_PIN GPIO_PIN_1
#define LED_PINS_N (GPIO_PIN_0 | GPIO_PIN_1)  // PN1 e PN0
#define LED_PINS_F (GPIO_PIN_0 | GPIO_PIN_4)  // PF4 e PF0

// Prototipação de funções
void setup_GPIO(void);
void setup_SSI0(void);
void setup_UART(void);
void GPIOJ_Handler(void);
void UART_SendString(const char *str);
void update_leds(void);
void debounce_delay(void);

// Cores definidas (RGB)
const uint16_t colors[] = {
    ST7789_COLOR_RED,     // Vermelho
    ST7789_COLOR_GREEN,   // Verde
    ST7789_COLOR_BLUE,    // Azul
    ST7789_COLOR_WHITE,   // Branco
    ST7789_COLOR_BLACK    // Preto
};
const size_t color_count = sizeof(colors) / sizeof(colors[0]);

volatile int current_color_index = -1; // Índice da cor atual
volatile bool state = false;             // Estado: 0 = sem cor, 1 = exibindo cor

int main(void) {
    // Configuração do clock do sistema (120 MHz)
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    setup_GPIO();
    setup_SSI0();
    setup_UART();
    ST7789_Init();

    while (1) {
        // Loop principal
    }
}

void setup_GPIO(void) {
    // Habilitar GPIOs N, J, F e B (para o Reset do display)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // Adicionando GPIOB para o Reset

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)); // Aguardando GPIOB ficar pronto

    // Configuração dos LEDs (GPIO N e GPIO F)
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, LED_PINS_N);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_PINS_F);

    // Configuração dos botões (GPIO J)
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, BUTTON1_PIN | BUTTON2_PIN);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, BUTTON1_PIN | BUTTON2_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Configuração de interrupções
    GPIOIntDisable(GPIO_PORTJ_BASE, BUTTON1_PIN | BUTTON2_PIN);
    GPIOIntClear(GPIO_PORTJ_BASE, BUTTON1_PIN | BUTTON2_PIN);
    GPIOIntRegister(GPIO_PORTJ_BASE, GPIOJ_Handler);
    GPIOIntTypeSet(GPIO_PORTJ_BASE, BUTTON1_PIN | BUTTON2_PIN, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTJ_BASE, BUTTON1_PIN | BUTTON2_PIN);

    IntEnable(INT_GPIOJ);
    IntPrioritySet(INT_GPIOJ, 1);

    // Configuração do pino PB2 como saída para Reset do ST7789
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_2); // Inicializa em HIGH (desativado)
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0); // Inicializa PB3 (DC) em LOW
}

void setup_SSI0(void) {
    // Habilita o módulo SSI0 e GPIO A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    // Configuração dos pinos PA2 (CLK), PA3 (FSS), PA4 (Rx), PA5 (Tx)
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

    // Configuração do SSI0
    SSIConfigSetExpClk(SSI0_BASE, 120000000, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);
    SSIEnable(SSI0_BASE);
}

void setup_UART(void) {
    // Habilita a UART0 e GPIO A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configuração dos pinos PA0 (Rx) e PA1 (Tx)
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configuração da UART0 (115200 baud rate)
    UARTConfigSetExpClk(UART0_BASE, 120000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART0_BASE);
}

void GPIOJ_Handler(void) {
    debounce_delay();

    uint32_t status = GPIOIntStatus(GPIO_PORTJ_BASE, true);
    GPIOIntClear(GPIO_PORTJ_BASE, status);

    if (status & BUTTON1_PIN) {
        // Se o estado estiver desativado, ativamos a primeira cor
        if (!state) {
            state = true;  // Muda o estado para ativo
            current_color_index = 0; // Primeira cor ativada
        } else {
            // Caso contrário, incrementamos o índice
            current_color_index = (current_color_index + 1) % color_count;
        }
        // Atualize a tela com a nova cor
				ST7789_DrawCircle(110,160,100,ST7789_COLOR_MAGENTA);
        //ST7789_FillScreen(colors[current_color_index]);	 
							
        
        // Imprime o valor atual do índice de cor
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "Cor atualizada \r\n Current color index: %d\r\n", current_color_index);
        UART_SendString(buffer);
    }

    if (status & BUTTON2_PIN) {
        if (state) {
            current_color_index = (current_color_index - 1 + color_count) % color_count;
					  ST7789_DrawCircle(110,160,100,ST7789_COLOR_MAGENTA);
            //ST7789_FillScreen(colors[current_color_index]); 
            UART_SendString("Cor retrocedida\r\n");
        }
    }

    update_leds();
}




void UART_SendString(const char *str) {
    while (*str) {
        while (UARTBusy(UART0_BASE));
        UARTCharPut(UART0_BASE, *str++);
    }
}

void update_leds(void) {
    uint32_t leds_n = 0, leds_f = 0;

    switch (current_color_index) {
        case 0: leds_n = GPIO_PIN_1; break; // LED1
        case 1: leds_n = LED_PINS_N; break; // LED1 + LED2
        case 2: leds_n = LED_PINS_N; leds_f = GPIO_PIN_4; break; // LED1 + LED2 + LED3
        case 3: leds_n = LED_PINS_N; leds_f = LED_PINS_F; break; // LED1 + LED2 + LED3 + LED4
        default: break;
    }

    GPIOPinWrite(GPIO_PORTN_BASE, LED_PINS_N, leds_n);
    GPIOPinWrite(GPIO_PORTF_BASE, LED_PINS_F, leds_f);
}


void debounce_delay(void) {
    SysCtlDelay(400000); // Aproximadamente 10ms (com clock de 120MHz)
}

