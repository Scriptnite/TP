#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "../inc/servo.h"
#include "../drivers/inc/servo.h"
#include "config.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"
#include "LPC17xx_timer.h"

#define anguloActual SERVO_getAnguloDesdePWM(SERVO_MIN_ANGULO, SERVO_MAX_ANGULO, SERVO_MIN_PULSE_uS,  SERVO_MAX_PULSE_uS, servo_radar.pulsoActual)

// Contador interno de ciclos de PWM (20 ms cada uno) para la estabilización mecánica
static volatile uint32_t ciclos_para_estabilizar = 0;
volatile Bool SERVO_estabilizado = FALSE;
static ServoRadar_t servo_radar;

void SERVO_init(void) {
    /* ################################################ */
    /* PINSEL - GPIO */
    PINSEL_CFG_T pinConfig;
    pinConfig.port = SERVO_SIGNAL_PORT;
    pinConfig.pin = SERVO_SIGNAL_PIN;
    pinConfig.func = PINSEL_FUNC_00;
    pinConfig.mode = PINSEL_TRISTATE;
    pinConfig.openDrain = DISABLE;
    PINSEL_ConfigPin(&pinConfig);

    GPIO_SetDir(SERVO_SIGNAL_PORT, SERVO_MASK_PIN, GPIO_OUTPUT);

    GPIO_ClearPins(SERVO_SIGNAL_PORT, SERVO_MASK_PIN);
    /* ################################################ */


    /* ################################################ */
    /* TIMER 0 */
    CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER0, CLKPWR_PCLKSEL_CCLK_DIV_1);

    servo_radar.pulsoActual = SERVO_MIN_PULSE_uS;
    servo_radar.direccion = SENTIDO_HORARIO;
    servo_radar.ciclosEsperaPaso = SERVO_CICLOS_ESTABILIZACION_PASO;
    servo_radar.paso = SERVO_CalcularPaso(
        CLKPWR_GetPCLK(CLKPWR_PCLKSEL_TIMER0),
        TIM_ReadPrescale(LPC_TIM0),
        SERVO_MIN_PULSE_uS,
        SERVO_MAX_PULSE_uS,
        SERVO_MAX_ANGULO - SERVO_MIN_ANGULO,
        1 // 1 grado por paso
    );

    TIM_TIMERCFG_T configTimer;
    configTimer.prescaleOpt = TIM_US;
    configTimer.prescaleValue = 1;
    TIM_InitTimer(LPC_TIM0, &configTimer);

    TIM_MATCHCFG_T matchConfigPulso;
    matchConfigPulso.channel = TIM_MATCH_0;
    matchConfigPulso.extOpt = TIM_NOTHING;
    matchConfigPulso.intEn = ENABLE;
    matchConfigPulso.resetEn = DISABLE;
    matchConfigPulso.stopEn = DISABLE;
    matchConfigPulso.matchValue = servo_radar.pulsoActual;
    TIM_ConfigMatch(LPC_TIM0, &matchConfigPulso);

    TIM_MATCHCFG_T matchConfigPeriodo;
    matchConfigPeriodo.channel = TIM_MATCH_1;
    matchConfigPeriodo.extOpt = TIM_NOTHING;
    matchConfigPeriodo.intEn = ENABLE;
    matchConfigPeriodo.resetEn = ENABLE;
    matchConfigPeriodo.stopEn = DISABLE;
    matchConfigPeriodo.matchValue = SERVO_SIGNAL_PERIODO_uS;
    TIM_ConfigMatch(LPC_TIM0, &matchConfigPeriodo);

    NVIC_EnableIRQ(TIMER0_IRQn);

    TIM_Enable(LPC_TIM0);
    /* ################################################ */

    SERVO_estabilizado = TRUE; // Inicialmente asumimos que está quieto
}

