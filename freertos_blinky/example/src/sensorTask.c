#include "FreeRTOS.h"
#include "task.h"
#include "uartTask.h"

#define SENSOR_COUNTER 30
#define BUT_NUM 7
bool but[BUT_NUM];

int iterArrs[BUT_NUM];
int averageIters[BUT_NUM];

//int checkPin(int port, int pin, uint32_t *pVals)
//{
////	uint32_t vals[SENSOR_COUNTER];
////	LPC_GPIO_T *pPort = LPC_GPIO + port;
////	Chip_IOCON_PinMux(LPC_IOCON, port, pin, IOCON_MODE_INACT, IOCON_FUNC0); //40
////	Chip_GPIO_SetPinOutLow(LPC_GPIO, port, pin);
////	//Chip_GPIO_SetPinOutHigh(LPC_GPIO, port, pin);
////	taskENTER_CRITICAL();
////	Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
////	Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
////	//Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, true);
////	//Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, false);
////	for(int i=0; i<SENSOR_COUNTER; i++){
////		vals[i] = pPort->PIN;
////	}
////	taskEXIT_CRITICAL();
//	for(int i=0; i<SENSOR_COUNTER; i++){
//		if( ((pVals[i]>>pin)&1) != 0)
//			return i;
//	}
//
//	return 0;
//}
uint32_t butBits = 0;
uint32_t lastButBits = 0;

void processBut(int ind, int pin, uint32_t *pVals)
{
	int i=0;
	for(i=0; i<SENSOR_COUNTER; i++){
		if( ((pVals[i]>>pin)&1) != 0)
			break;
	}

	iterArrs[ind] = i;
	//val = checkPin(port, pin, pVals);
	but[ind] = (i>0);
	if(i > 0){
		butBits |= (1<<ind);
		//DEBUGOUT("sens0: %d\r\n", val);
	}
	else{
		butBits &= ~(1<<ind);
	}
}


void collectSensorData(uint32_t *pVals, int portInd, uint32_t pinMask)
{
	LPC_GPIO_T *pGpio = LPC_GPIO + portInd;
	Chip_GPIO_SetPortOutLow(LPC_GPIO, portInd, pinMask);
	taskENTER_CRITICAL();
	pGpio->DIR |= pinMask;   //Chip_GPIO_SetPortDIROutput(LPC_GPIO, portInd, pinMask);
	pGpio->DIR &= ~pinMask;  //Chip_GPIO_SetPortDIRInput(LPC_GPIO, portInd, pinMask);
	for(int i=0; i<SENSOR_COUNTER; i++){
		*pVals++ = pGpio->PIN; //Chip_GPIO_GetPortValue(LPC_GPIO, portInd);
	}
//	pVals[0] = pGpio->PIN;
//	pVals[1] = pGpio->PIN;
//	pVals[2] = pGpio->PIN;
//	pVals[3] = pGpio->PIN;
//	pVals[4] = pGpio->PIN;
//	pVals[5] = pGpio->PIN;
//	pVals[6] = pGpio->PIN;
//	pVals[7] = pGpio->PIN;
//	pVals[8] = pGpio->PIN;
//	pVals[9] = pGpio->PIN;
//	pVals[10] = pGpio->PIN;
//	pVals[11] = pGpio->PIN;
//	pVals[12] = pGpio->PIN;
//	pVals[13] = pGpio->PIN;
//	pVals[14] = pGpio->PIN;
//	pVals[15] = pGpio->PIN;
//	pVals[16] = pGpio->PIN;
//	pVals[17] = pGpio->PIN;
//	pVals[18] = pGpio->PIN;
//	pVals[19] = pGpio->PIN;

	taskEXIT_CRITICAL();
}

void vSensorTask(void *pvParameters)
{

	Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC0); //40
	Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC0); //39
	Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC0); //38
	Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_INACT, IOCON_FUNC0); //38

	Chip_IOCON_PinMux(LPC_IOCON, 1, 29, IOCON_MODE_INACT, IOCON_FUNC0); //38
	Chip_IOCON_PinMux(LPC_IOCON, 1, 28, IOCON_MODE_INACT, IOCON_FUNC0); //38
	Chip_IOCON_PinMux(LPC_IOCON, 1, 26, IOCON_MODE_INACT, IOCON_FUNC0); //38

//	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 11, false);
//	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 11, false);
//

//	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 10, false);
//	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 10, false);
//

//	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 1, false);
//	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 1, false);


//	processBut(0, 0, 11);
//	processBut(1, 0, 10);
//	processBut(2, 0, 0);
//	processBut(3, 0, 1);
//	processBut(4, 1, 29);
//	processBut(5, 1, 28);
//	processBut(6, 1, 26);

	for(int i=0; i<BUT_NUM; i++){
		averageIters[i] = 0;
	}

	uint32_t vals[SENSOR_COUNTER];
	int pinMask;
	while(1)
	{
		pinMask = (1<<0)|(1<<1)|(1<<10)|(1<<11);
		collectSensorData(&(vals[0]), 0, pinMask);

		processBut(0, 11, &(vals[0]));
		processBut(1, 10, &(vals[0]));
		processBut(2, 0, &(vals[0]));
		processBut(3, 1, &(vals[0]));

		pinMask = (1<<26)|(1<<28)|(1<<29);
		collectSensorData(&(vals[0]), 1, pinMask);

		processBut(4, 29, &(vals[0]));
		processBut(5, 28, &(vals[0]));
		processBut(6, 26, &(vals[0]));

		if(butBits != lastButBits){
			xTaskNotify(xUartTaskHandle, SENSOR_BIT_NOTIFY, eSetBits);
			lastButBits = butBits;
		}

		DEBUGOUT("sens: %d %d %d %d %d %d %d\r\n", iterArrs[0], iterArrs[1], iterArrs[2], iterArrs[3], iterArrs[4], iterArrs[5], iterArrs[6]);


		vTaskDelay(configTICK_RATE_HZ / 50);

	}
}

