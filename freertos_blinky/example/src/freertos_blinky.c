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
#include "oneWire.h"
#include "adcTask.h"
#include <string.h>
#include "utils.h"
/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/


//bool bFanOn = false;
bool bHeatOn = false;
int xPos1 = 0, xPos2 = 0;
//uint16_t ssp0 = 0;
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



enum heatState_t {off, on} heatState = off;
static void vHeatTask(void *pvParameters)
{
	const unsigned long heatMaximumEnableTimeSec = 7; //включаем нагрев максимум на это время
	const unsigned long heatEnablePeriodTimeSec = 60; //не чаще чем один раз в это время
	unsigned long lastHeatEnableTime = 0;

	for(;;){
		uint32_t curTime = xTaskGetTickCount()/1000;
	  if(dallasTemp > -99){
		if(dallasTemp < 50){
		  //Serial.print("less15 ");
		  switch(heatState){
			case off:
			  if((curTime - lastHeatEnableTime) > heatEnablePeriodTimeSec){
				//Serial.print("gp ");
				lastHeatEnableTime = curTime;
				heatState = on;
				Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, true); //Heat
				bHeatOn = true;
			  }
			  else{
				Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, false); //Heat
				bHeatOn = false;
			  }
			  break;
			case on:
			//Serial.print("con ");
			  if((curTime - lastHeatEnableTime) > heatMaximumEnableTimeSec){
				//Serial.print("ge ");
				Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, false); //Heat
				heatState = off;
				bHeatOn = false;
			  }
			  else{
				//Serial.print("le ");
				Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, true); //Heat
				bHeatOn = true;
			  }
			  break;
		  }
		  //Serial.println(" ");
		}
		else{
			Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, false); //Heat
		  heatState = off;
		}
	  }
	  else{
		  Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, false); //Heat
		  heatState = off;
	  }
	  vTaskDelay(configTICK_RATE_HZ*heatMaximumEnableTimeSec );
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


static void vReleTask(void *pvParameters) {


	//Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 0, false);  //VBat
	//vTaskDelay(configTICK_RATE_HZ*3 );

	bool releState = false;

	while (1) {
		//Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, releState);
		//Chip_GPIO_WritePortBit(LPC_GPIO, 1, 0, releState);
		///Chip_GPIO_WritePortBit(LPC_GPIO, 0, 9, releState);

		//Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, releState); //Heat
		releState = (bool) !releState;
		/* About a 3Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ*2 );
	}
}

#define IN_BUF_LEN 50
uint8_t inString[IN_BUF_LEN] = "";
int curInStringInd = 0;
bool readSerial()
{
  bool ret = false;
  while (Chip_UART_ReadLineStatus(LPC_UART0) & UART_LSR_RDR) {
	  uint8_t inChar = Chip_UART_ReadByte(LPC_UART0);
    //if (isDigit(inChar)) {
      // convert the incoming byte to a char
      // and add it to the string:
      inString[curInStringInd++] = (char)inChar;
    //}

      if(curInStringInd>=IN_BUF_LEN){
    	  curInStringInd = 0;
      }

    if (inChar == '\n') {
      //andrCpuTemp = inString.toInt();
      //inString = "";
    	inString[curInStringInd] = 0;
    	curInStringInd = 0;
      ret = true;
      break;
    }
  }
  return ret;
}

static void vUARTTask(void *pvParameters) {
	uint32_t tickCnt = 0;

	char str[50], lastStr[50];

	int lastXPos1 = 0, lastXPos2 = 0;
	int lastAndrCpuTemp = 0, andrCpuTemp=0;
	uint64_t lastPhoneMsgRecvTime = 0;
	bool bDataUpdated = false;
	int lastDallasTemp = -990;
	int sharpValLast = 0;
	uint64_t lastDistContrTime = 0;
	int lastCashCount = 0;

	sprintf(str, "%04X %04X %04d %04d %04d    000 000 000 %06d", xPos2, xPos1, dallasTemp, sharpVal, andrCpuTemp, cashCount);
	resetPhone();

	while (1) {
		if(readSerial() == true){
		    if(strcmp((char*)inString, "reset\n") == 0){
		      resetPhone();
		      lastPhoneMsgRecvTime = xTaskGetTickCount();
		    }
		    else if(strcmp((char*)inString, "son\n") == 0){
		      soundOn();
		      lastPhoneMsgRecvTime = xTaskGetTickCount();
		      //DEBUGSTR("son!!");
		    }
		    else if(strcmp((char*)inString, "soff\n") == 0){
		      soundOff();
		      lastPhoneMsgRecvTime = xTaskGetTickCount();
		      //DEBUGSTR("soff!!");
		    }
		    else if(strstr((char*)inString, "t=") != NULL){
		    	lastPhoneMsgRecvTime = xTaskGetTickCount();
		    	andrCpuTemp = atoi(&(inString[2]));
		      //sprintf(&(str[20]),"%04d", andrCpuTemp);
			    if(andrCpuTemp > 30){
			    	fanOn();
			    }
			    else{
			    	fanOff();
			    }
		    }
		    else if(strstr((char*)inString, "fanOn\n") != NULL){
		    	fanOn();
		    }
		    else if(strstr((char*)inString, "fanOff\n") != NULL){
		    	fanOff();
		    }
		    else if(strstr((char*)inString, "pwrOn\n") != NULL){
		    	batPwrOff();
		    }
		    else if(strstr((char*)inString, "pwrOff\n") != NULL){
		    	batPwrOn();
		    }
		    else if(strstr((char*)inString, "usbOn\n") != NULL){
		    	usbOn();
		    }
		    else if(strstr((char*)inString, "usbOff\n") != NULL){
		    	usbOff();
		    }

//		    else if(inString.startsWith("d=") == true){
//		      inString.remove(0, 2);
//
//		      dallasTemp = inString.toInt();
//		      //sprintf(&(str[20]),"%04d", andrCpuTemp);
//		      lastPhoneMsgRecvTime = millis();
//		    }
		    //inString = "";
		}

		  if((xTaskGetTickCount() - lastDistContrTime) > 50 ){
		    lastDistContrTime = xTaskGetTickCount();
			if(sharpVal != sharpValLast){
				sharpValLast = sharpVal;
				bDataUpdated = true;
				sprintf(&(str[15]), "%04d", sharpVal);
				str[19] = ' ';
			}
		  }

		if(dallasTemp != lastDallasTemp){
			lastDallasTemp = dallasTemp;
			bDataUpdated = true;
	        sprintf(&(str[10]), "%04d", dallasTemp);
	        str[14] = ' ';
		}

		if(xPos1 != lastXPos1){
			lastXPos1 = xPos1;
		    bDataUpdated = true;
		    sprintf(&(str[5]), "%04X", xPos1);
		    str[9] = ' ';
		}


		if(xPos2 != lastXPos2){
			lastXPos2 = xPos2;
		    bDataUpdated = true;
		    sprintf(&(str[0]), "%04X", xPos2);
		    str[4] = ' ';
		}
		if(cashCount != lastCashCount){
			lastCashCount = cashCount;
		    bDataUpdated = true;
			sprintf(&(str[40]), "%06d", cashCount);
		}

		if(bDataUpdated){
			bDataUpdated = false;
			//DEBUGOUT("%04X %04X %04d %04d %04d    000 000 000", xPos1, xPos2, dallasTemp, sharpVal, andrCpuTemp);
			sprintf(&(str[20]), "%04d    000 000 000", andrCpuTemp);
			str[25] = isFanEnable()? 'E':'D';
			str[26] = 'D'; //str[26] = bHeatOn? 'E':'D';
			str[24] = str[27] = str[31] = str[35] = str[39] = ' ';
			str[46] = 0;
			DEBUGSTR(str);
			DEBUGSTR("\r\n");
			tickCnt++;
		}

		  if( ((xTaskGetTickCount() - lastPhoneMsgRecvTime)/1000) > 1200){
		    resetPhone();
		    lastPhoneMsgRecvTime = xTaskGetTickCount();
		  }
		/* About a 1s delay here */
		vTaskDelay(configTICK_RATE_HZ/100);
	}
}

