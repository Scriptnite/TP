#include "config.h"


/* ################################################ */
/* Banderas y variables de estado */
volatile MODO GLOBAL_modo_actual = RANGO_DE_ANGULOS;
volatile ORDEN GLOBAL_orden_actual = EN_ESPERA;
volatile ESTADO_DE_ESPERA GLOBAL_estado_actual = ESPERANDO_PRIMER_DATO;
volatile ESTADO_SISTEMA GLOBAL_estado_sistema = SISTEMA_EJECUTANDO; //TODO revisar

volatile uint16_t GLOBAL_angulo0 = 0;
volatile uint16_t GLOBAL_angulo1 = 0;
volatile uint16_t GLOBAL_distancia = 0;

volatile Bool GLOBAL_medicion_lista;
volatile uint16_t GLOBAL_ultima_distancia;
volatile uint16_t GLOBAL_ultimo_angulo;
/* ################################################ */

/* ################################################ */
/* Banderas de Periféricos de Comunicación */
volatile Bool GLOBAL_DMA_UART_completado = TRUE;
volatile Bool GLOBAL_I2C_completado = TRUE;
/* ################################################ */

/* ################################################ */
/* Teclado  P2[10:13] INPUT - P2[0:3] OUTPUT */

const char teclado[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
/* ################################################ */

/* ################################################ */
/* Ultrasonido */
/* ################################################ */
