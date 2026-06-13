#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_exti.h"
#include "teclado.h"
#include "config.h"
#include <stdio.h>
#include "lpc17xx_systick.h"
#include "uart0.h"
#include "../drivers/inc/manejo_de_bits.h"

#define filaReal ((uint8_t)((filaActual == 0) ? 3 : (filaActual - 1)))

static void evaluar(char val);
static void cambio_Modo(void);

volatile char valorCargando[3] = {};
static uint8_t buffer_idx = 0;
volatile uint8_t filaActual = 0;
volatile uint8_t contador_interrupciones = 0;

void iniciarTeclado(uint32_t miliSeconds) {
    config_GPIO(TECLADO_PORT, TECLADO_FILAS_MASK);
    config_EINT();

    /* SysTick*/
    config_SysTickMs(miliSeconds);
}

void config_GPIO(LPC_PORT puerto, uint32_t pinMask_INPUT) {
    /* GPIO */
    GPIO_SetDir(puerto, pinMask_INPUT, GPIO_OUTPUT);
    GPIO_WriteValue(puerto, 0x0);

    const LPC_PIN pinSet = (LPC_PIN)buscarPrimerMSB_SET(pinMask_INPUT);
    GPIO_SetPinState(puerto, pinSet, SET);

    /* PINSEL */
    PINSEL_CFG_T configPINSEL;
    configPINSEL.port = TECLADO_PORT;
    configPINSEL.func = PINSEL_FUNC_00;
    configPINSEL.mode = PINSEL_PULLDOWN;
    configPINSEL.openDrain = DISABLE;
    PINSEL_ConfigMultiplePins(&configPINSEL, pinMask_INPUT);
}

void config_EINT() {
    EXTI_Init();
    EXTI_PinConfig(EXTI_EINT0, EXTI_PULLDOWN);
    EXTI_PinConfig(EXTI_EINT1, EXTI_PULLDOWN);
    EXTI_PinConfig(EXTI_EINT2, EXTI_PULLDOWN);
    EXTI_PinConfig(EXTI_EINT3, EXTI_PULLDOWN);

    EXTI_CFG_T configExTI;
    configExTI.mode = EXTI_EDGE_SENSITIVE;
    configExTI.polarity = EXTI_RISING_EDGE;
    configExTI.line = EXTI_EINT0;
    EXTI_ConfigEnable(&configExTI);

    configExTI.line = EXTI_EINT1;
    EXTI_ConfigEnable(&configExTI);

    configExTI.line = EXTI_EINT2;
    EXTI_ConfigEnable(&configExTI);

    configExTI.line = EXTI_EINT3;
    EXTI_ConfigEnable(&configExTI);

    EXTI_EnableIRQ(EXTI_EINT0);
    EXTI_EnableIRQ(EXTI_EINT1);
    EXTI_EnableIRQ(EXTI_EINT2);
    EXTI_EnableIRQ(EXTI_EINT3);
}

/*
 * Columna 1
 * Botones -> {1,4,7,*}
 */
void EINT0_IRQHandler() {
    const char botonPresionado = teclado[filaReal][0];

    if (botonPresionado == '*') {
        GLOBAL_orden_actual = CONFIRMAR;
    }

    evaluar(botonPresionado);

    // Salir
    EXTI_ClearFlag(EXTI_EINT0);
    NVIC_ClearPendingIRQ(EINT0_IRQn);
}

/**
 * Columna 2
 * Botones -> {2,5,8,0}
 */
void EINT1_IRQHandler() {
    const char botonPresionado = teclado[filaReal][1];

    evaluar(botonPresionado);

    // Salir
    EXTI_ClearFlag(EXTI_EINT1);
    NVIC_ClearPendingIRQ(EINT1_IRQn);
}

/**
 * Columna 3
 * Botones -> {3,6,9,#}
 */
void EINT2_IRQHandler() {
    const char botonPresionado = teclado[filaReal][2];

    if (botonPresionado == '#') {
        GLOBAL_orden_actual = CANCELAR;
    }

    evaluar(botonPresionado);

    // Salir
    EXTI_ClearFlag(EXTI_EINT2);
    NVIC_ClearPendingIRQ(EINT2_IRQn);
}

/**
 * Columna 4
 * Botones -> {A,B,C,D}
 *
 * A, B, C -> modos
 * D -> Sin implementar
 */
void EINT3_IRQHandler() {
    const char botonPresionado = teclado[filaReal][3];

    if ((botonPresionado == 'A') || (botonPresionado == 'B') || (botonPresionado == 'C')) {
        GLOBAL_modo_actual = (MODO)botonPresionado;
        GLOBAL_angulo0 = 0;
        GLOBAL_angulo1 = 0;
        GLOBAL_distancia = 0;

        buffer_idx = 0;
        valorCargando[0] = '\0';
        GLOBAL_orden_actual = EN_ESPERA; // Reseteamos la orden

        GLOBAL_estado_actual =
            (GLOBAL_modo_actual == RANGO_DE_ANGULOS)
                ? ESPERANDO_PRIMER_DATO
                : ESPERANDO_UNICO_DATO;

        cambio_Modo();
    }

    EXTI_ClearFlag(EXTI_EINT3);
    NVIC_ClearPendingIRQ(EINT3_IRQn);
}

/**
 * Esta funcion se ejecuta a la hora de cambiar de modo
 */
void cambio_Modo() {
    if (contador_interrupciones >= 10) {
        printf("Modo cambiado a: %c\n", GLOBAL_modo_actual);

        // TODO Disparador de cambio de modo

        contador_interrupciones = 0;
    }
}

