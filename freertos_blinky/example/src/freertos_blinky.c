/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#include "gpio_17xx_40xx.h"
#include "billValidator.h"
/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
uint16_t ssp0 = 0;
uint16_t adc = 0;
/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
	Chip_IOCON_PinMux(LPC_IOCON, 0, 2, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 3, IOCON_MODE_INACT, IOCON_FUNC1);

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

/* LED1 toggle thread */
static void vReleTask(void *pvParameters) {

	//Chip_IOCON_PinMux(LPC_IOCON, 2, 5, IOCON_MODE_INACT, IOCON_FUNC1);
	//Chip_IOCON_PinMux(LPC_IOCON, 0, 3, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON, 1, 0, IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 5, true);  //fan rele
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 0, true);  //VBat

	bool releState = false;

	while (1) {
		//Board_LED_Set(0, LedState);

		Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, releState);
		Chip_GPIO_WritePortBit(LPC_GPIO, 1, 0, releState);
		releState = (bool) !releState;
		/* About a 3Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ*10 );
	}
}

static ADC_CLOCK_SETUP_T ADCSetup;
static void vLEDTask1(void *pvParameters) {
	Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_INACT, IOCON_FUNC1);
	//Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 25, false);  //VBat

	ADC_CLOCK_SETUP_T adcSetupStr;
	adcSetupStr.adcRate =  ADC_MAX_SAMPLE_RATE;
	adcSetupStr.bitsAccuracy = 12;
	adcSetupStr.burstMode = false;
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
	//bool LedState = false;


	Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);
	uint16_t val;
	Status stat;
	while (1) {
		Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH2, ADC_DR_DONE_STAT) != SET) {}

		Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &val);

		if(stat = SUCCESS){
			//DEBUGOUT("ADC: %d\r\n", val);
			adc = val;
		}
		else{
			DEBUGOUT("ADC error\r\n");

		}


		vTaskDelay(configTICK_RATE_HZ / 4);
	}
}

///* LED2 toggle thread */
//static void vLEDTask2(void *pvParameters) {
//	bool LedState = false;
//
//	while (1) {
//		Board_LED_Set(1, LedState);
//		LedState = (bool) !LedState;
//
//		/* About a 7Hz on/off toggle rate */
//		vTaskDelay(configTICK_RATE_HZ / 14);
//	}
//}

static SSP_ConfigFormat ssp_format;

/* UART (or output) thread */
static void vUARTTask(void *pvParameters) {
	int tickCnt = 0;

	while (1) {
		DEBUGOUT("Tick: %d, %x 0x%x\r\n", tickCnt, adc, ssp0);
		tickCnt++;

		/* About a 1s delay here */
		vTaskDelay(configTICK_RATE_HZ/2);
	}
}

static void vSSPTask(void *pvParameters)
{
	/* SSP initialization */
	Board_SSP_Init(LPC_SSP0);

	Chip_SSP_Init(LPC_SSP0);

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_14;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(LPC_SSP0, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_Enable(LPC_SSP0);

	while (1) {
		Chip_SSP_SendFrame(LPC_SSP0, 0xabcd);
		ssp0 = Chip_SSP_ReceiveFrame(LPC_SSP0);

		/* About a 7Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 14);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void)
{
	prvSetupHardware();

	printf("sysclk %.2f MHz periph %.2f MHz\r\n", Chip_Clock_GetSystemClockRate()/1000000., Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_SSP0)/1000000.);

	/* LED1 toggle thread */
	xTaskCreate(vLEDTask1, (signed char *) "vTaskLed1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

//	/* LED2 toggle thread */
//	xTaskCreate(vLEDTask2, (signed char *) "vTaskLed2",
//				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
//				(xTaskHandle *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vUARTTask, (signed char *) "vTaskUart",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vReleTask, (signed char *) "vReleTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vSSPTask, (signed char *) "vSSPTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vBVTask, (signed char *) "vBVTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

/**
 * @}
 */
