#ifndef TP_SERVO_H
#define TP_SERVO_H

#include "LPC17xx.h"

/**
 * @brief  Calcula el ángulo actual del servomotor basándose en el ancho de pulso PWM medido o seteado.
 * @details Realiza una interpolación lineal inversa libre de flotantes. Protege contra división por cero
 * y limita el resultado si el pulso actual excede los límites provistos.
 * @param  anguloMinimo       Ángulo correspondiente al pulso mínimo (usualmente 0).
 * @param  anguloMaximo       Ángulo correspondiente al pulso máximo (usualmente 180).
 * @param  anchoPulsoMinimo   Ancho de pulso en microsegundos o ticks para el ángulo mínimo (ej: 1000).
 * @param  anchoPulsoMaximo   Ancho de pulso en microsegundos o ticks para el ángulo máximo (ej: 2000).
 * @param  anchoPulsoActual   El ancho de pulso actual que se desea convertir a grados.
 * @return El valor del ángulo calculado en grados (dentro del rango anguloMinimo - anguloMaximo).
 */
uint16_t SERVO_getAnguloDesdePWM(uint16_t anguloMinimo, uint16_t anguloMaximo, uint32_t anchoPulsoMinimo,
                                 uint32_t anchoPulsoMaximo, uint32_t anchoPulsoActual);

/**
 * @brief  Calcula el ancho de pulso PWM necesario para posicionar el servo en un ángulo específico.
 * @details Realiza una interpolación lineal directa optimizada para enteros. Si el ángulo solicitado
 * está fuera de los límites, se recorta automáticamente al mínimo o máximo permitido.
 * @param  anguloMinimo       Ángulo correspondiente al pulso mínimo (usualmente 0).
 * @param  anguloMaximo       Ángulo correspondiente al pulso máximo (usualmente 180).
 * @param  anchoPulsoMinimo   Ancho de pulso en microsegundos o ticks para el ángulo mínimo (ej: 1000).
 * @param  anchoPulsoMaximo   Ancho de pulso en microsegundos o ticks para el ángulo máximo (ej: 2000).
 * @param  anguloActual       El ángulo objetivo que se desea convertir a ancho de pulso.
 * @return El valor del pulso equivalente (en las mismas unidades que se ingresaron los límites).
 */
uint32_t SERVO_getPulsoDesdeAngulo(uint16_t anguloMinimo, uint16_t anguloMaximo, uint32_t anchoPulsoMinimo,
                                   uint32_t anchoPulsoMaximo, uint16_t anguloActual);

/**
 * @brief  Calcula dinámicamente el paso del servo en ticks de Timer.
 * @param  pclk_hz          Frecuencia del PCLK del Timer (ej. 25000000 para 25 MHz).
 * @param  pr_val           Valor asignado al registro PR (Prescaler).
 * @param  pulse_min_us     Ancho de pulso mínimo en microsegundos (ej. 1000).
 * @param  pulse_max_us     Ancho de pulso máximo en microsegundos (ej. 2000).
 * @param  rango_grados     Rango total del servo (ej. 180).
 * @param  grados_por_paso  Cuántos grados quieres avanzar en cada movimiento (ej. 2).
 * @return El valor del paso convertido a ticks de Timer listo para sumar/restar a MR0.
 */
uint32_t SERVO_CalcularPaso(uint32_t pclk_hz,
                            uint32_t pr_val,
                            uint32_t pulse_min_us,
                            uint32_t pulse_max_us,
                            uint16_t rango_grados,
                            uint16_t grados_por_paso);


#endif //TP_SERVO_H
