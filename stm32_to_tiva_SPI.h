#ifndef SPI_TIVA_H
#define SPI_TIVA_H

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"

typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

// Declaração das funções
void SPI_Transmit_Tiva(uint8_t *pTxData, uint16_t size, uint32_t timeout);
int SPI_Receive_Tiva(uint8_t *pRxData, uint16_t size, uint32_t timeout);
int SPI_TransmitReceive_Tiva(uint8_t *pTxData, uint8_t *pRxData, uint16_t size, uint32_t timeout);
void HAL_GPIO_WritePin(uint32_t GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

#endif
