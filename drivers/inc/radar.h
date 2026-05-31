#ifndef RADAR_H_
#define RADAR_H_

#include <stdint.h>

/**
 * @brief Distintos modos de funcionamiento del sistema radar
 */
typedef enum {
    RADAR_MODE_SWEEP,
    RADAR_MODE_FIXED_ANGLE,
    RADAR_MODE_TRACKING
} RadarMode_T;




/**
 * @brief  Inicializa y configura el radar en modo sweep.
 * @param  ninguno
 * @return Ninguno.
 * @note   La misma funcion configura Timer0, Timer 1 y systick.
 */
void RADAR_init(void);

/**
 * @brief  Cambia el modo de operación actual del radar.
 * @param  newMode: Nuevo modo a ejecutar (miembro de la estructura RadarMode_T).
 * @return Ninguno.
 * @note   Esta función es segura para ser llamada desde el main o tareas de aplicación.
 */
void RADAR_setMode(RadarMode_T newMode);

/**
 * @brief  Ejecuta la máquina de estados del radar según el modo activo.
 * @param  Ninguno.
 * @return Ninguno.
 * @note   Debe ser llamada periódicamente (ej. cada 20ms o 50ms) desde el bucle
 * principal o un timer para controlar la velocidad de refresco del sistema.
 */
void RADAR_procces(void);

#endif
