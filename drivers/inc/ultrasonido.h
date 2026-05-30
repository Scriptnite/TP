#ifndef ULTRASONIDO_H_
#define ULTRASONIDO_H_

/**
 * @brief pines que utiliza el sensor ultrasonico
 */
#define TRIG_PORT 1
#define TRIG_PIN 20

/**
 * @brief  envia un pulso al pin trigger del sensor ultrasonico
 * @param  Ninguno.
 * @return Ninguno.
 * @note   Este metodo utiliza systick para poder contar los 10us del ancho de pulso. Asegurese de tener
 * el systick configurado y apagado antes de utilizar este metodo
 */
void sendTriggerPulse(void);

#endif /*ULTRASONIDO_H_ */
