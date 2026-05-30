#include "LPC17xx.h"
#include "LPC17xx_systick.h"
#include "LPC17xx_gpio.h"

#define TRIG_PORT 1
#define TRIG_PIN 20

void conf_systick(void)
{


	// 1. Configurar el pin del Trigger como salida
	GPIO_SetDir(TRIG_PORT, 1<<TRIG_PIN, 1);

	// 2. Encender el Trigger (Inicia el pulso)
	GPIO_SetPins(TRIG_PORT, 1<<TRIG_PIN);

	// 3. Configurar interrupciones en el NVIC
	NVIC_EnableIRQ(SysTick_IRQn);
	NVIC_SetPriority(SysTick_IRQn, 3);

	/* 4. Configuración nativa del SysTick para MICROSEGUNDOS
	 * SystemCoreClock es la frecuencia del micro.
	 * Dividido 1,000,000 nos da los ciclos por microsegundo.
	 * Multiplicado por 10 nos da los ciclos necesarios para 10 microsegundos.
	 */
	SysTick->LOAD = ((SystemCoreClock / 1000000) * 10) - 1;
	SysTick->VAL  = 0; // Limpiar el contador actual

	//5. Activo interrupciones por SysTick
	SYSTICK_IntCmd(ENABLE);

	//6. enciendo Systick con CLK interno
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
			         SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(void)
{
	GPIO_ClearPins(TRIG_PORT, 1<<TRIG_PIN);//desactiva el pulso del trigger luego de 10ms
	SYSTICK_Cmd(DISABLE); //Apaga el systick.
	SYSTICK_ClearCounterFlag(); //Limpio bandera
}
