#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpdma.h"

extern void UART0_SendString(const char* str);
void UART0_SendString_Variables(char* str);

static GPDMA_Channel_CFG_T dmaCfg;

void UART0_config(uint32_t baudRate) {
    if (baudRate == 0) baudRate = 9600;

    UART_CFG_T uart_cfg;
    uart_cfg.baudRate = baudRate;
    uart_cfg.dataBits = UART_DBITS_8;
    uart_cfg.parity = UART_PARITY_NONE;
    uart_cfg.stopBits = UART_STOPBIT_1;

    UART_Init(UART0, &uart_cfg);

    UART_PinConfig(UART_TX0_P0_2);
    UART_PinConfig(UART_RX0_P0_3);

    UART_TxEnable(UART0);
}

void UART0_config_dma(uint32_t baudRate) {
    UART0_config(baudRate);

    UART_FIFO_CFG_T uart_fifo_cfg;
    uart_fifo_cfg.resetRxBuf = ENABLE; // Vacía el FIFO RX antes de empezar
    uart_fifo_cfg.resetTxBuf = ENABLE; // Vacía el FIFO TX antes de empezar
    uart_fifo_cfg.dmaMode = ENABLE; //Habilita el trigger por Tx del UART0
    uart_fifo_cfg.level = UART_FIFO_TRGLEV0; // Solo para RX
    UART_FIFOConfig(UART0, &uart_fifo_cfg);

    GPDMA_Init();


    dmaCfg.channelNum = DMA_canal_UART0;
    dmaCfg.transferSize = UART_BUFFER_SIZE;
    dmaCfg.type = GPDMA_M2P;
    dmaCfg.srcMemAddr = (uint32_t)bufferUART;
    dmaCfg.dstMemAddr = (uint32_t)&UART0->THR;

    dmaCfg.srcConn = 0;
    dmaCfg.dstConn = GPDMA_UART0_Tx;

    /*-----------ENDPOINTS-----------*/
    GPDMA_Endpoint_T endpointSrc;
    endpointSrc.burst = GPDMA_BSIZE_1;
    endpointSrc.width = GPDMA_BYTE;
    endpointSrc.increment = ENABLE;

    GPDMA_Endpoint_T endpointDst;
    endpointDst.burst = GPDMA_BSIZE_1;
    endpointDst.width = GPDMA_BYTE;
    endpointDst.increment = DISABLE;
    /*-------------------------------*/

    dmaCfg.src = endpointSrc;
    dmaCfg.dst = endpointDst;
    dmaCfg.intErr = DISABLE;
    dmaCfg.intTC = DISABLE;

    /*---------------------------------*/

    dmaCfg.linkedList = 0;

    GPDMA_SetupChannel(&dmaCfg);
}

void UART0_iniciarTX(uint32_t longitud) {
    dmaCfg.transferSize = longitud;
    GPDMA_SetupChannel(&dmaCfg);

    GPDMA_ChannelStart(DMA_canal_UART0);
}

void UART_enviar(const char* format, ...) {
    char buffer[UART_BUFFER_SIZE];

    va_list args;

    // Inicializa la lista de argumentos variables
    va_start(args, format);

    // vsnprintf toma el formato y los argumentos variables (...),
    // los procesa y los guarda de manera segura dentro de 'buffer'
    // sin pasarse del tamaño límite (sizeof(buffer)).
    const int longitud = vsnprintf(buffer, sizeof(buffer), format, args);

    // Limpia la memoria de la lista de argumentos
    va_end(args);

    // Si el formateo tuvo éxito y generó texto, lo enviamos
    if (longitud > 0) {
        UART0_SendString_Variables(buffer);
    }
}


void UART0_SendString_Variables(char* str) {
    UART_Send(
        UART0,
        (uint8_t *)str,
        strlen(str),
        BLOCKING
    );
}

