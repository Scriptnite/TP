#ifndef TP_ADC_H
#define TP_ADC_H


/**
 * @brief Configura al ADC en modo burst, habilitando CH0 y CH1.
 * @details Funciona con dos canales CH0 y CH1. Ambos convierten a maxima velocidad en modo burst.
 * - CH0 -> P0.23 seteara la velocidad de procesamiento
 * - CH1 -> P0.24 seteara paso del servo
 */
void ADC_config();

#endif
