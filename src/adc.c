#include "adc.h"
#include "LPC17xx.h"
#include "lpc17xx_adc.h"

void ADC_config(void) {
    ADC_Init(100000); // ADC a 100 KHz
    ADC_PowerUp();

    ADC_PinConfig(ADC_CHANNEL_0); // P0.23
    ADC_PinConfig(ADC_CHANNEL_4); // P1.30

    ADC_ChannelEnable(ADC_CHANNEL_0);
    ADC_ChannelEnable(ADC_CHANNEL_4);

    ADC_IntEnable(ADC_INT_CH0);
    ADC_IntEnable(ADC_INT_CH4);

    ADC_BurstEnable();
}
