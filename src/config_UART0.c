#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include <string.h>

void config_UART0(uint32_t baudRate) {
    if (baudRate == 0) baudRate = 9600;

    UART_CFG_T uart_cfg;

    uart_cfg.baudRate = baudRate;
    uart_cfg.dataBits = UART_DBITS_8;
    uart_cfg.parity = UART_PARITY_NONE;
    uart_cfg.stopBits = UART_STOPBIT_1;

    UART_Init(UART0, &uart_cfg);

    UART_PinConfig(UART_TX0_P0_2);
    UART_PinConfig(UART_RX0_P0_3);

    UART_TxEnable(UART0);
}

void UART0_SendString(char *str) {
    // UART_Send envía el bloque completo de memoria de forma bloqueante (BLOCKING)
    UART_Send(
        UART0,
        (uint8_t *)str,
        strlen(str),
        BLOCKING
        );
}