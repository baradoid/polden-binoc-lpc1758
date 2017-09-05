#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio_17xx_40xx.h"


void soundOn()
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 9, false);
}
void soundOff()
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 9, true);
}
bool isSoundEnabled()
{
  return !Chip_GPIO_ReadPortBit(LPC_GPIO, 0, 9);;
}

void fanOn()
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, false);
}

void fanOff()
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, true);
}
bool isFanEnable()
{
	return !Chip_GPIO_ReadPortBit(LPC_GPIO, 2, 5);
}


void heatOn()
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, false);
}

void heatOff()
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 2, 6, true);
}
bool isheatEnable()
{
	return !Chip_GPIO_ReadPortBit(LPC_GPIO, 2, 6);
}


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


