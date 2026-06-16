#ifndef TP_MODOS_H
#define TP_MODOS_H

#include "config.h"

typedef enum {
    INACTIVO = 0, /* Sistema inactivo, esperando iniciar modo */
    EJECUTANDO /* Sistema ejecutando un modo */
} MODO_ESTADO;

typedef struct {
    uint16_t ANGULO; /* Ángulo en grados (ej. 165) */
    uint16_t DISTANCIA; /* Distancia medida en centimetros (ej. 24) */
} MEDICION;

/**
 * @brief inicializa estructuras internas
 */
void Modos_init();

/**
 * @brief Llamar periódicamente desde main()
 */
void Modos_bucle();

#endif
