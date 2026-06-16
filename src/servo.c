#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "../inc/servo.h"
#include "../drivers/inc/servo.h"
#include <stdlib.h>
#include "config.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"
#include "LPC17xx_timer.h"

#define anguloActual SERVO_getAnguloDesdePulso(SERVO_MIN_ANGULO, SERVO_MAX_ANGULO, SERVO_MIN_PULSE_uS,  SERVO_MAX_PULSE_uS, servo_radar.pulsoActual)

// Contador interno de ciclos de PWM (20 ms cada uno) para la estabilización mecánica
static volatile uint32_t ciclos_para_estabilizar = 0;
volatile Bool SERVO_estabilizado = FALSE;
static ServoRadar_t servo_radar;
static Bool CompletoBarrido = FALSE;
static Bool CompletoCiclo = TRUE;
static volatile uint32_t pulso_pendiente = 0;

void SERVO_init() {
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
    servo_radar.paso = SERVO_PASO;

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
    NVIC_SetPriority(TIMER0_IRQn, 0);

    TIM_Enable(LPC_TIM0);
    /* ################################################ */

    SERVO_estabilizado = TRUE;
}

void SERVO_step() {
    if (!SERVO_estabilizado) return;

    NVIC_DisableIRQ(TIMER0_IRQn);
    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);

    uint32_t proximo_pulso = servo_radar.pulsoActual;

    switch (GLOBAL_modo_actual) {
        case ANGULO_ESPECIFICO:

            break;
        case DISTANCIA_ESPECIFICA:

            break;
        case RANGO_DE_ANGULOS:

            break;
    }

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
    TIM_ResetCounter(LPC_TIM0);

    servo_radar.pulsoActual = proximo_pulso;
    GLOBAL_ultimo_angulo = anguloActual;
    GLOBAL_ultima_distancia = 0;


    SERVO_estabilizado = FALSE;
    ciclos_para_estabilizar = servo_radar.ciclosEsperaPaso; // 2 ciclos de 20 ms de espera para asentamiento

    NVIC_EnableIRQ(TIMER0_IRQn);
}

void SERVO_setAngulo(uint16_t angulo) {
    if (angulo < SERVO_MIN_ANGULO) angulo = SERVO_MIN_ANGULO;
    if (angulo > SERVO_MAX_ANGULO) angulo = SERVO_MAX_ANGULO;

    if (angulo == anguloActual) {
        SERVO_estabilizado = TRUE;
        return;
    }

    const uint16_t angulo_previo = anguloActual;
    const uint32_t nuevo_pulso = SERVO_getPulsoDesdeAngulo(
        SERVO_MIN_ANGULO, SERVO_MAX_ANGULO, SERVO_MIN_PULSE_uS, SERVO_MAX_PULSE_uS, angulo);

    servo_radar.pulsoActual = nuevo_pulso; // Actualizamos estado lógico

    // --- CÁLCULO DE ESTABILIZACIÓN ---
    const uint16_t delta_angulo = abs(angulo - angulo_previo);
    SERVO_estabilizado = FALSE;
    ciclos_para_estabilizar = 2 + (delta_angulo / 15);

    // --- DOBLE BUFFERING ---
    // En lugar de tocar MR0 o trabar la CPU, solo dejamos el recado.
    // La interrupción se encargará de actualizar MR0 de forma segura.
    pulso_pendiente = nuevo_pulso;
}

/**
 * @brief  Handler de interrupción del TIMER0.
 * @details Se ejecuta automáticamente cada 20 ms (fin de período PWM).
 * Cuenta cuántos ciclos pasaron y, al cumplir el tiempo de tránsito,
 * pone SERVO_estabilizado = 1.
 */
void TIMER0_IRQHandler() {
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)) {
        // El pulso en ALTO terminó. Pasamos el pin a BAJO.
        GPIO_ClearPins(SERVO_SIGNAL_PORT, SERVO_MASK_PIN);
        TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    }

    // Cambiado de "else if" a "if"
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR1_INT)) {
        // --- INICIO EXACTO DEL NUEVO CICLO (TC = 0) ---
        GPIO_SetPins(SERVO_SIGNAL_PORT, SERVO_MASK_PIN);

        // 1. ¿Hay un cambio de ángulo pendiente? Lo aplicamos AQUÍ.
        if (pulso_pendiente != 0) {
            TIM_UpdateMatchValue(LPC_TIM0, TIM_MATCH_0, pulso_pendiente);
            pulso_pendiente = 0; // Borramos el recado
        }

        // 2. Control de Estabilización Mecánica
        if (ciclos_para_estabilizar > 0) {
            ciclos_para_estabilizar--;
            if (ciclos_para_estabilizar == 0) {
                SERVO_estabilizado = TRUE;
            }
        }

        TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
    }

    NVIC_ClearPendingIRQ(TIMER0_IRQn);
}

void SERVO_ForzarEspera(uint32_t ciclos) {
    SERVO_estabilizado = FALSE;
    ciclos_para_estabilizar = ciclos;
}
