#ifndef TP_CONFIG_H
#define TP_CONFIG_H

#include "lpc_types.h"

// En config.h
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
/* Banderas de Periféricos de Comunicación */
extern volatile Bool GLOBAL_DMA_UART_completado;
extern volatile Bool GLOBAL_I2C_completado;
/* ################################################ */


/* ################################################ */
/* Teclado  P2[10:13] INPUT - P2[5:8] OUTPUT */
#define TECLADO_PORT	            PORT_2 // NO CAMBIAR
#define TECLADO_FILAS_MASK          (0 | (1 << PIN_5) | (1 << PIN_6) | (1 << PIN_7) | (1 << PIN_8))
#define TECLADO_COLUMNAS_MASK       (0 | (1 << PIN_10) | (1 << PIN_11) | (1 << PIN_12) | (1 << PIN_13))  // NO CAMBIAR
#define TECLADO_MS_BARRIDO          25
#define TECLADO_CONTADOR_DELAY      15
extern const char teclado[4][4];
/* ################################################ */


/* ################################################ */
/* Ultrasonido */
#define ULTRA_TRIG_PORT             PORT_1
#define ULTRA_TRIG_PIN              PIN_31
#define ULTRA_TRIG_MASK             (1 << ULTRA_TRIG_PIN)
#define ULTRA_ECHO_PORT             PORT_1
#define ULTRA_ECHO_PIN              PIN_18 // CAP1_0
#define ULTRA_TRIG_TIME_US          10  // 10 uS de pulso al trigger
/* ################################################ */


/* ################################################ */
/* Servo Motor */
#define SERVO_SIGNAL_PORT           PORT_0
#define SERVO_SIGNAL_PIN            PIN_4
#define SERVO_SIGNAL_PERIODO_uS     20000   // 20 ms
#define SERVO_MAX_ANGULO            180     // 180 grados
#define SERVO_MIN_ANGULO            0       // 0 grados
#define SERVO_MIN_PULSE_uS          1000    // 1 ms
#define SERVO_MAX_PULSE_uS          2000    // 2 ms
#define SERVO_MASK_PIN              (0 | (1 << SERVO_SIGNAL_PIN))
#define SERVO_CICLOS_ESTABILIZACION_PASO   2  // 2 ciclos PWM para estabilizar
#define SERVO_MAX_ANGULO_STEP 20 // 20 grados
/* ################################################ */


/* ################################################ */
/* Trimmer */
#define TRIMMER_MAX_TIEMPO_PROCESAMIENTO 1000 //1000ms = 1s
extern volatile uint32_t GLOBAL_trimmer[2]; //GLOBAL_trimmer[tiempo_de_procesamiento, set_paso_servo]
/* ################################################ */


/* ################################################ */
/* Buzzer */
#define BUZZER_PORT                 PORT_0
#define BUZZER_PIN                  PIN_9
#define BUZZER_MASK                 (0 | (1 << BUZZER_PIN))
/* ################################################ */

/* ################################################ */
/* LCD - I2C */
#define LCD_SLAVE_ADDRESS 0x27
/* ################################################ */


#endif