void SERVO_step(void) {
    // Si el servo se está moviendo o asentando, salimos inmediatamente sin modificar registros ni alterar las variables.
    if (!SERVO_estabilizado) return;

    uint32_t proximo_pulso = servo_radar.pulsoActual;

    if (servo_radar.direccion == SENTIDO_HORARIO) {
        if (proximo_pulso + servo_radar.paso <= SERVO_MAX_PULSE_uS) {
            proximo_pulso += servo_radar.paso;
        }
        else {
            servo_radar.direccion = SENTIDO_ANTIHORARIO;
            proximo_pulso -= servo_radar.paso;
        }
    }
    else {
        if (proximo_pulso >= SERVO_MIN_PULSE_uS + servo_radar.paso) {
            proximo_pulso -= servo_radar.paso;
        }
        else {
            servo_radar.direccion = SENTIDO_HORARIO;
            proximo_pulso += servo_radar.paso;
        }
    }

    TIM_UpdateMatchValue(LPC_TIM0, TIM_MATCH_0, proximo_pulso);
    servo_radar.pulsoActual = proximo_pulso;
    GLOBAL_ultimo_angulo = anguloActual;
    GLOBAL_ultima_distancia = 0;


    SERVO_estabilizado = FALSE;
    ciclos_para_estabilizar = servo_radar.ciclosEsperaPaso; // 2 ciclos de 20 ms de espera para asentamiento
}

Status SERVO_setAngulo(uint16_t angle) {
    if (angle > SERVO_MAX_ANGULO) return ERROR;

    const uint16_t angulo_previo = anguloActual;

    if (angle == angulo_previo) {
        SERVO_estabilizado = TRUE;
        return SUCCESS; // Ya está en ese ángulo
    }

    const uint32_t nuevo_pulso =
        SERVO_MIN_PULSE_uS + ((uint32_t)angle * (SERVO_MAX_PULSE_uS - SERVO_MIN_PULSE_uS)) / 180;

    servo_radar.pulsoActual = nuevo_pulso;
    TIM_UpdateMatchValue(LPC_TIM0, TIM_MATCH_0, nuevo_pulso);

    // --- CÁLCULO DEL TIEMPO DE ESTABILIZACIÓN MECÁNICA ---
    // Un servo estándar tarda aproximadamente 4 ms por cada 10 grados en moverse.
    // Para ser seguros, calculamos cuántos ciclos de PWM (20 ms) necesita esperar la CPU.
    const uint16_t delta_angulo =
        (angle > angulo_previo)
            ? (angle - angulo_previo)
            : (angulo_previo - angle);

    SERVO_estabilizado = FALSE;
    // Fórmula empírica segura: Mínimo 2 ciclos (40 ms) + 1 ciclo extra por cada 15 grados de giro
    ciclos_para_estabilizar = 2 + (delta_angulo / 15);

    return SUCCESS;
}

/**
 * @brief  Handler de interrupción del TIMER0.
 * @details Se ejecuta automáticamente cada 20 ms (fin de período PWM).
 * Cuenta cuántos ciclos pasaron y, al cumplir el tiempo de tránsito,
 * pone SERVO_estabilizado = 1.
 */
void TIMER0_IRQHandler(void) {
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)) {
        // El pulso en ALTO terminó. Pasamos el pin a BAJO.
        GPIO_ClearPins(SERVO_SIGNAL_PORT, SERVO_MASK_PIN);
        TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    }
    else {
        // El período completo de 20 ms terminó. Pasamos el pin a ALTO.
        GPIO_SetPins(SERVO_SIGNAL_PORT, SERVO_MASK_PIN);

        // --- CONTROL DE ESTABILIZACIÓN MECÁNICA ---
        // Se ejecuta exactamente una vez cada 20 ms (al finalizar el ciclo de PWM)
        if (ciclos_para_estabilizar > 0) {
            ciclos_para_estabilizar--;
            if (ciclos_para_estabilizar == 0) {
                SERVO_estabilizado = TRUE;

                //TODO Trigger de medicion del Ultrasonido
            }
        }

        TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
    }

    NVIC_ClearPendingIRQ(TIMER0_IRQn);
}
