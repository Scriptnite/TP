#include "LPC17xx.h"
#include "config_global.h"
#include <stdio.h>

void enviarDatosSimulación();

int main() {
    iniciarTeclado(TECLADO_MS_BARRIDO);
    config_UART0(9600);


    while (1);
}

void enviarDatosSimulación() {
    char buffer[50]; // Buffer para formatear el texto

    config_UART0(9600);

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        const uint8_t distancias[] = {
            223, 10, 145, 67, 192, 45, 112, 238, 89, 15, 176, 54, 201, 33, 128, 9, 245, 78, 163, 11,
            199, 42, 137, 85, 22, 156, 94, 210, 58, 184, 12, 119, 71, 230, 39, 167, 88, 205, 47, 142,
            248, 63, 179, 27, 115, 96, 218, 51, 134, 18, 195, 74, 227, 36, 151, 102, 241, 82, 160, 14,
            188, 56, 124, 91, 213, 30, 172, 69, 235, 44, 148, 107, 249, 76, 181, 21, 118, 93, 221, 53,
            131, 25, 202, 80, 233, 38, 159, 105, 244, 17, 187, 61, 127, 98, 215, 48, 170, 73, 239, 32,
            141, 110, 250, 79, 175, 24, 116, 95, 224, 55, 133, 28, 198, 83, 229, 41, 154, 101, 246, 13,
            191, 59, 122, 90, 211, 46, 166, 70, 232, 35, 147, 104, 242, 75, 178, 19, 114, 92, 217, 52,
            130, 26, 204, 87, 226, 37, 158, 100, 240, 16, 185, 60, 126, 97, 214, 49, 169, 72, 237, 31,
            140, 109, 247, 77, 173, 23, 113, 99, 220, 50, 132, 29, 196, 84, 228, 40, 152, 103, 243, 6
        };

        for (uint8_t var = 0; var < 180; ++var) {
            // sprintf(buffer, "%c", distancias[var]);

            sprintf(buffer, "%d,%d\r\n", var, distancias[var]);

            UART0_SendString(buffer);
            for (volatile int i = 0; i < 2000000; i++);
        }

        for (uint8_t var = 180; var > 0; --var) {
            // sprintf(buffer, "%c", distancias[var]);

            sprintf(buffer, "%d,%d\r\n", var, distancias[var]);

            UART0_SendString(buffer);
            for (volatile int i = 0; i < 2000000; i++);
        }
    }
}
