#include "config_global.h"
#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "servo.h"
#include "config.h"

// Size = 1, Src_Width = WORD, Dst_Width = WORD
#define def_control 0 | 1 | (1 << 19) | (1 << 22) | (0 << 27)

GPDMA_LLI_T lli1; // Lectura de la velocidad de procesamiento (LPC_ADC -> ADDR0)
GPDMA_LLI_T lli2; // Lectura de el paso del SERVO (LPC_ADC -> ADDR4)

void DMA_config_CH0(void) {
    GPDMA_Init();

    GPDMA_Channel_CFG_T dmaCfg;
    dmaCfg.channelNum = DMA_canal_ADC;
    dmaCfg.transferSize = 1;
    dmaCfg.type = GPDMA_P2M;
    dmaCfg.srcMemAddr = (uint32_t)&LPC_ADC->ADDR0;
    dmaCfg.dstMemAddr = (uint32_t)&GLOBAL_trimmer_value_step;

    dmaCfg.srcConn = GPDMA_ADC;
    dmaCfg.dstConn = 0;

    /*-----------ENDPOINTS-----------*/
    GPDMA_Endpoint_T endpointSrc;
    endpointSrc.burst = GPDMA_BSIZE_1;
    endpointSrc.width = GPDMA_WORD;
    endpointSrc.increment = DISABLE;

    GPDMA_Endpoint_T endpointDst;
    endpointDst.burst = GPDMA_BSIZE_1;
    endpointDst.width = GPDMA_WORD;
    endpointDst.increment = DISABLE;
    /*-------------------------------*/

    dmaCfg.src = endpointSrc;
    dmaCfg.dst = endpointDst;
    dmaCfg.intErr = DISABLE;
    dmaCfg.intTC = DISABLE;

    //ll1
    lli1.srcAddr = (uint32_t)&LPC_ADC->ADDR0;
    lli1.dstAddr = (uint32_t)&GLOBAL_trimmer_value_step;
    lli1.nextLLI = (uint32_t)&lli2;
    lli1.control = def_control;


    lli2.srcAddr = (uint32_t)&LPC_ADC->ADDR4;
    lli2.dstAddr = (uint32_t)&GLOBAL_trimmer_value_delay;
    lli2.nextLLI = (uint32_t)&lli1;
    lli2.control = def_control;

    /*---------------------------------*/

    dmaCfg.linkedList = (uint32_t)&lli2;

    GPDMA_SetupChannel(&dmaCfg);
    GPDMA_ChannelStart(DMA_canal_ADC);
}


/*
 * El valor del trimmer se interpreta como un ADC de 12 bits:
 * 12 bits permiten representar valores entre 0 y 2^12-1 = 4095.
 *
 * Por lo tanto, se realiza una interpolación lineal entre el mínimo y máximo tiempo de
 * procesamiento usando 4095 como valor máximo de la escala del ADC.
 *
 * La multiplicación se realiza en 64 bits para evitar overflow, ya que dato_crudo y rango
 * son uint32_t y su producto puede superar el límite de 32 bits antes de realizar la división.
 */

uint32_t DMA_get_tiempoProcesamiento(void) {
    uint32_t dato_crudo = (GLOBAL_trimmer_value_delay >> 4) & 0x0FFF;
    uint32_t rango = TRIMMER_MAX_TIEMPO_PROCESAMIENTO - TRIMMER_MIN_TIEMPO_PROCESAMIENTO;

    uint32_t val = TRIMMER_MIN_TIEMPO_PROCESAMIENTO + ((uint64_t)dato_crudo * rango) / 4095;

    //if(val > TRIMMER_MAX_TIEMPO_PROCESAMIENTO) return TRIMMER_MAX_TIEMPO_PROCESAMIENTO;
    //if(val < TRIMMER_MIN_TIEMPO_PROCESAMIENTO) return TRIMMER_MIN_TIEMPO_PROCESAMIENTO;

    return val;
}

uint32_t DMA_get_servoStep() {
    uint32_t dato_crudo = (GLOBAL_trimmer_value_step >> 4) & 0x0FFF;
    uint32_t rango = TRIMMER_MAX_ANGULO_STEP - TRIMMER_MIN_ANGULO_STEP;

    uint32_t val = TRIMMER_MIN_ANGULO_STEP + ((dato_crudo * rango) / 4095);


    SERVO_actualizarPaso(val);
    SERVO_PASO = val;

    //if(val > TRIMMER_MAX_ANGULO_STEP) return TRIMMER_MAX_ANGULO_STEP;
    //if(val < TRIMMER_MIN_ANGULO_STEP) return TRIMMER_MIN_ANGULO_STEP;

    return val;
}
