#ifndef TP_UART0_H
#define TP_UART0_H
#include "LPC17xx.h"

/**
 *
 * @param baudRate Baudios al que se configura el módulo, si se deja en blanco, se configurará a 9600.
 * Valores posibles: 2400, 4800, 9600, 19200, 38400, 57600, 115200
 */
void UART0_config(uint32_t baudRate);

/**
 * @brief Envía una cadena formateada por el puerto UART0 (Estilo printf).
 * @param format Cadena de texto con especificadores de formato (ej. %d, %c, %s)
 * @param ... Argumentos variables según el formato especificado
 */
void UART_enviar(const char* format, ...);


void enviarDatosSimulación();

#endif
