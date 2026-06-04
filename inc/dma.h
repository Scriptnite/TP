#ifndef TP_DMA_H
#define TP_DMA_H

/**
 *@brief Header configuracion de los canales de DMA
 *
 * Channel 0 = ADC -> Memoria
 * Channel 1 = Memoria (TMRx) -> DAC
 * Channel 2 = Memoria -> UART0
 * Channel 3 -> Memoria -> I2C
 */

/**
 * @brief Channel 0 = ADC -> Memoria
 * toma el Dato del ADC y lo lleva a una variable global
 * trigger Soruce ADC - CH_0
 */
void config_DMA_CH0();

/**
 * @brief Channel 1 = Memoria (TMRx) -> DAC
 * * toma el Dato de la direccion de memoria y lo lleva al DAC
 * trigger soruce (TMR2)
 */
void config_DMA_CH1();

/**
 * @brief Channel 2 = Memoria -> UART0
 * Incio manual del canal 2 atraves del handler del capture, enviara lso datos de distancia, angulos a UART
 */
void config_DMA_CH2();

/**
 * @brief Channel 3 -> Memoria -> I2C
 * * Incio manual del canal 3 atraves del handler del capture, enviara lso datos de distancia, angulos a I2C
 */
void config_DMA_CH3();

#endif
