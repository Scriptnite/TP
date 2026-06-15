#include "config_global.h"
#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "config.h"

void config_DMA_CH0()
{
    GPDMA_Init();
    
    GPDMA_Channel_CFG_T dmaCfg;
    dmaCfg.channelNum = GPDMA_CH_0;
    dmaCfg.transferSize = 1;
    dmaCfg.type = GPDMA_P2M;
    dmaCfg.srcMemAddr = (uint32_t) &(LPC_ADC->ADDR0);
    dmaCfg.dstMemAddr = (uint32_t) &GLOBAL_lectura_channel0;
    dmaCfg.srcConn = GPDMA_ADC;

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

    /*-----------LINKEDLIST-----------*/
    static GPDMA_LLI_T lli1; //ADDR0 --> GLOBAL_lectura_channel0
    static GPDMA_LLI_T lli2; //ADDR1 --> GLOBAL_lectura_channel1

    //ll1
    lli1.srcAddr = (uint32_t) &(LPC_ADC->ADDR0);
    lli1.dstAddr = (uint32_t) &GLOBAL_lectura_channel0;
    lli1.nextLLI = (uint32_t) &lli2;
    lli1.control = 0x00000000
    			 |1        //transferSize = 1
				 |1u<<19   //SWidth = WORD
				 |1u<<22;  //DWifth = WORD

    //lli2
    lli2.srcAddr = (uint32_t) &(LPC_ADC->ADDR1);
    lli2.dstAddr = (uint32_t) &GLOBAL_lectura_channel1;
    lli2.nextLLI = (uint32_t) &lli1;
    lli2.control = 0x00000000
    			 |1        //transferSize = 1
				 |1u<<19   //SWidth = WORD
				 |1u<<22;  //DWifth = WORD
    /*---------------------------------*/
    dmaCfg.linkedList = (uint32_t) &lli2;

    GPDMA_SetupChannel(&dmaCfg);

    GPDMA_ChannelStart(GPDMA_CH_0);
}


uint32_t get_tiempoProcesamiento(void)
{
	uint32_t dato_crudo = (GLOBAL_lectura_channel0 >> 4) & 0x0FFF;

	// Calculamos el rango de forma dinámica usando tus defines
	uint32_t rango = TRIMMER_MAX_TIEMPO_PROCESAMIENTO - TRIMMER_MIN_TIEMPO_PROCESAMIENTO;

	// Aplicamos interpolación con uint64_t para evitar overflow
	uint32_t delay_calculado = TRIMMER_MIN_TIEMPO_PROCESAMIENTO + (uint32_t)(((uint64_t)dato_crudo * (uint64_t)rango) / 4095ULL);

	return delay_calculado;
}
uint32_t get_servoStep(void)
{
	uint32_t dato_crudo = (GLOBAL_lectura_channel1 >> 4) & 0x0FFF;

	// Rango dinámico
	uint32_t rango = TRIMMER_MAX_ANGULO_STEP - TRIMMER_MIN_ANGULO_STEP;

    // Acá no hace falta uint64_t porque los números son muy chicos
    uint32_t paso_calculado = TRIMMER_MIN_ANGULO_STEP + ((dato_crudo * rango) / 4095);

    return paso_calculado;
}
