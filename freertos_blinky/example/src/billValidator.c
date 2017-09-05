#include "FreeRTOS.h"
#include "task.h"

typedef enum{
  unknownState,
  powerUpState,
  idleState,
  initState,
  disableState,
  acceptingState,
  stackingState,
  rejectingState,
} TCashCodeState;

TCashCodeState  ccState = unknownState;

portTickType lastBVPollTime=0;
char pollReqArr[] = {0x02, 0x03, 0x06, 0x33, 0xda, 0x81};
char resetReqArr[] = {0x02, 0x03, 0x06, 0x30, 0x41, 0xb3};
char writeBillTypeArr[] = {0x02, 0x03, 0x0C, 0x34, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xb5, 0xc1};
char ackArr[] = {0x02, 0x03, 0x06, 0x00, 0xc2, 0x82};
//char *sendArr;
char data[60];

void vBVTask(void *pvParameters)
{

	Chip_IOCON_PinMux(LPC_IOCON, 1, 19, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 19, true);  //fan rele
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 19, false);


	Chip_IOCON_PinMux(LPC_IOCON, 2, 0, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_IOCON_PinMux(LPC_IOCON, 2, 1, IOCON_MODE_INACT, IOCON_FUNC2);

	Chip_UART_Init(LPC_UART1);
	Chip_UART_SetBaud(LPC_UART1, 19200);

	Chip_UART_TXEnable(LPC_UART1);

	//int tickCnt = 0;
	int cashCount = 0;

	vTaskDelay(2*configTICK_RATE_HZ);

	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 19, true);

	for(;;){
		  if((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR) != 0){
			int rb = Chip_UART_ReadByte(LPC_UART1);
			if (rb != 0x02)
			  continue;
			while((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR) == 0) ;
			rb = Chip_UART_ReadByte(LPC_UART1);
			if((rb == -1) || (rb != 0x03))
				continue;
			while((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR) == 0) ;
			rb = Chip_UART_ReadByte(LPC_UART1);
			if(rb == -1)
				continue;
			int dataLength = rb-3;
			Chip_UART_ReadBlocking(LPC_UART1, &(data[0]), dataLength);
			if(dataLength == 0x03){
				if(data[0]==0x10){  //power up state
					//DEBUGOUT("powerUpState\r\n");
				  ccState = powerUpState;
				  Chip_UART_SendBlocking(LPC_UART1, &(ackArr[0]), 6);
				}
				else if(data[0]==0x13){  //init state
					//DEBUGOUT("initState\r\n");
				  ccState = initState;
				  Chip_UART_SendBlocking(LPC_UART1, &(ackArr[0]), 6);
				}
				else if(data[0]==0x19){  //disable state
					//DEBUGOUT("disableState\r\n");
				  ccState = disableState;
				  Chip_UART_SendBlocking(LPC_UART1, &(ackArr[0]), 6);
				}
				else if(data[0]==0x14){  //idle state
					//DEBUGOUT("idleState\r\n");
				  ccState = idleState;
				  Chip_UART_SendBlocking(LPC_UART1, &(ackArr[0]), 6);
				}
				else if(data[0]==0x15){  //accepting state
					//DEBUGOUT("acceptingState\r\n");
				  ccState = acceptingState;
				  Chip_UART_SendBlocking(LPC_UART1, &(ackArr[0]), 6);
//		              dispArr[0] = asciiTable['A'];
//		              dispArr[1] = asciiTable['C'];
//		              dispArr[2] = asciiTable['C'];
//		              dispArr[3] = asciiTable['0'];
				}
				else if(data[0]==0x17){  //stacking state
					//DEBUGOUT("stackingState\r\n");
				  ccState = stackingState;
				  Chip_UART_SendBlocking(LPC_UART1, &(ackArr[0]), 6);
//		              dispArr[0] = asciiTable['A'];
//		              dispArr[1] = asciiTable['C'];
//		              dispArr[2] = asciiTable['C'];
//		              dispArr[3] = asciiTable['1'];
				}
			  }
			  else if(dataLength == 0x04){
				if(data[0]==0x81){      //rubls packed
				  if(data[1]==0x02)
					cashCount +=10;
				  if(data[1]==0x03)
					cashCount +=50;
				  if(data[1]==0x04)
					cashCount +=100;

				  DEBUGOUT("cashCount %d\r\n", cashCount);
				  Chip_UART_SendBlocking(LPC_UART1, &(ackArr[0]), 6);
				}
				else if(data[0]==0x1c){  //rejecting state
					//DEBUGOUT("rejectingState\r\n");
				  ccState = rejectingState;
				  Chip_UART_SendBlocking(LPC_UART1, &(ackArr[0]), 6);
				}
			  }
//		          dispArr[7] = digTable[(cashCount%10)&0xf];
//		          dispArr[6] = digTable[(int)(cashCount/10)%10];
//		          dispArr[5] = digTable[(int)(cashCount/100)%10];
//		          dispArr[4] = digTable[(int)(cashCount/1000)%10];
		  }
		  else if((xTaskGetTickCount() - lastBVPollTime) > 200){
			  lastBVPollTime = xTaskGetTickCount();
			  if(ccState == powerUpState){
				  //DEBUGOUT("pu-state -> reset\r\n");
				  Chip_UART_SendBlocking(LPC_UART1, &(resetReqArr[0]), 6);
				  ccState = unknownState;
			  }
			  else if(ccState == disableState){
				  //DEBUGOUT("dis-state -> write bill\r\n");
				  Chip_UART_SendBlocking(LPC_UART1, &(writeBillTypeArr[0]), 12);
				  ccState = unknownState;
			  }

			  else{
				  //DEBUGOUT("poll\r\n");
				  Chip_UART_SendBlocking(LPC_UART1, &(pollReqArr[0]), 6);
			  }
		  }

		  vTaskDelay(configTICK_RATE_HZ/10);

	}
	//DEBUGOUT("exit BV task\r\n");
}


void BVOn()
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 19, false);
}

void BVOff()
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 19, true);
}
bool isBVEnable()
{
	return !Chip_GPIO_ReadPortBit(LPC_GPIO, 1, 19);
}

