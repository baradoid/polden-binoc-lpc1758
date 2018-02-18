/*
 * heatTask.h
 *
 *  Created on: 19 февр. 2018 г.
 *      Author: Dmitry
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "oneWire.h"

bool bHeatOn = false;

enum heatState_t {off, on} heatState = off;
void vHeatTask(void *pvParameters)
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

