#ifndef TP_TECLADO_H
#define TP_TECLADO_H

#include "lpc17xx_common.h"


/**
 * @file teclado.h
 * @brief Driver para teclado matricial 4x4.
 *
 * El teclado utiliza:
 * - Filas:		P2.6 - P2.9  (salidas)
 * - Columnas:	P2.10 - P2.13 (entradas)
 *
 * Las filas son excitadas secuencialmente mediante SysTick y las columnas
 * generan interrupciones por flanco de subida cuando se detecta una pulsación.
 *
 * El valor asociado a cada tecla se obtiene utilizando la matriz
 * @ref teclado definida en config.h.
 *
 * Teclas especiales:
 *
 * - [0-9]	-> ingreso de valores numéricos.
 * - [*]	-> confirmar entrada.
 * - [#]	-> cancelar o borrar entrada.
 * - [A]	-> modo rango de ángulos.
 * - [B]	-> modo ángulo específico.
 * - [C]	-> modo distancia específica.
 *
 * Las teclas de selección de modo modifican la variable global
 * @ref modo_actual.
 */

/**
 * @brief Inicializa el teclado matricial.
 *
 * Configura:
 * - GPIO.
 * - SysTick para el barrido de filas.
 * - EINT0, EINT1, EINT2 y EINT3 para la detección de pulsaciones.

 * @param miliSeconds Tiempo utilizado por SysTick para el barrido.
 */
void iniciarTeclado(uint32_t miliSeconds);

/**
 * @brief Configura los GPIO asociados al teclado.
 *
 * Los pines de entrada deben poseer resistencias Pull-Down externas.
 *
 * @param puerto Puerto GPIO utilizado.
 * @param pinMask_INPUT Máscara correspondiente a las columnas.
 */
void config_GPIO(LPC_PORT puerto, uint32_t pinMask_INPUT);

/**
 * @brief Configura las interrupciones externas asociadas al teclado.
 *
 * Configura:
 * - P2.10 -> EINT0
 * - P2.11 -> EINT1
 * - P2.12 -> EINT2
 * - P2.13 -> EINT3
 *
 * Todas las interrupciones son configuradas por flanco de subida.
 */
void config_EINT();

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
