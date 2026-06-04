#ifndef TP_UART0_H
#define TP_UART0_H

/**
 *
 * @param baudRate Baudios al que se configura el módulo, si se deja en blanco, se configurará a 9600.
 * Valores posibles: 2400, 4800, 9600, 19200, 38400, 57600, 115200
 */
void config_UART0(uint32_t baudRate);

/**
 * Función auxiliar para enviar una cadena de texto (String)
 */
void UART0_SendString(char* str);

#endif
