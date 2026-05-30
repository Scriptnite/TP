#include "LPC17xx.h"
#include "LPC17xx_gpio.h"
#include "LPC17xx_systick.h"

void sendTriggerPulse()
{
	// Se configurar el tiempo de recarga (Por si se borró o modificó)
	SysTick->LOAD = ((SystemCoreClock / 1000000) * 10) - 1;
	//Se reinicia el valor del Systick
	SysTick->VAL = 0;
	//Se activa el trigger
	GPIO_SetPins(TRIG_PORT, 1<<TRIG_PIN);
	//Se enciende el Systick
	SYSTICK_Cmd(ENABLE);
}
