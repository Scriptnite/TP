#include "LPC17xx.h"
#include "LPC17xx_gpio.h"
#include "LPC17xx_systick.h"
#include "ultrasonido.h"
#include "config_global.h"
#include <stdbool.h>

static volatile double distance;//distancia medida por el sensor de ultraSonido
static volatile bool data_ready = false;//indica que si hay una nueva medida de distancia por el sensor de ultrasonido

void ULTRASONIDO_sendTriggerPulse(void)
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


double ULTRASONIDO_getDistance(void)
{
	return distance;
}

bool ULTRASONIDO_isDataReady(void)
{
    return data_ready;
}

void ULTRASONIDO_setDataisReady(bool ready)
{
	data_ready = ready;
}

void ULTRASONIDO_calculateDistance(uint32_t time)
{
	distance = (double)time / 58.2;
}
