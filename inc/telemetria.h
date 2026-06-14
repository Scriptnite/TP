#ifndef TELEMETRIA_H_
#define TELEMETRIA_H_

/**
 * @brief Envía el estado actual de las variables globales a UART, I2C y display
 */
void TELEMETRIA_Actualizar(void);

/**
 * @brief Procesa una línea recibida por UART (desde ISR RX)
 * @param data Cadena con formato "modo,distancia,angulo0,angulo1" (Ej. "A,150,30,0")
 * @details Parsea la cadena y actualiza las variables globales GLOBAL_xxxx
 */
void TELEMETRIA_procesarRecibido(const char* data);

#endif
