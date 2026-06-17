#include "modos.h"
#include <stdlib.h>
#include <stdio.h>
#include "../Drivers/inc/servo.h"
#include "../inc/servo.h"
#include "telemetria.h"
#include "config.h"

#define MAX_MEDICIONES (((SERVO_MAX_ANGULO - SERVO_MIN_ANGULO) / TRIMMER_MIN_ANGULO_STEP) + 1)

static MEDICION mediciones_tabla[MAX_MEDICIONES];
static uint32_t escaneos_count = 0;
static MODO_ESTADO modo_estado = INACTIVO;

static uint16_t angulo_actual; /* Ángulo actual que se está procesando */
static uint16_t range_start, range_end; /* Ángulos de inicio y fin para modo RANGO */
static uint8_t range_dir; /*  // 1 = Subiendo, -1 = Bajando */

// Variables para el doble escaneo del Modo C
static int8_t scan_dir = 1; // 1 = Subiendo, -1 = Bajando
static uint8_t scan_passes = 0; // Contador de pasadas (ida y vuelta)
static size_t best_index_encontrado = 0; // Guarda el índice ganador de la matriz

/**
 * @brief Estados para el modo ANGULO_ESPECIFICO
 * @details Flujo: SIN_PARAMETRO -> MOVIENDO_Y_ESTABILIZANDO -> MIDIENDO -> MIDIENDO (bucle permanente)
 */
static enum {
    ANGULO_SIN_PARAMETRO, /* Estado inicial, esperando parámetro GLOBAL_angulo0 */
    ANGULO_MOVIENDO_Y_ESTABILIZANDO, /* Moviendo servo al ángulo especificado y esperando estabilización */
    ANGULO_MIDIENDO, /* Realizando medición de distancia en ángulo fijo (permanentemente) */
    ANGULO_LISTO /* Modo completado/cancelado */
} ANGULO_ESPECIFICO_ESTADO;

/**
 * @brief Estados para el modo RANGO_DE_ANGULOS
 * @details Flujo: SIN_PARAMETROS -> INIT -> MOVIENDO -> ESPERANDO_MEDICION -> SIGUIENTE -> ... -> LISTO
 * @details El servo barre desde GLOBAL_angulo0 hasta GLOBAL_angulo1 en pasos de SERVO_PASO
 */
static enum {
    RANGO_SIN_PARAMETROS = 0, /* Estado inicial, esperando parámetros GLOBAL_angulo0 y GLOBAL_angulo1 */
    RANGO_INIT, /* Inicializando variables para el barrido (ángulo inicial, dirección) */
    RANGO_MOVIENDO, /* Moviendo servo al siguiente ángulo en el rango */
    RANGO_ESPERANDO_MEDICION, /* Esperando a que la medición de distancia esté lista */
    RANGO_SIGUIENTE, /* Verificando si hay más ángulos en el rango; si sí, volver a RANGO_MOVIENDO */
    RANGO_LISTO /* Barrido completado */
} RANGO_DE_ANGULOS_ESTADO;

/**
 * @brief Estados para el modo DISTANCIA_ESPECIFICA
 * @details Flujo: SIN_PARAMETROS -> SCAN_INIT -> SCAN_MOVIENDO -> SCAN_MIDIENDO -> SCAN_MOVIENDO (bucle) -> SCAN_GUARDAR_ESPERANDO -> BUSCAR -> MOVIENDO_RESULTADO -> LISTO
 * - Primera fase: Escanear desde SERVO_MIN_ANGULO hasta SERVO_MAX_ANGULO, guardando (ángulo, distancia) en matriz.
 * - Segunda fase: Cuando usuario ingrese GLOBAL_distancia, buscar la medición con distancia más cercana y mover el servo a ese ángulo.
 */
static enum {
    DISTANCIA_SIN_PARAMETROS, /* Estado inicial, esperando comando de escaneo */
    DISTANCIA_SCAN_INIT, /* Inicializando escaneo: resetear contador y ángulo inicial */
    DISTANCIA_SCAN_MOVIENDO, /* Moviendo servo al siguiente ángulo del escaneo */
    DISTANCIA_SCAN_MIDIENDO, /* Esperando medición de distancia en ángulo actual */
    DISTANCIA_SCAN_GUARDAR_ESPERANDO, /* Escaneo completado; guardando tabla y esperando ingreso de GLOBAL_distancia */
    DISTANCIA_BUSCAR, /* Buscando en la tabla el ángulo con distancia más cercana a GLOBAL_distancia */
    DISTANCIA_MOVIENDO_RESULTADO, /* Moviendo servo al ángulo resultante de la búsqueda */
    DISTANCIA_LISTO /* Operación completada */
} DISTANCIA_ESPECIFICA_ESTADO;

/**
 * @brief Comprueba entradas y arranca modo si corresponde
 */
