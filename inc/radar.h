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

void ComenzarMedicion();

/**
 * @brief Inicializa de forma centralizada todos los módulos y periféricos del Radar.
 * @details Llama internamente a las inicializaciones de UART0, Servo (Timer0), Ultrasonido (Timer1), DMA e I2C.
 */
void RADAR_Init(void);

/**
 * @brief Máquina de estados principal del Radar. Debe llamarse continuamente en el while(1).
 * @details Coordina de forma no-bloqueante la secuencia:
 * Esperar Asentamiento -> Disparar Trigger -> Medir Echo -> Enviar Datos -> Dar Paso.
 */
void RADAR_Actualizar(void);

#endif