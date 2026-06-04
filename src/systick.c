#include "LPC17xx.h"
#include "LPC17xx_systick.h"
#include "LPC17xx_gpio.h"
#include "config.h"
#include "systick.h"

#include <stdio.h>

#include "../drivers/inc/manejo_de_bits.h"

void config_SysTickMs(uint32_t miliSeconds) {
    SYSTICK_InternalInit(miliSeconds);
    SYSTICK_Cmd(ENABLE);
    SYSTICK_IntCmd(ENABLE);
}

/**
 * @brief Realiza el barrido de @ref TECLADO_FILAS_MASK.
 *
 * @note Leer el puerto valor del puerto según la mascara y desplazarlo
 */
void SysTick_Handler() {
    const uint32_t valorActual = GPIO_ReadValue(TECLADO_PORT);

    const uint32_t nuevoValor =
        desplazarMascaraCircular(
            valorActual,
            TECLADO_FILAS_MASK
        );

    //printf("Valor Actual: %u\n", valorActual);
    //printf("Valor Nuevo: %u\n", nuevoValor);


    TECLADO_FILA_ACTUAL = (TECLADO_FILA_ACTUAL + 1) % 4;
    TECLADO_CONTADOR_INTERRUPCIONES_DEBOUNCE++;

    GPIO_WriteValue(TECLADO_PORT, nuevoValor);

    SYSTICK_ClearCounterFlag();
}
