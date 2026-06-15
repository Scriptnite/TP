#include "config_global.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_uart.h"

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

/*   TESTING ADC-DMA Y CONVERSION DE DATOS
int main(void) {
    UART0_config(0);

    // Inicializamos lo que armamos hoy
    conf_adc();
    config_DMA_CH0();

    while(1) {
        // 1. Llamamos a nuestros métodos GET (Matemática aplicada)
        uint32_t delay_actual = get_tiempoProcesamiento();
        uint32_t paso_actual = get_servoStep();

        // 2. Rescatamos los valores crudos del ADC (0 a 4095) solo para ver qué lee
        uint32_t crudo_ch0 = (GLOBAL_lectura_channel0 >> 4) & 0x0FFF;
        uint32_t crudo_ch1 = (GLOBAL_lectura_channel1 >> 4) & 0x0FFF;

        // 3. Enviamos a la terminal DIRECTAMENTE con tu función
        UART_enviar("CH0 (Crudo): %u -> Delay: %u | CH1 (Crudo): %u -> Paso: %u\r\n",
                    crudo_ch0, delay_actual, crudo_ch1, paso_actual);

        // 4. Delay bloqueante EXCLUSIVO PARA TESTING
        for(volatile uint32_t i = 0; i < 10000000; i++);
    }

    return 0;
}
*/
