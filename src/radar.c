#include "../inc/radar.h"
#include "config.h"
#include "../inc/servo.h"
#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "../drivers/inc/telemetria.h"

static volatile uint32_t tiempo_subida = 0;
static volatile uint32_t tiempo_bajada = 0;
static volatile uint32_t tiempo_diferencial = 0;
static volatile uint8_t echo_listo = 0;

//static EstadoRadar_t estado_actual = ST_RADAR_ESPERAR_SERVO;

void ComenzarMedicion() {
    GPIO_SetPinState(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, SET);
    TIM_Enable(LPC_TIM1);
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
    TIM_InitTimer(LPC_TIM1, &timerCfg);

    /* Match 0 Trigger (baja el estado del pin) */
    TIM_MATCHCFG_T matchCfg;
    matchCfg.channel = TIM_MATCH_0;
    matchCfg.intEn = ENABLE;
    matchCfg.resetEn = DISABLE;
    matchCfg.stopEn = DISABLE;
    matchCfg.extOpt = TIM_NOTHING;
    matchCfg.matchValue = ULTRA_TRIG_TIME_US - 3; //-3 por correccion de Delay
    TIM_ConfigMatch(LPC_TIM1, &matchCfg);

    /* Capture ECHO */
    TIM_CAPTURECFG_T capCfg;
    capCfg.channel = TIM_CAPTURE_0; // Canal CAP0 del Timer 1
    capCfg.risingEn = ENABLE;
    capCfg.fallingEn = ENABLE;
    capCfg.intEn = ENABLE;
    TIM_ConfigCapture(LPC_TIM1, &capCfg);

    TIM_PinConfig(TIM_CAP1_0_P1_18);

    NVIC_EnableIRQ(TIMER1_IRQn);
    /* ################################################ */
}

/*
void RADAR_Actualizar(void) {
    switch (estado_actual) {

        case ST_RADAR_ESPERAR_SERVO:
            // Condición de Guarda: Esperamos que el paso previo del servo se haya asentado mecánicamente
            if (SERVO_estabilizado == 1) {
                estado_actual = ST_RADAR_DISPARAR_TRIGGER;
            }
            break;

        case ST_RADAR_DISPARAR_TRIGGER:
            echo_listo = 0;
            LPC_TIM1->TC = 0; // Resetear la cuenta del Timer 1 a cero de forma directa

            GPIO_SetPins(US_TRIGGER_PORT, US_TRIGGER_MASK); // Levantar línea de Trigger
            TIM_Cmd(LPC_TIM1, ENABLE);                     // Arrancar Timer 1

            estado_actual = ST_RADAR_ESPERANDO_ECHO;
            break;

        case ST_RADAR_ESPERANDO_ECHO:
            // Espera pasiva y no-bloqueante. El while(1) sigue girando libremente.
            if (echo_listo == 1) {
                TIM_Cmd(LPC_TIM1, DISABLE); // Apagar Timer 1 para ahorrar energía y limpiar estados
                estado_actual = ST_RADAR_PROCESAR_MEDICION;
            }
            break;

        case ST_RADAR_PROCESAR_MEDICION: {
            uint32_t delta_tiempo = 0;

            // Mitigación matemática ante desbordamiento de registros (Aritmética de complemento a 2)
            if (tiempo_bajada >= t_start) {
                delta_tiempo = tiempo_bajada - t_start;
            } else {
                delta_tiempo = (0xFFFFFFFF - t_start) + tiempo_bajada + 1;
            }

            // Conversión estandarizada a centímetros utilizando enteros
            GLOBAL_distancia = (uint16_t)(delta_tiempo / 58);

            // Forzar banderas en '0' antes de gatillar periféricos en segundo plano
            GLOBAL_DMA_UART_completado = 0;
            GLOBAL_I2C_completado = 0;

            estado_actual = ST_RADAR_ENVIAR_REPORTES;
            break;
        }

        case ST_RADAR_ENVIAR_REPORTES:
            // 1. GATILLAR DMA PARA UART0 (Canal 2 - Memoria a Periférico)
            // Aquí llamarías a tu función que configura la ráfaga del DMA de la UART0
            // DMA_DispararTransmisionUART(&GLOBAL_distancia, sizeof(GLOBAL_distancia));

            // 2. GATILLAR TRANSMISIÓN POR INTERRUPCIÓN I2C (Canal No-Bloqueante)
            // Aquí llamarías a tu driver master de I2C que corre por interrupciones independientes
            // I2C_MasterTransferData(LPC_I2C0, &config, I2C_TRANSFER_INTERRUPT);

            // NOTA TEMPORAL PARA PRUEBAS: Si aún no tienes cableados los drivers de comunicación,
            // descomenta estas líneas para simular que ya terminaron instantáneamente:
            GLOBAL_DMA_UART_completado = 1;
            GLOBAL_I2C_completado = 1;

            estado_actual = ST_RADAR_AVANZAR_PASO;
            break;

        case ST_RADAR_AVANZAR_PASO:
            // Condición de guarda de comunicaciones: El servo NO se moverá si el DMA o el I2C están ocupados
            if (GLOBAL_DMA_UART_completado == 1 && GLOBAL_I2C_completado == 1) {

                // Evolucionamos la posición del servo
                SERVO_step();

                // Reiniciamos el lazo de control volviendo al estado 0
                estado_actual = ST_RADAR_ESPERAR_SERVO;
            }
            break;

        default:
            estado_actual = ST_RADAR_ESPERAR_SERVO;
            break;
    }
}
*/

void TIMER1_IRQHandler(void) {
    if (TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT)) {
        // Bajar el pin
        GPIO_SetPinState(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, RESET);

        TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
        NVIC_ClearPendingIRQ(TIMER1_IRQn);

        return;
    }

    const uint32_t tiempo = TIM_GetCaptureValue(LPC_TIM1, TIM_CAPTURE_0);

    if (tiempo_subida == 0) tiempo_subida = tiempo;
    else {
        tiempo_bajada = tiempo;

        tiempo_diferencial = tiempo_bajada - tiempo_subida;

        tiempo_bajada = 0;
        tiempo_subida = 0;
        echo_listo = TRUE;
        GLOBAL_medicion_lista = TRUE;

        GLOBAL_ultima_distancia = tiempo_diferencial / 58;

        tiempo_diferencial = 0;


        //UART_enviar("%u,%u\r\n", GLOBAL_ultimo_angulo, GLOBAL_ultima_distancia);

        TELEMETRIA_Actualizar();

        SERVO_step();
        TIM_Disable(LPC_TIM1);
        TIM_ResetCounter(LPC_TIM1);
    }

    TIM_ClearIntPending(LPC_TIM1, TIM_CR0_INT);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
}
