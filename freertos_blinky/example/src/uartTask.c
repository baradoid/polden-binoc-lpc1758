/*
 * uartTask.c
 *
 *  Created on: 19 февр. 2018 г.
 *      Author: Dmitry
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "sspTask.h"
#include "oneWire.h"
#include "adcTask.h"
#include "billValidator.h"
#include "utils.h"

#include <string.h>
#include <stdlib.h>


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

extern xSemaphoreHandle xUartTaskSemaphore;

void vUARTTask(void *pvParameters)
{
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
	//resetPhone();

	while (1) {
		if(xSemaphoreTake(xUartTaskSemaphore, configTICK_RATE_HZ/200) == true){
			//DEBUGSTR("sem taked\r\n");
			//bDataUpdated = true;
		}
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
			    if(andrCpuTemp > 40){
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
		    	batPwrOn();
		    }
		    else if(strstr((char*)inString, "pwrOff\n") != NULL){
		    	batPwrOff();
		    }
		    else if(strstr((char*)inString, "usbOn\n") != NULL){
		    	usbOn();
		    }
		    else if(strstr((char*)inString, "usbOff\n") != NULL){
		    	usbOff();
		    }
		    else if(strstr((char*)inString, "audioOn\n") != NULL){
		    	soundOn();
		    }
		    else if(strstr((char*)inString, "audioOff\n") != NULL){
		    	soundOff();
		    }
		    else if(strstr((char*)inString, "heatOn\n") != NULL){
		    	heatOn();
		    }
		    else if(strstr((char*)inString, "heatOff\n") != NULL){
		    	heatOff();
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
		    //resetPhone();
		    lastPhoneMsgRecvTime = xTaskGetTickCount();
		  }

		//vTaskDelay(configTICK_RATE_HZ/200);
	}
}

