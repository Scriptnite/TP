#ifndef SERVOMOTOR_H_
#define SERVOMOTOR_H_
#include <stdint.h>
#include <stdbool.h>
/**
 * @brief Distintos modos de funcionamiento del sistema radar
 */
typedef enum {
    RADAR_MODE_SWEEP,
    RADAR_MODE_FIXED_ANGLE,
    RADAR_MODE_TRACKING
} RadarMode_T;

/**
 * @brief Direcciones posibles para el barrido del servomotor
 */
typedef enum{
	SWEEP_FORWARD, // sentido horario, de 0 a 180 grados
	SWEEP_REVERSE //sentido antihorario, de 180 a 0 grados
}SweepDirection;

/**
 * @brief  Estructura de configuración del radar (Aplica principio OCP de SOLID)
 * @details Permite cambiar el comportamiento del radar (límites o velocidad) sin modificar las funciones internas.
 */
typedef struct {
    uint32_t current_pulse;  // Ancho de pulso actual en microsegundos
    uint32_t step;           // Paso de incremento/decremento por ciclo
    uint32_t min_pulse;      // Límite mínimo permitido (ej: 1000us)
    uint32_t max_pulse;      // Límite máximo permitido (ej: 2000us)
    SweepDirection direction;// Dirección actual de giro
} ServoRadar_t;

/* =========================================================================
   PROTOTIPOS DE FUNCIONES PÚBLICAS
   ========================================================================= */

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



/**
 * @brief  Incrementa o decrementa el ancho de pulso del servo para avanzar un grado.
 * @details Modifica el registro de Match (MR0) del TIMER0 controlando que no se
 * excedan los límites físicos del servomotor (1ms a 2ms). Cuando llega
 * al extremo, conmuta automáticamente la dirección de giro.
 * @param  Ninguno.
 * @return Ninguno.
 */
void RADAR_step(void);

/**
 * @brief  Setea un ciclo de trabajo en el pwm, para mantener el servo en cierto angulo
 * @details Modifica el registro de Match (MR0) del TIMER0 controlando que no se
 * excedan los límites físicos del servomotor (1ms a 2ms).
 * @param  angle: angulo deseado para setear en el servomotor .
 * @return true si el angulo se seteo con exito, false si el angulo esta fuera de rango.
 */
bool RADAR_setAngle(uint32_t angle);


#endif /*SERVOMOTOR_H_ */
