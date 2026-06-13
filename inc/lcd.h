#ifndef LCD_I2C_H
#define LCD_I2C_H
#include "LPC17xx.h"

/**
 * @brief Inicializa el módulo LCD.
 * @details Configura los pines del PCF8574, establece el modo de 4 bits y realiza el reset necesario para el controlador HD44780.
 */
void LCD_Init(void);

/**
 * @brief Escribe un string en una posición específica del LCD.
 * @param datos Puntero al arreglo de caracteres (string) a mostrar.
 * @param fila Fila del LCD (normalmente 0 o 1).
 * @param columna Columna donde comenzará a escribirse el texto.
 */
void LCD_WRITE(const char* datos, uint8_t fila, uint8_t columna);

/**
 * @brief Limpia la pantalla completa del LCD.
 * @details Envía el comando de limpieza y mueve el cursor a la posición 0,0.
 */
void LCD_Clear(void);

#endif
