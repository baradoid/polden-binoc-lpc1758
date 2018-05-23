/*
 * sspTask.c
 *
 *  Created on: 19 февр. 2018 г.
 *      Author: Dmitry
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio_17xx_40xx.h"
#include "semphr.h"
#include "uartTask.h"


int xPos1 = 0, xPos2 = 0;

static SSP_ConfigFormat ssp_format;
void vSSPTask(void *pvParameters)
{
	/* SSP initialization */
	Board_SSP_Init(LPC_SSP0);
	Chip_SSP_Init(LPC_SSP0);
	Chip_SSP_SetBitRate(LPC_SSP0, 200000);

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_14;
	ssp_format.clockMode = SSP_CLOCK_CPHA1_CPOL1;
	Chip_SSP_SetFormat(LPC_SSP0, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_Enable(LPC_SSP0);

	Board_SSP_Init(LPC_SSP1);
	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_SetBitRate(LPC_SSP1, 200000);

	Chip_SSP_SetFormat(LPC_SSP1, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_Enable(LPC_SSP1);

	int lastXPos1 = 0, lastXPos2 = 0;
	//bool bDataUpd = false;
	while (1) {
		Chip_SSP_SendFrame(LPC_SSP0, 0xabcd);
		uint16_t ssp0 = Chip_SSP_ReceiveFrame(LPC_SSP0);

		//ssp0 &= 0x1fff;
		xPos1 = ssp0&0x1fff;
		if(lastXPos1 != xPos1){
			lastXPos1 = xPos1;
			xTaskNotify(xUartTaskHandle, SSP_ENC1_BIT_NOTIFY, eSetBits);
		}

		Chip_SSP_SendFrame(LPC_SSP1, 0xabcd);
		uint16_t ssp1 = Chip_SSP_ReceiveFrame(LPC_SSP1);
		xPos2 = ssp1&0x1fff;
		if(lastXPos2 != xPos2){
			lastXPos2 = xPos2;
			xTaskNotify(xUartTaskHandle, SSP_ENC2_BIT_NOTIFY, eSetBits);
		}

		vTaskDelay(configTICK_RATE_HZ / 200);
	}
}
