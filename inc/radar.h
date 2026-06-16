#ifndef TP_ULTRASONIDO_H
#define TP_ULTRASONIDO_H

typedef enum {
    ST_RADAR_ESPERAR_SERVO, // Espera a que el servo termine de moverse
    ST_RADAR_ESPERANDO_ECHO, // Espera a que la interrupción mida el pulso del sensor
    ST_RADAR_ESPERANDO_CONSUMO // Espera a que modos.c lea el dato y baje la bandera
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
void RADAR_Init();

/**
 * @brief Máquina de estados principal del Radar. Debe llamarse continuamente en el while(1).
 * @details Coordina de forma no-bloqueante la secuencia:
 * Esperar Asentamiento -> Disparar Trigger -> Medir Echo -> Enviar Datos -> Dar Paso.
 */
void RADAR_Actualizar();

#endif
