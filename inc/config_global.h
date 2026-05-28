#ifndef CONFIG_GLOBAL_H_
#define CONFIG_GLOBAL_H_
#include <stdint.h>

/**
 * Timer configurado para generar la señal PWM del servomotor
 */
void config_TMR0(void);

/**
 *
 */
void config_TMR1(void);

/**
 *
 */
void config_DMA(void);

/**
 *
 * @param baudRate Baudios al que se configura el módulo, si se deja en blanco, se configurará a 9600. Valores posibles: 2400, 4800, 9600, 19200, 38400, 57600, 115200
 */
void config_UART0(uint32_t baudRate);

// Función auxiliar para enviar una cadena de texto (String)
void UART0_SendString(char *str);

/**
 * Configurado a 700uA.
 */
void config_DAC(void);

/**
 *
 */
void config_GPIO(void);

/**
 *
 */
void config_I2C(void);

/**
 *
 */
void config_ADC(void);

#endif /* CONFIG_GLOBAL_H_ */