#include "config_global.h"
#include "LPC17xx.h"
#include "LPC17xx_timer.h"
#include "LPC17xx_gpio.h"
#include "servomotor.h"

//la configuracion del Timer0 esta destinada para el pwm del servomotor.

void conf_timer0()
{
	//timer aumenta cada 1 us
	TIM_TIMERCFG_T timerCfg = {TIM_US,1};
	TIM_InitTimer(LPC_TIM0, &timerCfg);

	//el pwm inicia con un angulo de 0grados,el ancho de pulso duro 1ms
	//pone en bajo el pin P1[28] mediante el hardware del match0
	TIM_MATCHCFG_T matchCfg0 = {TIM_MATCH_0,DISABLE,DISABLE,DISABLE,TIM_LOW,1000};
	TIM_ConfigMatch(LPC_TIM0, &matchCfg0);

	//el pwm tiene una frecuencia de 50 hz.
	//pone en alto el pin P1[28] mediante sofware, utilizando interrupciones
	//reinicia el timer
	TIM_MATCHCFG_T matchCfg1 = {TIM_MATCH_1,ENABLE,DISABLE,ENABLE,TIM_NOTHING,20000};
	TIM_ConfigMatch(LPC_TIM0, &matchCfg1);

	NVIC_EnableIRQ(TIMER0_IRQn);
	TIM_Enable(LPC_TIM0);
}

void TIMER0_IRQHandler()
{
	GPIO_SetPins(PORT_1, 1<<28); //pone en "1" el pin P1[28]
	TIM_ClearIntPending(LPC_TIM0, TIM_MATCH_1);//limpia la bandera de interrupcion
}

