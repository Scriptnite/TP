#include "config_global.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"

int main() {
    PINSEL_ConfigPin(&(PINSEL_CFG_T){PORT_3, PIN_25, PINSEL_TRISTATE, DISABLE});
    GPIO_SetDir(PORT_3, (1 << 25), GPIO_OUTPUT);
    GPIO_SetPins(PORT_3, 1 << 25);

    iniciarTeclado(TECLADO_MS_BARRIDO);
    UART0_config(19200);
    SERVO_init();
    RADAR_Init();
    LCD_Init();

    LCD_Clear();
    LCD_Clear();
    LCD_Clear();
    LCD_Clear();


    LPC_WDT->WDMOD &= ~(1 << 0);

    while (1) {
        ComenzarMedicion();

        for (int i = 0; i < 7000000; i++);
    }
}
