#include "config_global.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

PINSEL_CFG_T LedRojo = (PINSEL_CFG_T){PORT_0, PIN_22, PINSEL_FUNC_00, PINSEL_PULLDOWN, DISABLE};
PINSEL_CFG_T LedVerde = (PINSEL_CFG_T){PORT_3, PIN_25, PINSEL_FUNC_00, PINSEL_PULLDOWN, DISABLE};
PINSEL_CFG_T LedAzul = (PINSEL_CFG_T){PORT_3, PIN_26, PINSEL_FUNC_00, PINSEL_PULLDOWN, DISABLE};

int main() {
    PINSEL_ConfigPin(&LedRojo);
    PINSEL_ConfigPin(&LedVerde);
    PINSEL_ConfigPin(&LedAzul);
    GPIO_SetDir(PORT_0, 0 | (1 << PIN_22), GPIO_OUTPUT);
    GPIO_SetDir(PORT_3, 0 | (1 << PIN_25), GPIO_OUTPUT);
    GPIO_SetDir(PORT_3, 0 | (1 << PIN_26), GPIO_OUTPUT);
    GPIO_ClearPins(PORT_0, 1 << PIN_22);
    GPIO_SetPins(PORT_3, 1 << PIN_25);
    GPIO_ClearPins(PORT_3, 1 << PIN_26);

    iniciarTeclado(TECLADO_MS_BARRIDO);
    UART0_config(19200);
    SERVO_init();
    RADAR_Init();
    LCD_Init();


    //LPC_WDT->WDMOD &= ~(1 << 0);


    while (1) {
        RADAR_Actualizar();

        //ComenzarMedicion();

        //	SERVO_step();

        for (int i = 0; i < 1000000; i++) {}
    }
}
