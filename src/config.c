#include "config.h"


/* ################################################ */
/* Banderas y variables de estado */
volatile MODO GLOBAL_modo_actual = RANGO_DE_ANGULOS;
volatile ORDEN GLOBAL_orden_actual = EN_ESPERA;
volatile ESTADO_DE_ESPERA GLOBAL_estado_actual = ESPERANDO_PRIMER_DATO;
volatile ESTADO_SISTEMA GLOBAL_estado_sistema = SISTEMA_CONFIGURANDO; //TODO revisar

volatile uint16_t GLOBAL_angulo0 = 999;
volatile uint16_t GLOBAL_angulo1 = 999;
volatile uint16_t GLOBAL_distancia = 0;

volatile Bool GLOBAL_medicion_lista;
volatile uint16_t GLOBAL_ultima_distancia;
volatile uint16_t GLOBAL_ultimo_angulo;
/* ################################################ */

volatile uint32_t SERVO_PASO = TRIMMER_MIN_ANGULO_STEP;


/* ################################################ */
/* Teclado  P2[10:13] INPUT - P2[0:3] OUTPUT */

const char teclado[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/* ################################################ */
/* Trimmer - ADC */
volatile uint32_t GLOBAL_trimmer_value_delay = 1000000;
volatile uint32_t GLOBAL_trimmer_value_step = 5;
volatile uint32_t adc_buffer[2];
volatile uint32_t varsADC;
/* ################################################ */

