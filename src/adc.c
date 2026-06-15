#include "LPC17xx.h"
#include "config_global.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpio.h"


//El adc funciona con dos canales CH0 y CH1. Ambos convierten a maxima velocidad en modo burst
//CH0 --> P0[23] seteara la velocidad de procesamiento
//CH1 --> P0[24] seteara paso del servo
void conf_adc(void)
{
	ADC_Init(100000); //ADC a una velocidad de 100Khz
	ADC_PinConfig(ADC_CHANNEL_0); //P0[23]
	ADC_PinConfig(ADC_CHANNEL_1); //P0[24]

	ADC_ChannelEnable(ADC_CHANNEL_0);
	ADC_ChannelEnable(ADC_CHANNEL_1);

	ADC_BurstEnable();

	ADC_PowerUp();
}
