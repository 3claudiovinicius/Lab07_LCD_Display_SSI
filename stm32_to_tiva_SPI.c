#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "stm32_to_tiva_SPI.h"  // Inclui o cabeçalho correspondente

void SPI_Transmit_Tiva(uint8_t *pTxData, uint16_t size, uint32_t timeout) {
    uint32_t i;
    for (i = 0; i < size; i++) {
        SSIDataPut(SSI0_BASE, pTxData[i]); // Envia um byte
        while (SSIBusy(SSI0_BASE)); // Aguarda a transmissão concluir
    }
}

int SPI_Receive_Tiva(uint8_t *pRxData, uint16_t size, uint32_t timeout) {
    uint32_t data;
    uint16_t i = 0;
    uint32_t start_time = SysCtlClockGet() / 1000 * timeout;

    while (i < size) {
        if (SSIDataGetNonBlocking(SSI0_BASE, &data)) {
            pRxData[i++] = (uint8_t)data;
        }
        if (start_time-- == 0) {
            return -1;
        }
    }
    return 0;
}

int SPI_TransmitReceive_Tiva(uint8_t *pTxData, uint8_t *pRxData, uint16_t size, uint32_t timeout) {
    uint32_t data;
    uint16_t i = 0;
    uint32_t start_time = SysCtlClockGet() / 1000 * timeout;

    while (i < size) {
        SSIDataPut(SSI0_BASE, pTxData[i]);
        while (SSIBusy(SSI0_BASE));

        if (SSIDataGetNonBlocking(SSI0_BASE, &data)) {
            pRxData[i] = (uint8_t)data;
            i++;
        }

        if (start_time-- == 0) {
            return -1;
        }
    }
    return 0;
}

void HAL_GPIO_WritePin(uint32_t GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    if (PinState == GPIO_HIGH_LEVEL) {
        uint32_t mask = (1<<GPIO_Pin);
			  GPIOx |= mask;
    } else {
        // Define o pino como LOW
        GPIOPinWrite(GPIOx, GPIO_Pin, 0x00);
    }
}
