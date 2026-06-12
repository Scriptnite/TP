#include "config_global.h"
#include "LPC17xx.h"
#include "LPC17xx_timer.h"
#include "LPC17xx_gpio.h"
#include "LPC17xx_pinsel.h"
#include "ultrasonido.h"
#include <stdio.h>
#include <stdbool.h>



/*
 La configuracion del Timer1 esta destinada para el trigger del sensor ultrasonico, utilizando el modo de captura.
 Los valores capturados por el modo capture, seran trasladados por el DMA a una direccion de memoria
donde seran procesados.
*/
void config_TMR1(void)
{
	//Configuro el pin del capture1[1]
		PINSEL_CFG_T pinCfg = {
				.port = PORT_1,
				.pin = PIN_19,
				.func = PINSEL_FUNC_11 ,
				.mode = PINSEL_PULLDOWN,
				.openDrain = DISABLE
		};
		PINSEL_ConfigPin(&pinCfg);

	TIM_TIMERCFG_T timerCfg = {TIM_US, 1}; //el timer aumenta cada 1us
    TIM_InitTimer(LPC_TIM1, &timerCfg);

    TIM_PinConfig(TIM_CAP1_1_P1_19); //Se configura el pin P1[19] para tomar los pulsos de echo medianto el modo capture

    TIM_CAPTURECFG_T capCfg = {
    		.channel = TIM_CAPTURE_1,
    		.fallingEn = ENABLE,
			.risingEn = ENABLE,
			.intEn = ENABLE
    };
    TIM_ConfigCapture(LPC_TIM1, &capCfg); //Se configura el capture1[1]
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_SetPriority(TIMER1_IRQn, 2);

    TIM_Enable(LPC_TIM1);
}

void TIMER1_IRQHandler()
{
	static uint32_t risingEdgeValue;
	if ((GPIO_ReadValue(PORT_1) & (1 << 19))) //identifica si se disparo la interrupcion por flanco de subida o de bajada
	{
		risingEdgeValue = TIM_GetCaptureValue(LPC_TIM1, TIM_CAPTURE_1);
		ULTRASONIDO_setDataisReady(false);
	}
	else
	{
		ULTRASONIDO_calculateDistance(TIM_GetCaptureValue(LPC_TIM1, TIM_CAPTURE_1) - risingEdgeValue);
		ULTRASONIDO_setDataisReady(true);
	}
	TIM_ClearIntPending(LPC_TIM1, TIM_CR1_INT);
}
