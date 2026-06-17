#ifndef TP_CONFIG_H
#define TP_CONFIG_H

#include "lpc_types.h"

typedef enum {
    SISTEMA_CONFIGURANDO, // Esperando ingresos por teclado
    SISTEMA_INICIANDO_MODO, // Transición para inicializar variables de barrido
    SISTEMA_EJECUTANDO, // Corriendo el barrido/medición activa
    SISTEMA_PARADO // Parada de emergencia o cancelación
} ESTADO_SISTEMA;

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
extern volatile ESTADO_SISTEMA GLOBAL_estado_sistema;

/* Parametros de los modos */
extern volatile uint16_t GLOBAL_angulo0;
extern volatile uint16_t GLOBAL_angulo1;
extern volatile uint16_t GLOBAL_distancia;

/* Variables actuales */
extern volatile Bool GLOBAL_medicion_lista;
extern volatile uint16_t GLOBAL_ultima_distancia;
extern volatile uint16_t GLOBAL_ultimo_angulo;
/* ################################################ */


/* ################################################ */
/* Teclado  P2[10:13] OUTPUT - P2[0:3] INPUT */
#define TECLADO_PORT	            PORT_2 // NO CAMBIAR
#define TECLADO_FILAS_MASK          (0 | (1 << PIN_0) | (1 << PIN_1) | (1 << PIN_2) | (1 << PIN_3))
#define TECLADO_COLUMNAS_MASK       (0 | (1 << PIN_10) | (1 << PIN_11) | (1 << PIN_12) | (1 << PIN_13))  // NO CAMBIAR
#define TECLADO_MS_BARRIDO          25
#define TECLADO_CONTADOR_DELAY      20 // 3 * 25ms = 75ms (un antirrebote perfecto)
extern const char teclado[4][4];
/* ################################################ */


/* ################################################ */
/* Ultrasonido */
#define ULTRA_TRIG_PORT             PORT_0
#define ULTRA_TRIG_PIN              PIN_10
#define ULTRA_TRIG_MASK             (0 | (1 << ULTRA_TRIG_PIN))
#define ULTRA_ECHO_PORT             PORT_0
#define ULTRA_ECHO_PIN              PIN_5 // CAP2_1
#define ULTRA_TRIG_TIME_US          10  // 10 uS de pulso al trigger
/* ################################################ */


/* ################################################ */
/* Servo Motor */
#define SERVO_SIGNAL_PORT           PORT_0
#define SERVO_SIGNAL_PIN            PIN_4
#define SERVO_SIGNAL_PERIODO_uS     20000       // 20 ms
#define SERVO_MAX_ANGULO            180         // 180 grados
#define SERVO_MIN_ANGULO            0           // 0 grados
#define SERVO_MIN_PULSE_uS          1000        // 1 ms
#define SERVO_MAX_PULSE_uS          2000        // 2 ms
#define SERVO_MASK_PIN              (0 | (1 << SERVO_SIGNAL_PIN))
#define SERVO_CICLOS_ESTABILIZACION_PASO   3    // 5 ciclos PWM para estabilizar
extern volatile uint32_t SERVO_PASO;
/* ################################################ */


/* ################################################ */
/* LCD - I2C */
#define LCD_SLAVE_ADDRESS 0x27
/* ################################################ */


/* ################################################ */
/* UART */
#define UART_BUFFER_SIZE 25
extern volatile uint8_t bufferUART[UART_BUFFER_SIZE];
/* ################################################ */

/* ################################################ */
/* DMA Canales */
#define DMA_canal_UART0     (GPDMA_CH) GPDMA_CH_1
#define DMA_canal_ADC       (GPDMA_CH) GPDMA_CH_0
/* ################################################ */

/* ################################################ */
/* Trimmer - ADC */
#define TRIMMER_MAX_TIEMPO_PROCESAMIENTO 1000000       // Valor maximo para bucle
#define TRIMMER_MIN_TIEMPO_PROCESAMIENTO 500        // Valor minimo para bucle
#define TRIMMER_MAX_ANGULO_STEP 30                      // Valor maximo de SERVO_PASO
#define TRIMMER_MIN_ANGULO_STEP 1                       // Valor minimo de SERVO_PASO
extern volatile uint32_t GLOBAL_trimmer_value_delay;
extern volatile uint32_t GLOBAL_trimmer_value_step;
extern volatile uint32_t adc_buffer[2];
extern volatile uint32_t varsADC;
/* ################################################ */

#endif
