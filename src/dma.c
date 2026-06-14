#include "config_global.h"
#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "config.h"

void config_DMA_CH0()
{
    GPDMA_Init();
    
    GPDMA_Channel_CFG_T dmaCfg;
    dmaCfg.channelNum = GPDMA_CH_0;
    dmaCfg.transferSize = 2;
    dmaCfg.type = GPDMA_P2M;
    dmaCfg.srcMemAddr = (uint32_t) &(LPC_ADC->ADGDR);
    dmaCfg.dstMemAddr = (uint32_t) &GLOBAL_trimmer[0];
    dmaCfg.srcConn = GPDMA_ADC;

    /*-----------ENDPOINTS-----------*/
    GPDMA_Endpoint_T endpointSrc;
    endpointSrc.burst = GPDMA_BSIZE_1;
    endpointSrc.width = GPDMA_WORD;
    endpointSrc.increment = DISABLE;
    GPDMA_Endpoint_T endpointDst;
    endpointDst.burst = GPDMA_BSIZE_1;
    endpointDst.width = GPDMA_WORD;
    endpointDst.increment = ENABLE;
    /*-------------------------------*/

    dmaCfg.src = endpointSrc;
    dmaCfg.dst = endpointDst;
    dmaCfg.intErr = DISABLE;
    dmaCfg.intTC = DISABLE;

    /*-----------LINKEDLIST-----------*/
    static GPDMA_LLI_T lli;
    lli.srcAddr = (uint32_t) &(LPC_ADC->ADGDR);
    lli.dstAddr = (uint32_t) &GLOBAL_trimmer[0];
    lli.nextLLI = (uint32_t) &lli;
    lli.control = 0x00000000
    			 |2
				 |1u<<19
				 |1u<<22
				 |1u<<27;
    /*---------------------------------*/
    dmaCfg.linkedList = (uint32_t) &lli;

    GPDMA_SetupChannel(&dmaCfg);

    GPDMA_ChannelStart(GPDMA_CH_0);
}


uint32_t get_tiempoProcesamiento(void)
{
	return ((GLOBAL_trimmer[0]>>4) & 0x0FFF)* TRIMMER_MAX_TIEMPO_PROCESAMIENTO / 0x0FFF;
}
void get_servoStep(void)
{

}