void Modos_comienza_si_esta_listo() {
    if (GLOBAL_estado_sistema != SISTEMA_INICIANDO_MODO) return;

    switch (GLOBAL_modo_actual) {
        case ANGULO_ESPECIFICO: {
            if (GLOBAL_angulo0 <= SERVO_MAX_ANGULO) {
                ANGULO_ESPECIFICO_ESTADO = ANGULO_MOVIENDO_Y_ESTABILIZANDO;
                angulo_actual = GLOBAL_angulo0;
                modo_estado = EJECUTANDO;
                GLOBAL_estado_sistema = SISTEMA_EJECUTANDO;
            }
            break;
        }
        case RANGO_DE_ANGULOS: {
            if (GLOBAL_angulo0 <= SERVO_MAX_ANGULO && GLOBAL_angulo1 <= SERVO_MAX_ANGULO) {
                range_start = GLOBAL_angulo0;
                range_end = GLOBAL_angulo1;

                if (range_start > range_end) {
                    uint16_t tmp = range_start;
                    range_start = range_end;
                    range_end = tmp;
                }

                angulo_actual = range_start;
                range_dir = 1;

                RANGO_DE_ANGULOS_ESTADO = RANGO_MOVIENDO;
                modo_estado = EJECUTANDO;
                GLOBAL_estado_sistema = SISTEMA_EJECUTANDO;
            }
            break;
        }
        case DISTANCIA_ESPECIFICA: {
            if (GLOBAL_distancia > 0) {
                escaneos_count = 0;
                scan_passes = 0;
                scan_dir = 1; // Inicia subiendo
                best_index_encontrado = 0;
                angulo_actual = SERVO_MIN_ANGULO;

                DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_SCAN_MOVIENDO;
                modo_estado = EJECUTANDO;
                GLOBAL_estado_sistema = SISTEMA_EJECUTANDO;
            }
            break;
        }
    }
}

void Modos_init() {
    escaneos_count = 0;
    modo_estado = INACTIVO;
    ANGULO_ESPECIFICO_ESTADO = ANGULO_SIN_PARAMETRO;
    RANGO_DE_ANGULOS_ESTADO = RANGO_SIN_PARAMETROS;
    DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_SIN_PARAMETROS;
}

