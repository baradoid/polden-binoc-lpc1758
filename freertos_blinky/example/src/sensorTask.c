#include "FreeRTOS.h"
#include "task.h"


void vSensorTask(void *pvParameters)
{
	Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC0); //40
	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, false);
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 11, false);

	Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC0); //39
	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 10, false);
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 10, false);

	Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC0); //38
	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 1, false);
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 1, false);

	while(1)
	{
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, true);
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, false);
		for(int i=0; i<100; i++){
			if(Chip_GPIO_ReadPortBit(LPC_GPIO, 0, 11)){
				if(i>1){
					DEBUGOUT("sens0: %d\r\n", i);
				}
				break;
			}
		}

		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 10, true);
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 10, false);
		for(int i=0; i<100; i++){
			if(Chip_GPIO_ReadPortBit(LPC_GPIO, 0, 10)){
				if(i>1){
					DEBUGOUT("sens1: %d\r\n", i);
				}
				break;
			}
		}

		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 1, true);
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 1, false);
		for(int i=0; i<100; i++){
			if(Chip_GPIO_ReadPortBit(LPC_GPIO, 0, 1)){
				if(i>1){
					DEBUGOUT("sens2: %d\r\n", i);
				}
				break;
			}
		}

		vTaskDelay(configTICK_RATE_HZ / 10);

	}
}
