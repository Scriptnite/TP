#ifndef TP_SYSTICK_H
#define TP_SYSTICK_H

#include "LPC17xx.h"

/**
 * Este Periferico es utilizado para generar el barrido en las filas del teclado matricial para mapeo
 *
 * El Handler del SysTick será el encargado de hacer el desplazamiento del "uno" en el nibble
 */

/**
 * @brief Configura el SysTick para generar interrupciones periódicas.
 * @note Habilita la interrupción del SysTick y la correspondiente entrada en el NVIC.
 * @param miliSeconds Tiempo en milisegundos.
 */
void config_SysTickMs(uint32_t miliSeconds);

#endif
