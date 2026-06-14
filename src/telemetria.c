#include "../inc/telemetria.h"
#include <stdio.h>
#include "uart0.h"
#include "config.h"
#include "lcd.h"

/* ============================================================================ */
/* FUNCIONES PRIVADAS (senders a cada canal) */

/**
 * @brief Envía por UART según el formato
 * @details Formato: "modo,distancia,angulo0,angulo1,distancia_actual,angulo_actual\r\n" ej. "A,123,456,789,012,345\r\n"
 */
static void uart_send(MODO modo, uint16_t distancia, uint16_t angulo0, uint16_t angulo1, uint16_t distancia_actual,
                      uint16_t angulo_actual) {
    char buffer[45];
    snprintf(
        buffer, sizeof(buffer), "%c,%u,%u,%u,%u,%u\r\n",
        (char)modo, distancia, angulo0, angulo1, distancia_actual, angulo_actual
    );

    UART_enviar("%s", buffer);
}

/**
 * @brief Envía por I2C (implementar según tu driver I2C)
 * @details Se envia una secuencia de datos para formar un patron:
 * Ej.
 * Modo A   dis=789
 * a0=132   a1=456
 *
 *  - En 0,0 escribe -> Modo <modo>
 *  - En 0,9 escribe -> dis=<distancia>
 *  - En 1,0 escribe -> a0=<angulo0>
 *  - En 1,9 escribe -> a1=<angulo1>
 */
static void i2c_send(MODO modo, uint16_t distancia, uint16_t angulo0, uint16_t angulo1) {
    char buffer0[10];
    char buffer1[10];
    char buffer2[10];
    char buffer3[10];

    LCD_Clear();

    snprintf(buffer0, sizeof(buffer0), "Modo %c", (char)modo);
    LCD_WRITE(buffer0, 0, 0);

    snprintf(buffer1, sizeof(buffer1), "dis=%u", distancia);
    LCD_WRITE(buffer1, 0, 9);

    snprintf(buffer2, sizeof(buffer2), "a0=%u", angulo0);
    LCD_WRITE(buffer2, 1, 0);

    snprintf(buffer3, sizeof(buffer3), "a1=%u", angulo1);
    LCD_WRITE(buffer3, 1, 9);
}

/**
 * @brief Envía a display 7-segmentos (implementar según tu driver)
 */
static void display_send(MODO modo, uint16_t distancia, uint16_t angulo0, uint16_t angulo1) {
    // TODO: Implementar envío a display 7-segmentos
    // Placeholder: no hace nada por ahora
    (void)modo;
    (void)distancia;
    (void)angulo0;
    (void)angulo1;
}

/* ============================================================================ */

void TELEMETRIA_Actualizar() {
    uart_send(GLOBAL_modo_actual, GLOBAL_distancia, GLOBAL_angulo0, GLOBAL_angulo1, GLOBAL_ultima_distancia,
              GLOBAL_ultimo_angulo);
    i2c_send(GLOBAL_modo_actual, GLOBAL_distancia, GLOBAL_angulo0, GLOBAL_angulo1);
    //display_send(GLOBAL_modo_actual, GLOBAL_distancia, GLOBAL_angulo0, GLOBAL_angulo1);
}

//TODO revisar
void TELEMETRIA_procesarRecibido(const char* data) {
    if (data == NULL) return;

    uint32_t modo = 0;
    uint32_t dist = 0.0, ang0 = 0.0, ang1 = 0.0;

    // Parsear: "modo,distancia,angulo0,angulo1"
    uint32_t parsed = sscanf(data, "%d,%lf,%lf,%lf", &modo, &dist, &ang0, &ang1);

    if (parsed >= 3) {
        // Al menos modo, distancia y angulo0 deben estar presentes
        GLOBAL_modo_actual = (MODO)modo;
        GLOBAL_distancia = dist;
        GLOBAL_angulo0 = ang0;
        if (parsed == 4) {
            GLOBAL_angulo1 = ang1;
        }
    }
}
