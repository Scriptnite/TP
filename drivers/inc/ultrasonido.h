#ifndef ULTRASONIDO_H_
#define ULTRASONIDO_H_

#include <stdint.h>
#include <stdbool.h> // Necesario para usar el tipo 'bool'

/**
 * @brief pines que utiliza el sensor ultrasonico
 */
#define TRIG_PORT 1
#define TRIG_PIN 20

/**
 * @brief  envia un pulso al pin trigger del sensor ultrasonico
 * @param  Ninguno.
 * @return Ninguno.
 * @note   Este metodo utiliza systick para poder contar los 10us del ancho de pulso. Asegurese de tener
 * el systick configurado y apagado antes de utilizar este metodo
 */
void ULTRASONIDO_sendTriggerPulse(void);

/**
 * @brief  calcula el valor de la distancia captada por el sensor de ultrasonido
 * @param  tiempo del ancho de pulso del pin echo.
 * @return valor de la distancia.
 */
double ULTRASONIDO_getDistance(void);

/**
 * @brief  indica si ya hay un nuevo tiempo del pin echo para calcular la distancia
 * @param  Ninguno.
 * @return true si hay una nueva distancia para calcular. False si no la hay
 */
bool ULTRASONIDO_isDataReady(void);


/**
 * @brief  Cambia la bandera que indica si hay un nuevo tiempo del pin echo
 * @param  Ninguno.
 * @return Ninguno.y
 */
void ULTRASONIDO_setDataisReady(bool ready);

/**
 * @brief  Cambia el tiempo del pulso en el pin echo.
 * @param  Ninguno.
 * @return Ninguno.
 */
void ULTRASONIDO_setEchoTime(uint32_t time);

#endif /*ULTRASONIDO_H_ */