static SSP_ConfigFormat ssp_format;
static void vSSPTask(void *pvParameters)
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

	while (1) {
		Chip_SSP_SendFrame(LPC_SSP0, 0xabcd);
		uint16_t ssp0 = Chip_SSP_ReceiveFrame(LPC_SSP0);

		//ssp0 &= 0x1fff;
		xPos1 = ssp0&0x1fff;

		Chip_SSP_SendFrame(LPC_SSP1, 0xabcd);
		uint16_t ssp1 = Chip_SSP_ReceiveFrame(LPC_SSP1);

		/* About a 7Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 100);
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

	Chip_IOCON_PinMux(LPC_IOCON, 2, 6, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 6, true);  //Heat

	Chip_IOCON_PinMux(LPC_IOCON, 0, 9, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 9, true);  //Mute
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
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 0, true);  //fan rele
	fanOn();

	Chip_IOCON_PinMux(LPC_IOCON, 2, 6, IOCON_MODE_INACT, IOCON_FUNC0); //heat rele
	Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 6, true);  //fan rele
	heatOff();

	Chip_IOCON_PinMux(LPC_IOCON, 1, 19, IOCON_MODE_INACT, IOCON_FUNC0); //BV pwr
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 19, true);  //fan rele
	BVOn();

	Chip_IOCON_PinMux(LPC_IOCON, 1, 9, IOCON_MODE_INACT, IOCON_FUNC0); //bat pwr
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 9, true);  //bat rele

	Chip_IOCON_PinMux(LPC_IOCON, 1, 1, IOCON_MODE_INACT, IOCON_FUNC0); //usb pwr
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 1, true);  //usb pwr rele

	printf("sysclk %.2f MHz periph %.2f MHz\r\n", Chip_Clock_GetSystemClockRate()/1000000., Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_SSP0)/1000000.);

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
				(xTaskHandle *) NULL);

	xTaskCreate(vReleTask, (signed char *) "vReleTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vSSPTask, (signed char *) "vSSPTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vBVTask, (signed char *) "vBVTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vOneWireTask, (signed char *) "vOneWireTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(vHeatTask, (signed char *) "vHeatTask",
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
