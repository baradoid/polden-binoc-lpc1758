#include "FreeRTOS.h"
#include "task.h"
#include "uartTask.h"

#define SENSOR_COUNTER 20
bool but[7];

int checkPin(int port, int pin)
{
	uint32_t vals[SENSOR_COUNTER];
	LPC_GPIO_T *pPort = LPC_GPIO + port;
	Chip_IOCON_PinMux(LPC_IOCON, port, pin, IOCON_MODE_INACT, IOCON_FUNC0); //40
	Chip_GPIO_SetPinOutLow(LPC_GPIO, port, pin);
	//Chip_GPIO_SetPinOutHigh(LPC_GPIO, port, pin);
	taskENTER_CRITICAL();
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
	taskEXIT_CRITICAL();
	for(int i=0; i<SENSOR_COUNTER; i++){
		if( ((vals[i]>>pin)&1) != 0)
			return i;
	}

	return 0;
}
uint32_t butBits = 0;
uint32_t lastButBits = 0;

void processBut(int ind, int port, int pin)
{
	int val = 0;
	val = checkPin(port, pin);
	but[ind] = (val>0);
	if(val > 0){
		butBits |= (1<<ind);
		//DEBUGOUT("sens0: %d\r\n", val);
	}
	else{
		butBits &= ~(1<<ind);
	}
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
		processBut(0, 0, 11);
		processBut(1, 0, 10);
		processBut(2, 0, 0);
		processBut(3, 0, 1);
		processBut(4, 1, 29);
		processBut(5, 1, 28);
		processBut(6, 1, 26);

		if(butBits != lastButBits){
			xTaskNotify(xUartTaskHandle, SENSOR_BIT_NOTIFY, eSetBits);
			lastButBits = butBits;
		}

		vTaskDelay(configTICK_RATE_HZ / 50);

	}
}

