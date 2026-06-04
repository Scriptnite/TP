#ifndef TP_CONFIG_H
#define TP_CONFIG_H

#include "LPC17xx.h"

typedef enum {
    RANGO_DE_ANGULOS = 'A',
    ANGULO_ESPECIFICO = 'B',
    DISTANCIA_ESPECIFICA = 'C'
} MODO;

typedef enum {
    ESPERANDO_PRIMER_DATO,
    ESPERANDO_SEGUNDO_DATO,
    ESPERANDO_UNICO_DATO,
} ESTADO_DE_ESPERA;

typedef enum {
    CAMBIAR_MODO, /* Debera cambiar modo, y esperar el/los nuevos datos */
    CANCELAR, /* Borrará el valor que se esté cargando */
    CONFIRMAR, /* Confirmará el valor que se esté cargando, y pasará a esperar el siguiente */
    EN_ESPERA /* Orden por defecto, a la ausencia de cualquier otra */
} ORDEN;

typedef enum {
    PERIODO_ONDA, /** Valor de tiempo se considera como periodo completo de la onda. */
    PERIODO_INTERRUPCION /** Valor de tiempo se considera como período de interrupción. */
} PERIODO;


/* ################################################ */
/* Banderas y variables de estado */
extern volatile MODO GLOBAL_modo_actual;
extern volatile ORDEN GLOBAL_orden_actual;
extern volatile ESTADO_DE_ESPERA GLOBAL_estado_actual;

extern volatile uint16_t GLOBAL_angulo0;
extern volatile uint16_t GLOBAL_angulo1;
extern volatile uint16_t GLOBAL_distancia;
/* ################################################ */


/* ################################################ */
/* Teclado  P2[10:13] INPUT - P2[6:9] OUTPUT */
#define TECLADO_PORT	        PORT_2 // NO CAMBIAR
#define TECLADO_FILAS_MASK      (0 | (1 << PIN_6) | (1 << PIN_7) | (1 << PIN_8) | (1 << PIN_9))
#define TECLADO_COLUMNAS_MASK   (0 | (1 << PIN_10) | (1 << PIN_11) | (1 << PIN_12) | (1 << PIN_13))  // NO CAMBIAR
#define TECLADO_MS_BARRIDO 25
extern volatile uint8_t TECLADO_FILA_ACTUAL;
extern const char teclado[4][4];
extern volatile uint8_t TECLADO_CONTADOR_INTERRUPCIONES_DEBOUNCE;
/* ################################################ */


/* ################################################ */
/* Ultrasonido */
#define TRIG_PORT           PORT_0
#define TRIG_PIN            PIN_22

#define ECHO_PORT           PORT_0
#define ECHO_PIN            PIN_23
/* ################################################ */


#endif
