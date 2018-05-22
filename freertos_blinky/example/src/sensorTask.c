#include "FreeRTOS.h"
#include "task.h"

#define SENSOR_COUNTER 20
bool but[7];

int checkPin(int port, int pin)
{
	uint32_t vals[SENSOR_COUNTER];
	LPC_GPIO_T *pPort = LPC_GPIO + port;
	Chip_IOCON_PinMux(LPC_IOCON, port, pin, IOCON_MODE_INACT, IOCON_FUNC0); //40
	Chip_GPIO_SetPinOutLow(LPC_GPIO, port, pin);
	//Chip_GPIO_SetPinOutHigh(LPC_GPIO, port, pin);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
	//Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, true);
	//Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, false);
	for(int i=0; i<SENSOR_COUNTER; i++){
		vals[i] = pPort->PIN;
//		if(Chip_GPIO_GetPinState(LPC_GPIO, port, pin)){
//			if(i>1){
//				//DEBUGOUT("sens0: %d\r\n", i);
//				return i;
//			}
//			break;
//		}
	}

	for(int i=0; i<SENSOR_COUNTER; i++){
		if( ((vals[i]>>pin)&1) != 0)
			return i;
	}

	return 0;


}

void vSensorTask(void *pvParameters)
{
//	Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC0); //40
//	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, false);
//	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 11, false);
//
//	Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC0); //39
//	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 10, false);
//	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 10, false);
//
//	Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC0); //38
//	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 1, false);
//	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 1, false);

	while(1)
	{
		taskENTER_CRITICAL();
		int val = 0;
		val = checkPin(0, 11);
		but[0] = (val>0);
		if(val > 0){
			//DEBUGOUT("sens0: %d\r\n", val);
		}

		val = checkPin(0, 10);
		but[1] = (val>0);
		if(val > 0){
			//DEBUGOUT("sens1: %d\r\n", val);
		}

		val = checkPin(0, 0);
		but[2] = (val>0);
		if(val > 0){
			//DEBUGOUT("sens2: %d\r\n", val);
		}

		val = checkPin(0, 1);
		but[3] = (val>0);
		if(val > 0){
			//DEBUGOUT("sens2: %d\r\n", val);
		}



		val = checkPin(1, 29);
		but[4] = (val>0);
		if(val > 0){
			//DEBUGOUT("sens2: %d\r\n", val);
		}

		val = checkPin(1, 28);
		but[5] = (val>0);
		if(val > 0){
			//DEBUGOUT("sens2: %d\r\n", val);
		}

		val = checkPin(1, 26);
		but[6] = (val>0);
		if(val > 0){
			//DEBUGOUT("sens2: %d\r\n", val);
		}

		taskEXIT_CRITICAL();

		vTaskDelay(configTICK_RATE_HZ / 50);

	}
}