void Modos_bucle() {
    // Si el teclado dio la orden, intentamos disparar el modo
    if (GLOBAL_estado_sistema == SISTEMA_INICIANDO_MODO) Modos_comienza_si_esta_listo();

    // Si el gatillo funcionó, corremos los pasos
    if (GLOBAL_estado_sistema == SISTEMA_EJECUTANDO && modo_estado == EJECUTANDO) {
        switch (GLOBAL_modo_actual) {
            case ANGULO_ESPECIFICO: {
                switch (ANGULO_ESPECIFICO_ESTADO) {
                    case ANGULO_MOVIENDO_Y_ESTABILIZANDO: {
                        SERVO_setAngulo(angulo_actual);
                        ANGULO_ESPECIFICO_ESTADO = ANGULO_MIDIENDO;
                        break;
                    }
                    case ANGULO_MIDIENDO: {
                        // Esperamos a que el radar dispare y traiga el dato
                        if (GLOBAL_medicion_lista) {
                            GLOBAL_medicion_lista = FALSE;

                            GLOBAL_ultimo_angulo = angulo_actual;

                            TELEMETRIA_Actualizar();
                            SERVO_ForzarEspera(3);
                        }
                        break;
                    }

                    default: break;
                }
                break;
            }
            case RANGO_DE_ANGULOS: {
                switch (RANGO_DE_ANGULOS_ESTADO) {
                    case RANGO_MOVIENDO: {
                        SERVO_setAngulo(angulo_actual);
                        RANGO_DE_ANGULOS_ESTADO = RANGO_ESPERANDO_MEDICION;
                        break;
                    }
                    case RANGO_ESPERANDO_MEDICION: {
                        if (GLOBAL_medicion_lista) {
                            GLOBAL_ultimo_angulo = angulo_actual;

                            // Calculamos el siguiente paso continuo (Ida-Vuelta)
                            if (range_dir == 1) {
                                // Si estamos subiendo
                                if (angulo_actual + SERVO_PASO >= range_end) {
                                    angulo_actual = range_end;
                                    range_dir = -1; // Chocamos el techo, ahora bajamos
                                }
                                else angulo_actual += SERVO_PASO;
                            }
                            else {
                                // Si estamos bajando
                                if (angulo_actual <= range_start + SERVO_PASO) {
                                    angulo_actual = range_start;
                                    range_dir = 1; // Chocamos el piso, ahora subimos
                                }
                                else angulo_actual -= SERVO_PASO;
                            }

                            RANGO_DE_ANGULOS_ESTADO = RANGO_MOVIENDO;
                            GLOBAL_medicion_lista = FALSE;

                            TELEMETRIA_Actualizar();
                        }
                        break;
                    }
                    case RANGO_LISTO: {
                        GLOBAL_estado_sistema = SISTEMA_CONFIGURANDO;
                        modo_estado = INACTIVO;
                        break;
                    }
                    default: break;
                }
                break;
            }
            case DISTANCIA_ESPECIFICA: {
                switch (DISTANCIA_ESPECIFICA_ESTADO) {
                    case DISTANCIA_SCAN_MOVIENDO: {
                        SERVO_setAngulo(angulo_actual);
                        DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_SCAN_MIDIENDO;
                        break;
                    }
                    case DISTANCIA_SCAN_MIDIENDO: {
                        if (GLOBAL_medicion_lista) {
                            GLOBAL_medicion_lista = FALSE;

                            // Guardar en la matriz
                            if (escaneos_count < MAX_MEDICIONES) {
                                mediciones_tabla[escaneos_count].ANGULO = angulo_actual;
                                mediciones_tabla[escaneos_count].DISTANCIA = GLOBAL_ultima_distancia;
                                escaneos_count++;
                            }

                            // Lógica de Doble Barrido (Ping-Pong completo de Ida y Vuelta)
                            if (scan_dir == 1) {
                                if (angulo_actual + SERVO_PASO >= SERVO_MAX_ANGULO) {
                                    angulo_actual = SERVO_MAX_ANGULO;
                                    scan_dir = -1; // Chocó arriba, ahora baja
                                    scan_passes++; // Terminó 1 pasada (Ida)
                                }
                                else angulo_actual += SERVO_PASO;
                            }
                            else {
                                if (angulo_actual <= SERVO_MIN_ANGULO + SERVO_PASO) {
                                    angulo_actual = SERVO_MIN_ANGULO;
                                    scan_dir = 1; // Chocó abajo, ahora sube
                                    scan_passes++; // Terminó otra pasada (Vuelta)
                                }
                                else angulo_actual -= SERVO_PASO;
                            }

                            // Si completó la Ida y la Vuelta (2 pasadas)
                            if (scan_passes >= 2) {
                                DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_BUSCAR;
                                GLOBAL_medicion_lista = TRUE;
                            }
                            else DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_SCAN_MOVIENDO;
                        }
                        break;
                    }
                    case DISTANCIA_BUSCAR: {
                        if (escaneos_count == 0) {
                            GLOBAL_medicion_lista = FALSE;
                            DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_LISTO;
                            break;
                        }

                        uint16_t target = GLOBAL_distancia;
                        best_index_encontrado = 0;
                        uint32_t best_diff = (uint32_t)abs((int)mediciones_tabla[0].DISTANCIA - (int)target);

                        for (size_t i = 1; i < escaneos_count; ++i) {
                            uint32_t diff = (uint32_t)abs((int)mediciones_tabla[i].DISTANCIA - (int)target);
                            if (diff < best_diff) {
                                best_diff = diff;
                                best_index_encontrado = i;
                            }
                        }

                        angulo_actual = mediciones_tabla[best_index_encontrado].ANGULO;

                        //printf("\r\n========================================\r\n");
                        //printf(" ESCANEO COMPLETADO (Ida y Vuelta)\r\n");
                        //printf(" Distancia Buscada: %d cm\r\n", target);
                        //printf(" Mejor Coincidencia: %d cm (Error: %d cm)\r\n",
                        //mediciones_tabla[best_index_encontrado].DISTANCIA, best_diff);
                        //printf(" Posicion Encontrada: %d grados\r\n", angulo_actual);
                        // printf(" Moviendo servo al objetivo...\r\n");
                        //printf("========================================\r\n");

                        DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_MOVIENDO_RESULTADO;
                        break;
                    }
                    case DISTANCIA_MOVIENDO_RESULTADO: {
                        SERVO_setAngulo(angulo_actual);
                        GLOBAL_medicion_lista = FALSE;

                        if (escaneos_count > 0) {
                            GLOBAL_ultimo_angulo = mediciones_tabla[best_index_encontrado].ANGULO;
                            GLOBAL_ultima_distancia = mediciones_tabla[best_index_encontrado].DISTANCIA;
                        }

                        TELEMETRIA_Actualizar();

                        DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_LISTO;
                        break;
                    }
                    case DISTANCIA_LISTO: {
                        modo_estado = INACTIVO;
                        DISTANCIA_ESPECIFICA_ESTADO = DISTANCIA_SIN_PARAMETROS;
                        GLOBAL_estado_sistema = SISTEMA_CONFIGURANDO;
                        GLOBAL_medicion_lista = FALSE;
                        break;
                    }
                    default: break;
                }
                break;
            }
        }
    }
}
