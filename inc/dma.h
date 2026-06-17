#ifndef TP_DMA_H
#define TP_DMA_H

/**
 *@brief Header configuracion de los canales de DMA
 *
 * Channel 0 = ADC -> Memoria
 * Channel 1 = Memoria (TMRx) -> DAC
 * Channel 2 = Memoria -> UART0
 */

/**
 * @brief Channel 0 = ADC -> Memoria
 * toma el Dato del ADC y lo lleva a una variable global
 * trigger Source ADC - CH_0
 */
void DMA_config_CH0();

/**
 * @brief getter de GLOBAL_trimmer_value_delay
 * @details
 */
uint32_t DMA_get_tiempoProcesamiento(void);


/**
 * @brief getter de GLOBAL_trimmer_value_step
 * @details
 */
uint32_t DMA_get_servoStep(void);

/**
 * @brief Channel 1 = Memoria (TMRx) -> GPIO
 * * toma el Dato de la direccion de memoria y lo lleva al GPIO
 * trigger source (TMR2)
 */
void config_DMA_CH1();

/**
 * @brief Channel 2 = Memoria -> UART0
 * Inicio manual del canal 2 a traves del handler del "capture", enviará lso datos de distancia, angulos a UART
 */
void config_DMA_CH2();

/*
* @brief Channel 2 = Memoria -> I2C
 * Inicio manual del canal 2 a traves del handler del "capture", enviará lso datos de distancia, angulos a i2C
 */
void config_DMA_CH3();

#endif