void evaluar(char val) {
    if (contador_interrupciones >= TECLADO_CONTADOR_DELAY) {
        //UART_enviar("Boton: %c\r\n", val);


        // Si el carácter es un número, lo acumulamos en el buffer
        if (val >= '0' && val <= '9') {
            if (buffer_idx < 3) valorCargando[buffer_idx++] = val;

            contador_interrupciones = 0;
            return; // Salimos de la función, ya que solo estamos cargando dígitos
        }

        // Si no fue un número, procesamos las órdenes (* o #) según el modo
        switch (GLOBAL_modo_actual) {
            case RANGO_DE_ANGULOS: {
                // === MODO 'A' ===
                switch (GLOBAL_orden_actual) {
                    case CONFIRMAR: {
                        // Se presionó '*'
                        // Convertimos el string acumulado a entero de 8 bits
                        uint16_t temporal = 0;
                        for (int i = 0; i < buffer_idx; i++) {
                            temporal = temporal * 10 + (valorCargando[i] - '0');
                        }

                        if (GLOBAL_estado_actual == ESPERANDO_PRIMER_DATO) {
                            GLOBAL_angulo0 = (uint8_t)temporal;
                            printf("CONFIRMAR | RANGO_DE_ANGULOS | Angulo0 guardado: %d\n", GLOBAL_angulo0);
                            GLOBAL_estado_actual = ESPERANDO_SEGUNDO_DATO;

                            //sprintf(buffer, "CONFIRMAR | RANGO_DE_ANGULOS | Angulo0 guardado: %u\r\n", GLOBAL_angulo0);
                            //UART0_SendString(buffer);
                        }
                        else if (GLOBAL_estado_actual == ESPERANDO_SEGUNDO_DATO) {
                            GLOBAL_angulo1 = (uint8_t)temporal;
                            printf("CONFIRMAR | RANGO_DE_ANGULOS | Angulo1 guardado: %d\n", GLOBAL_angulo1);
                            GLOBAL_estado_actual = ESPERANDO_PRIMER_DATO;

                            //sprintf(buffer, "CONFIRMAR | RANGO_DE_ANGULOS | Angulo1 guardado: %u\r\n", GLOBAL_angulo1);
                            //UART0_SendString(buffer);

                            GLOBAL_estado_sistema = SISTEMA_INICIANDO_MODO; // Trigger
                        }

                        buffer_idx = 0; // Limpia el buffer para la próxima carga
                        GLOBAL_orden_actual = EN_ESPERA; // Resetea la orden procesada
                        break;
                    }
                    case CANCELAR: {
                        // Se presionó '#'
                        buffer_idx = 0; // Limpia el buffer del dato actual
                        GLOBAL_estado_sistema = SISTEMA_CONFIGURANDO;
                        GLOBAL_orden_actual = EN_ESPERA;
                        printf("CANCELAR | RANGO_DE_ANGULOS | Buffer de rango limpiado.\n");
                        break;
                    }
                    default: break;
                }
                break;
            }

            case ANGULO_ESPECIFICO: {
                // === MODO 'B' ===
                switch (GLOBAL_orden_actual) {
                    case CONFIRMAR: {
                        uint16_t temporal = 0;
                        for (int i = 0; i < buffer_idx; i++) {
                            temporal = temporal * 10 + (valorCargando[i] - '0');
                        }
                        GLOBAL_angulo0 = (uint8_t)temporal;
                        printf("CONFIRMAR | ANGULO ESPECÍFICO | Angulo especifico guardado: %d\n", GLOBAL_angulo0);

                        //sprintf(buffer, "CONFIRMAR | ANGULO ESPECÍFICO | Angulo especifico guardado: %u\r\n",GLOBAL_angulo0);
                        //UART0_SendString(buffer);

                        GLOBAL_estado_sistema = SISTEMA_INICIANDO_MODO; // Trigger

                        buffer_idx = 0;
                        GLOBAL_orden_actual = EN_ESPERA;
                        break;
                    }
                    case CANCELAR: {
                        buffer_idx = 0;
                        GLOBAL_orden_actual = EN_ESPERA;
                        printf("CANCELAR | ANGULO ESPECÍFICO | Buffer de angulo especifico limpiado.\n");
                        break;
                    }
                    default: break;
                }
                break;
            }

            case DISTANCIA_ESPECIFICA: {
                // === MODO 'C' ===
                switch (GLOBAL_orden_actual) {
                    case CONFIRMAR: {
                        uint16_t temporal = 0;
                        for (int i = 0; i < buffer_idx; i++) {
                            temporal = temporal * 10 + (valorCargando[i] - '0');
                        }
                        GLOBAL_distancia = (uint8_t)temporal;
                        printf("CONFIRMAR | DISTANCIA ESPECÍFICA | Distancia guardada: %d\n", GLOBAL_distancia);

                        //sprintf(buffer, "CONFIRMAR | DISTANCIA ESPECÍFICA | Distancia guardada: %u\r\n", GLOBAL_distancia);
                        //UART0_SendString(buffer);

                        GLOBAL_estado_sistema = SISTEMA_INICIANDO_MODO; // TRIGGER

                        buffer_idx = 0;
                        GLOBAL_orden_actual = EN_ESPERA;
                        break;
                    }
                    case CANCELAR: {
                        buffer_idx = 0;
                        GLOBAL_orden_actual = EN_ESPERA;
                        printf("CANCELAR | DISTANCIA ESPECÍFICA | Buffer de distancia limpiado.\n");
                        break;
                    }
                    default: break;
                }
                break;
            }
        }

        contador_interrupciones = 0;
    }
}

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

    filaActual = (filaActual + 1) % 4;
    contador_interrupciones++;

    GPIO_WriteValue(TECLADO_PORT, nuevoValor);

    SYSTICK_ClearCounterFlag();
}