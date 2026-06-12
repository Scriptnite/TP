#ifndef TELEMETRIA_H_
#define TELEMETRIA_H_

#include <stdint.h>

#define BUFFER_SIZE 32

/**
 * @brief  Estructura de los datos telemetricos del radar
 * @details Estos datos se guardan en arrays de 16bytes, con el objetivo de ser enviados mediante comunicacion uart
 */
typedef struct {
    char distancia[16]; // "D:XXX.XX "
    char angulo[16];    // "A:YYY.YY\r\n"
} Telemetria_T;

/**
 * @brief  Actualiza los datos de telemetria
 * @param  distancia: double que indica la distancia en cm.
 *		   angulo: double que indica el angulo del servomotor.
 * @return Ninguno.
 */
void TELEMETRIA_actualizar(double distancia, double angulo);

#endif
