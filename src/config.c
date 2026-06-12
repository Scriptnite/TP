#include "config.h"


/* ################################################ */
/* Banderas y variables de estado */
volatile MODO GLOBAL_modo_actual = RANGO_DE_ANGULOS;
volatile ORDEN GLOBAL_orden_actual = EN_ESPERA;
volatile ESTADO_DE_ESPERA GLOBAL_estado_actual = ESPERANDO_PRIMER_DATO;

volatile uint16_t GLOBAL_angulo0 = 0;
volatile uint16_t GLOBAL_angulo1 = 0;
volatile uint16_t GLOBAL_distancia = 0;
/* ################################################ */


/* ################################################ */
/* Teclado  P2[10:13] INPUT - P2[6:9] OUTPUT */
volatile uint8_t TECLADO_FILA_ACTUAL = 0;
volatile uint8_t TECLADO_CONTADOR_INTERRUPCIONES_DEBOUNCE = 0;
const char teclado[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
/* ################################################ */
