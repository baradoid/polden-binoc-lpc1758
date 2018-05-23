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
#include "semphr.h"

#include "gpio_17xx_40xx.h"
#include "billValidator.h"
#include "oneWire.h"
#include "adcTask.h"
#include "sspTask.h"
#include "heatTask.h"
#include "uartTask.h"
#include <string.h>
#include "utils.h"
#include "sensorTask.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/


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


static void vReleTask(void *pvParameters) {
	fanOn();
	usbOff();
	batPwrOff();

	vTaskDelay(configTICK_RATE_HZ*2 );
	batPwrOn();
	vTaskDelay(configTICK_RATE_HZ*2 );
	usbOn();
	vTaskDelay(configTICK_RATE_HZ*10 );
	usbOff();


	fanOff();
	//Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 0, false);  //VBat
	//vTaskDelay(configTICK_RATE_HZ*3 );

	bool releState = false;

	while (1) {
		//Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, releState);
		//Chip_GPIO_WritePortBit(LPC_GPIO, 1, 0, releState);
		//Chip_GPIO_WritePortBit(LPC_GPIO, 1, 9, releState);

		//Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, releState); //Heat
		releState = (bool) !releState;
		/* About a 3Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ*2 );
	}
}




/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
xSemaphoreHandle xUartTaskSemaphore = NULL;
TaskHandle_t xUartTaskHandle = NULL;

int main(void)
{
	prvSetupHardware();

	Chip_IOCON_PinMux(LPC_IOCON, 2, 6, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 6, true);  //Heat

	Chip_IOCON_PinMux(LPC_IOCON, 1, 10, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 10, true);  //Mute
	soundOff();

	Chip_IOCON_PinMux(LPC_IOCON, 1, 23, IOCON_MODE_INACT, IOCON_FUNC0); //Tacho_Fan1
	Chip_IOCON_EnableOD(LPC_IOCON, 1, 23);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 23, true);  //Tacho_Fan1
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 23, true); //Tacho_Fan1

	Chip_IOCON_PinMux(LPC_IOCON, 1, 22, IOCON_MODE_INACT, IOCON_FUNC0); //Tacho_Fan2
	Chip_IOCON_EnableOD(LPC_IOCON, 1, 22);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 22, true);  //Tacho_Fan2
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 22, true); //Tacho_Fan2

//	Chip_IOCON_PinMux(LPC_IOCON, 1, 0, IOCON_MODE_INACT, IOCON_FUNC0); //VBat
//	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 0, true);  //VBat
//	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 0, true); //VBat

	Chip_IOCON_PinMux(LPC_IOCON, 1, 0, IOCON_MODE_INACT, IOCON_FUNC0); //fan rele
	//Chip_IOCON_EnableOD(LPC_IOCON, 1, 0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 0, true);  //fan rele
	fanOn();

	Chip_IOCON_PinMux(LPC_IOCON, 1, 8, IOCON_MODE_INACT, IOCON_FUNC0); //heat
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 8, true);  //heat
	heatOff();

	Chip_IOCON_PinMux(LPC_IOCON, 1, 19, IOCON_MODE_INACT, IOCON_FUNC0); //BV pwr
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 19, true);  //fan rele
	BVOn();

	Chip_IOCON_PinMux(LPC_IOCON, 1, 9, IOCON_MODE_INACT, IOCON_FUNC0); //bat pwr
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 9, true);  //bat rele

	Chip_IOCON_PinMux(LPC_IOCON, 1, 1, IOCON_MODE_INACT, IOCON_FUNC0); //usb pwr
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 1, true);  //usb pwr rele

	Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, false);



	printf("sysclk %.2f MHz periph %.2f MHz\r\n", Chip_Clock_GetSystemClockRate()/1000000., Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_SSP0)/1000000.);

	vSemaphoreCreateBinary( xUartTaskSemaphore );

	/* LED1 toggle thread */
	xTaskCreate(vAdcTask, (signed char *) "vAdcTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

//	/* LED2 toggle thread */
//	xTaskCreate(vLEDTask2, (signed char *) "vTaskLed2",
//				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
//				(xTaskHandle *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vUARTTask, (signed char *) "vTaskUart",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) &xUartTaskHandle);

	xTaskCreate(vReleTask, (signed char *) "vReleTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vSSPTask, (signed char *) "vSSPTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL),
				(xTaskHandle *) NULL);

//	xTaskCreate(vBVTask, (signed char *) "vBVTask",
//				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
//				(xTaskHandle *) NULL);

	xTaskCreate(vOneWireTask, (signed char *) "vOneWireTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vHeatTask, (signed char *) "vHeatTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vSensorTask, (signed char *) "vSensorTask",
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
