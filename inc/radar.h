#ifndef TP_ULTRASONIDO_H
#define TP_ULTRASONIDO_H

typedef enum {
    ST_RADAR_ESPERAR_SERVO,
    ST_RADAR_DISPARAR_TRIGGER,
    ST_RADAR_ESPERANDO_ECHO,
    ST_RADAR_PROCESAR_MEDICION,
    ST_RADAR_ENVIAR_REPORTES,
    ST_RADAR_AVANZAR_PASO
} EstadoRadar_t;

/**
 * @brief Comienza una nueva medición únicamente si:
 *        1. El servo está estabilizado (posición final alcanzada)
 *        2. No hay medición en progreso
 * @details Esta función es NO-BLOQUEANTE. El bucle while(1) continúa girando
 *          y solo cuando se cumplen las condiciones se dispara el trigger.
 */
void ComenzarMedicion();

/**
 * @brief Inicializa de forma centralizada todos los módulos y periféricos del Radar.
 * @details Llama internamente a las inicializaciones de UART0, Servo (Timer0), Ultrasonido (Timer2), DMA e I2C.
 */
void RADAR_Init(void);

/**
 * @brief Máquina de estados principal del Radar. Debe llamarse continuamente en el while(1).
 * @details Coordina de forma no-bloqueante la secuencia:
 * Esperar Asentamiento -> Disparar Trigger -> Medir Echo -> Enviar Datos -> Dar Paso.
 */
void RADAR_Actualizar(void);

#endif