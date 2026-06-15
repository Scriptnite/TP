#include "../inc/radar.h"
#include "config.h"
#include "../inc/servo.h"
#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "../inc/telemetria.h"
#include <stdio.h>

static volatile uint32_t tiempo_subida = 0;
static volatile uint32_t tiempo_bajada = 0;
static volatile uint32_t tiempo_diferencial = 0;
static volatile Bool echo_listo = FALSE;
static volatile Bool medicion_en_progreso = FALSE;

static EstadoRadar_t estado_actual = ST_RADAR_ESPERAR_SERVO;


void ComenzarMedicion() {
    if (!SERVO_estabilizado | medicion_en_progreso) return;

    medicion_en_progreso = TRUE;
    echo_listo = FALSE;

    TIM_ResetCounter(LPC_TIM2);
    TIM_ClearIntPending(LPC_TIM2, TIM_MR0_INT);
    TIM_ClearIntPending(LPC_TIM2, TIM_CR1_INT);

    GPIO_SetPinState(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, SET);
    TIM_Enable(LPC_TIM2);
}

void RADAR_Init(void) {
    /* ################################################ */
    /* PIN trigger */
    PINSEL_CFG_T pinCfg;
    pinCfg.func = PINSEL_FUNC_00;
    pinCfg.mode = PINSEL_TRISTATE;
    pinCfg.openDrain = DISABLE;
    pinCfg.pin = ULTRA_TRIG_PIN;
    pinCfg.port = ULTRA_TRIG_PORT;
    PINSEL_ConfigPin(&pinCfg);

    GPIO_SetDir(ULTRA_TRIG_PORT, ULTRA_TRIG_MASK, GPIO_OUTPUT);
    GPIO_SetPinState(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, RESET);
    /* ################################################ */

    /* ################################################ */
    CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER1, CLKPWR_PCLKSEL_CCLK_DIV_1);

    /* Timer 1 */
    TIM_TIMERCFG_T timerCfg;
    timerCfg.prescaleOpt = TIM_US;
    timerCfg.prescaleValue = 1;
    TIM_InitTimer(LPC_TIM2, &timerCfg);

    /* Match 0 Trigger (baja el estado del pin) */
    TIM_MATCHCFG_T matchCfg;
    matchCfg.channel = TIM_MATCH_0;
    matchCfg.intEn = ENABLE;
    matchCfg.resetEn = DISABLE;
    matchCfg.stopEn = DISABLE;
    matchCfg.extOpt = TIM_NOTHING;
    matchCfg.matchValue = ULTRA_TRIG_TIME_US - 2; //-3 por correccion de Delay
    TIM_ConfigMatch(LPC_TIM2, &matchCfg);

    /* Capture ECHO */
    TIM_CAPTURECFG_T capCfg;
    capCfg.channel = TIM_CAPTURE_1; // Canal CAP0 del Timer 2
    capCfg.risingEn = ENABLE;
    capCfg.fallingEn = ENABLE;
    capCfg.intEn = ENABLE;
    TIM_ConfigCapture(LPC_TIM2, &capCfg);

    TIM_PinConfig(TIM_CAP2_1_P0_5);

    NVIC_EnableIRQ(TIMER2_IRQn);
    NVIC_SetPriority(TIMER2_IRQn, 1);
    /* ################################################ */
}

void TIMER2_IRQHandler(void) {
    if (TIM_GetIntStatus(LPC_TIM2, TIM_MR0_INT)) {
        GPIO_SetPinState(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, RESET);
        TIM_ClearIntPending(LPC_TIM2, TIM_MR0_INT);
        NVIC_ClearPendingIRQ(TIMER2_IRQn);
        return;
    }

    const uint32_t tiempo = TIM_GetCaptureValue(LPC_TIM2, TIM_CAPTURE_1);

    if (tiempo_subida == 0) {
        tiempo_subida = tiempo;
    }
    else {
        tiempo_bajada = tiempo;
        tiempo_diferencial = tiempo_bajada - tiempo_subida;

        tiempo_bajada = 0;
        tiempo_subida = 0;

        // Almacenamos la distancia y avisamos al main
        GLOBAL_ultima_distancia = tiempo_diferencial / 58;
        GLOBAL_medicion_lista = TRUE;

        // Apagar timer para evitar re-disparos molestos
        TIM_Disable(LPC_TIM2);
        TIM_ResetCounter(LPC_TIM2);
        medicion_en_progreso = 0;

        TELEMETRIA_Actualizar();
    }

    TIM_ClearIntPending(LPC_TIM2, TIM_CR1_INT);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);
}


void RADAR_Actualizar(void) {
    switch (estado_actual) {

        case ST_RADAR_ESPERAR_SERVO:
            // Condición de Guarda: Esperamos a que el servo esté 100% quieto
            if (SERVO_estabilizado == TRUE) {
                estado_actual = ST_RADAR_DISPARAR_TRIGGER;
            }
            break;

        case ST_RADAR_DISPARAR_TRIGGER:
            // Disparamos la medición de forma limpia.
            // Como aquí SERVO_estabilizado es TRUE y medicion_en_progreso es FALSE, se ejecutará perfecto.
            ComenzarMedicion();

            // Pasamos directo a esperar el fin del pulso por hardware
            estado_actual = ST_RADAR_ESPERANDO_ECHO;
            break;

        case ST_RADAR_ESPERANDO_ECHO:
            // Espera pasiva y no-bloqueante. El while(1) gira libremente.
            // La interrupción TIMER1_IRQHandler se encargará de levantar esta bandera.
            if (GLOBAL_medicion_lista == TRUE) {
                GLOBAL_medicion_lista = FALSE; // Bajamos la bandera inmediatamente
                estado_actual = ST_RADAR_PROCESAR_MEDICION;
            }
            break;

        case ST_RADAR_PROCESAR_MEDICION:
            // Procesamos los datos en el contexto del main, de forma segura
            TELEMETRIA_Actualizar();

            // Podés agregar aquí el formateo de tu LCD o reportes si querés
            estado_actual = ST_RADAR_AVANZAR_PASO;
            break;

        case ST_RADAR_AVANZAR_PASO:
            // Movemos el servo un paso hacia la siguiente posición.
            // Esto internamente pone SERVO_estabilizado = FALSE y configura los ciclos de espera.
            SERVO_step();

            // Reiniciamos el lazo volviendo al estado 0.
            // En la próxima vuelta del while(1), la máquina se quedará atrapada en ST_RADAR_ESPERAR_SERVO
            // durante 40ms hasta que el timer del servo le dé el OK.
            estado_actual = ST_RADAR_ESPERAR_SERVO;
            break;

        default:
            estado_actual = ST_RADAR_ESPERAR_SERVO;
            break;
    }
}
