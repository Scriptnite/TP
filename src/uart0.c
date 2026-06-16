#include <stdarg.h>
#include <stdio.h>

#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include <string.h>

#define TX_BUFFER_SIZE 256

// Le indicamos al compilador que esta función ya existe en tu proyecto.
// Asegúrate de que el nombre coincida exactamente con tu función original.
extern void UART0_SendString(const char* str);
void UART0_SendString_Variables(char* str);


void UART0_config(uint32_t baudRate) {
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

void UART_enviar(const char* format, ...) {
    // Al NO usar DMA, el buffer puede ser local (se aloja en el Stack)
    // porque la función UART0_SendString esperará a que se envíe todo.
    char buffer[TX_BUFFER_SIZE];

    va_list args;

    // Inicializa la lista de argumentos variables
    va_start(args, format);

    // vsnprintf toma el formato y los argumentos variables (...),
    // los procesa y los guarda de manera segura dentro de 'buffer'
    // sin pasarse del tamaño límite (sizeof(buffer)).
    const int longitud = vsnprintf(buffer, sizeof(buffer), format, args);

    // Limpia la memoria de la lista de argumentos
    va_end(args);

    // Si el formateo tuvo éxito y generó texto, lo enviamos
    if (longitud > 0) {
        UART0_SendString_Variables(buffer);
    }
}


void UART0_SendString_Variables(char* str) {
    UART_Send(
        UART0,
        (uint8_t *)str,
        strlen(str),
        BLOCKING
    );
}

