#include "lcd.h"
#include "config.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

// Define el estado de los bits de control (P3=1 para luz encendida)
#define LCD_BACKLIGHT 0x08  // Backlight ON
#define LCD_ENABLE    0x04
#define LCD_RS        0x01

typedef enum {
    COMANDO = 0x00,
    DATO = LCD_RS
} MODE;

/* ============================================================================ */
/**
 * @brief Envía un único byte de datos a un esclavo I2C mediante polling.
 * @details Configura la estructura I2C_M_SETUP_Type y utiliza la API de LpcOpen para realizar la transmisión bloqueante.
 * @param address Dirección de 7 bits del periférico esclavo.
 * @param data Byte de datos que se desea transmitir.
 */
static void I2C_Transmit(const uint8_t address, const uint8_t data) {
    uint8_t txbuf[1] = {data}; // Buffer de transmisión

    I2C_M_SETUP_Type txsetup;
    txsetup.sl_addr7bit = address; // Dirección del esclavo
    txsetup.tx_data = txbuf; // Buffer de datos a transmitir
    txsetup.tx_length = 1; // 1 byte de TX
    txsetup.rx_data = NULL; // No se reciben datos
    txsetup.rx_length = 0; // 0 bytes de RX

    // Reintentos ante error/NACK
    txsetup.retransmissions_max = 0;

    // Transferencia en modo polling
    I2C_MasterTransferData(LPC_I2C0, &txsetup, I2C_TRANSFER_POLLING);
}

/**
 * @brief Genera el pulso de habilitación (Enable) necesario para que el LCD capture los datos.
 * @details Cambia el estado del pin E de alto a bajo, manteniendo los niveles de datos y control estables.
 * Incluye retardos de estabilización para cumplir con los tiempos mínimos del controlador HD44780.
 * @param data El byte de estado actual (datos + flags de control).
 */
static void lcd_i2c_PulseEnable(uint8_t data) {
    I2C_Transmit(LCD_SLAVE_ADDRESS, data | LCD_ENABLE | LCD_BACKLIGHT);
    for (int i = 0; i < 1000; i++) {} // Delay de estabilización

    I2C_Transmit(LCD_SLAVE_ADDRESS, (data & ~LCD_ENABLE) | LCD_BACKLIGHT);
    for (int i = 0; i < 1000; i++) {} // Delay para procesamiento interno
}

/**
 * @brief Prepara y envía un nibble (4 bits) a través del adaptador I2C.
 * @details Combina los bits de datos con las señales de control (RS, Backlight) y dispara el pulso de habilitación.
 * @param nibble Los 4 bits de datos a enviar (se esperan en los bits superiores del byte).
 * @param mode Modo de operación: 0 para comandos, LCD_RS para datos.
 */
static void lcd_i2c_SendNibble(uint8_t nibble, MODE mode) {
    const uint8_t data = (nibble & 0xF0) | mode | LCD_BACKLIGHT;
    I2C_Transmit(LCD_SLAVE_ADDRESS, data);
    lcd_i2c_PulseEnable(data);
}

/**
 * @brief Divide un byte en dos nibbles para enviarlo al LCD en modo 4 bits.
 * @details Envía primero los 4 bits más significativos (MSB) y luego los 4 bits menos significativos (LSB) mediante llamadas sucesivas a SendNibble.
 * @param byte El byte completo (8 bits) a transmitir.
 * @param mode Modo de operación: COMANDOS, DATOS
 */
static void lcd_i2c_SendByte(uint8_t byte, MODE mode) {
    lcd_i2c_SendNibble(byte & 0xF0, mode); // Envío del nibble alto
    lcd_i2c_SendNibble((byte << 4) & 0xF0, mode); // Envío del nibble bajo
}

/* ============================================================================ */

void LCD_Init() {
    PINSEL_CFG_T pinCfg;

    // SDA0 -> P0.27
    pinCfg.port = PORT_0;
    pinCfg.pin = PIN_27;
    pinCfg.func = PINSEL_FUNC_01;
    pinCfg.mode = PINSEL_PULLUP;
    pinCfg.openDrain = ENABLE;
    PINSEL_ConfigPin(&pinCfg);

    // SCL0 -> P0.28
    pinCfg.pin = PIN_28;
    PINSEL_ConfigPin(&pinCfg);


    I2C_Cmd(LPC_I2C0, DISABLE);
    I2C_Init(LPC_I2C0, 100000);
    I2C_Cmd(LPC_I2C0, ENABLE);


    // Delay inicial luego del encendido del LCD
    for (int i = 0; i < 5000000; i++) {}


    // Secuencia de sincronización en modo 8 bits
    lcd_i2c_SendNibble(0x30, COMANDO);
    for (int i = 0; i < 100000; i++) {}

    lcd_i2c_SendNibble(0x30, COMANDO);
    for (int i = 0; i < 100000; i++) {}

    lcd_i2c_SendNibble(0x30, COMANDO);
    for (int i = 0; i < 100000; i++) {}

    // Cambio definitivo a modo 4 bits
    lcd_i2c_SendNibble(0x20, COMANDO);

    /* ################################################ */
    /* Configuración */
    lcd_i2c_SendByte(0x28, COMANDO); // Function set: 4 bits, 2 líneas, 5x8
    lcd_i2c_SendByte(0x0C, COMANDO); // Display ON, cursor OFF, blink OFF
    LCD_Clear(); // Limpia pantalla
    lcd_i2c_SendByte(0x06, COMANDO); // Incremento automático del cursor

    LCD_Clear(); // Limpia pantalla
}

void LCD_WRITE(const char* datos, uint8_t fila, uint8_t columna) {
    const uint8_t dir = 0x80 + (fila * 0x40) + columna;
    lcd_i2c_SendByte(dir, COMANDO); // 0 = Modo Comando

    while (*datos) {
        lcd_i2c_SendByte((uint8_t)(*datos++), DATO); // LCD_RS = Modo Dato
    }
}

void LCD_Clear() {
    lcd_i2c_SendByte(0x01, COMANDO);
    for (int i = 0; i < 100000; i++);
}
