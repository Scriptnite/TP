#ifndef SERVO_H_
#define SERVO_H_

#include "lpc_types.h"

/**
 * @file servo.h
 * @brief Driver de comportamiento del servomotor.
 * @details Utiliza el TIMER0 de la placa para generar la señal PWM por hardware.
 *
 * Configuración del TIMER0 a 25 MHz:
 * - MR1: Temporiza el ciclo completo del período PWM (20 ms). El temporizador se reinicia aquí.
 * - MR0: Controla el tiempo en alto (ancho de pulso). El pin MAT0.0 se configura para ponerse en ALTO al reiniciar el ciclo (MR1) y pasar a BAJO al coincidir con MR0.
 *
 * Rango físico del servo estándar:
 * - 0 grados   -> Pulso de 1000 us (1 ms)
 * - 180 grados -> Pulso de 2000 us (2 ms)
 */

/**
 * @brief Bandera que indica si el servo ya llegó a la posición física deseada.
 * @details 0: El servo se está moviendo. 1: El servo llegó y está quieto.
 * El 'main' debe esperar a que esté en 1 para disparar el HC-SR04.
 */
extern volatile Bool SERVO_estabilizado;

typedef enum {
    SENTIDO_HORARIO,
    SENTIDO_ANTIHORARIO
} DIRECCION_GIRO;

typedef struct {
    uint32_t pulsoActual; // Ancho de pulso actual en microsegundos
    uint32_t paso; // Paso de incremento/decremento por ciclo (us por paso)
    DIRECCION_GIRO direccion; // Dirección actual de giro en modo barrido automático
    uint32_t ciclosEsperaPaso; // Tiempo de estabilización configurabilidad para el barrido
} ServoRadar_t;

/**
 * @brief  Inicializa el TIMER0 para generar PWM de 20 ms e interrupción por período.
 * @details Configura MR1 para resetear el timer a los 20 ms y generar interrupción.
 * Configura MR0 para controlar el ancho del pulso (1 ms a 2 ms).
 */
void SERVO_init();

/**
 * @brief  Incrementa o decrementa el ancho de pulso del servo para avanzar según su configuración.
 * @details Modifica el registro de Match (MR0) del TIMER0 controlando que no se excedan
 * los límites físicos. Al llegar a un extremo, conmuta automáticamente la dirección de giro.
 */
void SERVO_step();

/**
 * @brief  Mueve el servomotor a un ángulo específico de forma directa.
 * @details Calcula el ancho de pulso equivalente en microsegundos para el ángulo dado y actualiza el registro MR0. Además, dispara el temporizador de estabilización mecánica (SERVO_estabilizado=0).
 * @param  angulo Ángulo deseado para setear en el servomotor (0 a 180).
 * @return TRUE si el ángulo se seteó con éxito, FALSE si el ángulo estaba fuera de rango.
 */
void SERVO_setAngulo(uint16_t angulo);


/**
 * @brief Fuerza un tiempo de estabilizacion al Servo
 * @details Esto evita caídas de tensión y elimina el temblor del servo
 * @param ciclos Cantidad de ciclos de periodo de PWM de estabilizacion
 */
void SERVO_ForzarEspera(uint32_t ciclos);

#endif /* SERVO_H_ */
