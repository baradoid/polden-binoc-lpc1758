#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio_17xx_40xx.h"


void soundOn() { Chip_GPIO_WritePortBit(LPC_GPIO, 1, 10, false); }
void soundOff() { Chip_GPIO_WritePortBit(LPC_GPIO, 1, 10, true); }
bool isSoundEnabled() { return !Chip_GPIO_ReadPortBit(LPC_GPIO, 1, 10); }

void fanOn() { Chip_GPIO_WritePortBit(LPC_GPIO, 1, 0, true); }
void fanOff() { Chip_GPIO_WritePortBit(LPC_GPIO, 1, 0, false); }
bool isFanEnable() { return !Chip_GPIO_ReadPortBit(LPC_GPIO, 1, 0); }


//void heatOn() { Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, false);}
//void heatOff() { Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, true); }
//bool isheatEnable(){ return !Chip_GPIO_ReadPortBit(LPC_GPIO, 2, 6); }
void heatOn() { Chip_GPIO_WritePortBit(LPC_GPIO, 1, 8, true); }
void heatOff() { Chip_GPIO_WritePortBit(LPC_GPIO, 1, 8, false); }
bool isHeatEnabled() { return !Chip_GPIO_ReadPortBit(LPC_GPIO, 1, 8); }

void batPwrOn(){ Chip_GPIO_WritePortBit(LPC_GPIO, 1, 9, true); }
void batPwrOff(){ Chip_GPIO_WritePortBit(LPC_GPIO, 1, 9, false); }
bool isBatPwrOn() { return !Chip_GPIO_ReadPortBit(LPC_GPIO, 1, 9); }

void usbOn(){ Chip_GPIO_WritePortBit(LPC_GPIO, 1, 1, true); }
void usbOff(){ Chip_GPIO_WritePortBit(LPC_GPIO, 1, 1, false); }
bool isUsbOn(){ return !Chip_GPIO_ReadPortBit(LPC_GPIO, 1, 1); }



void resetPhone()
{
	//Chip_GPIO_WritePortBit(LPC_GPIO, 1, 0, false); //VBat
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 22, false); //Tacho_Fan2
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 23, true); //Tacho_Fan1

	vTaskDelay(configTICK_RATE_HZ*1);
	//Chip_GPIO_WritePortBit(LPC_GPIO, 1, 0, true); //VBat
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 23, false); //Tacho_Fan1
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 22, true); //Tacho_Fan2
	//vTaskDelay(configTICK_RATE_HZ*5);


	vTaskDelay(configTICK_RATE_HZ*15);
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 23, true); //Tacho_Fan1
	//vTaskDelay(configTICK_RATE_HZ*30);
}


