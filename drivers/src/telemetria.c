#include "telemetria.h"
#include "LPC17xx.h"
#include <stdio.h>

// Forzamos al compilador a ubicar esta estructura EXACTAMENTE en 0x2007C000 (0x2007C000 hasta 0x2007C020)
static volatile Telemetria_T* paquete_radar = (Telemetria_T *) 0x2007C000;

void TELEMETRIA_actualizar(double distancia, double angulo)
{
    // Armamos los strings directamente en la zona de memoria AHB
    sprintf((char*)paquete_radar->distancia, "D:%06.2f ", distancia);
    sprintf((char*)paquete_radar->angulo, "A:%06.2f\r\n", angulo);
}
