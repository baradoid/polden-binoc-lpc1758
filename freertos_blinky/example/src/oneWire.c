#include "FreeRTOS.h"
#include "task.h"

//int16_t temp = -990;
int dallasTemp = -990;

int reset()
{
	Chip_UART_SetBaud(LPC_UART1, 9600);

	while((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR) != 0){
		Chip_UART_ReadByte(LPC_UART1);
	}

	Chip_UART_SendByte(LPC_UART1, 0xf0);
	while((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR) == 0) ;
	uint8_t rb = Chip_UART_ReadByte(LPC_UART1);
	//DEBUGOUT("OW: presence byte %x\r\n", rb);
	int iPresence = 0;
	if(rb!=0xf0)
		iPresence = 1;
	return iPresence;

}
void sendByte(uint8_t ch)
{
	Chip_UART_SetBaud(LPC_UART1, 115200);

	for(int i=0; i<8; i++){
		uint8_t d = 0;
		if((ch>>i)&0x1)
			d=0xff;
		else
			d=0x00;
		while(Chip_UART_CheckBusy(LPC_UART1) == SET );
			Chip_UART_SendByte(LPC_UART1, d);
		while((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR) == 0) ;
			Chip_UART_ReadByte(LPC_UART1);
	}
}

uint8_t readByte()
{
	uint8_t d = 0;
	Chip_UART_SetBaud(LPC_UART1, 115200);
	for(int i=0; i<8; i++){
		while(Chip_UART_CheckBusy(LPC_UART1) == SET );
			Chip_UART_SendByte(LPC_UART1, 0xff);
		while((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR) == 0) ;
		uint8_t rb = Chip_UART_ReadByte(LPC_UART1);
		if(rb == 0xff)
			d |= (1<<i);
	}

	return d;
}

void readScratchPad(uint8_t *d)
{

}

uint8_t calc_crc(uint8_t *mas, uint8_t Len )
{
	uint8_t i,dat,crc,fb,st_byt;
  st_byt=0; crc=0;
  do{
    dat=mas[st_byt];
    for( i=0; i<8; i++) {  // счетчик битов в байте
      fb = crc ^ dat;
      fb &= 1;
      crc >>= 1;
      dat >>= 1;
      if( fb == 1 ) crc ^= 0x8c; // полином
    }
    st_byt++;
  } while( st_byt < Len ); // счетчик байтов в массиве
  return crc;
}
void controlHeat(int t);
void controlFan();
void vOneWireTask(void *pvParameters)
{
	Chip_IOCON_PinMux(LPC_IOCON, 2, 0, IOCON_MODE_INACT, IOCON_FUNC2); //tx
	Chip_IOCON_PinMux(LPC_IOCON, 2, 1, IOCON_MODE_INACT, IOCON_FUNC2); //rx
	Chip_IOCON_EnableOD(LPC_IOCON, 2, 0);

	Chip_UART_Init(LPC_UART1);
	//Chip_UART_SetBaud(LPC_UART2, 9600);
	Chip_UART_TXEnable(LPC_UART1);
	uint8_t data[9];
	for(;;){
		int iPresence = reset();
		//DEBUGOUT("OW: presence %d\r\n", iPresence);
		if(iPresence){
			sendByte(0xcc); //Skip Rom
			sendByte(0xbe); //Read Scratchpad command
			for(int i=0; i<9; i++){
				data[i] = readByte();
			}
			uint8_t crc = calc_crc(&(data[0]), 8);
			if(crc != data[8]){
				//DEBUGOUT("OW: crc not match. Repeat.\r\n");
			}
			else{
				//DEBUGOUT("OW: %x %x %x %x %x %x %x %x %x crc %x\r\n",
				//data[0],data[1],data[2],data[3],data[4],data[5],data[6],
				//data[7],data[8], crc);
				int8_t t = (data[0]>>4)|(data[1]<<4);
				dallasTemp = t*10;
				//DEBUGOUT("OW: %d", temp);

			}
		}
		else{
			dallasTemp = -990;
		}
		iPresence = reset();
		if(iPresence){
			sendByte(0xcc); //Skip Rom
			sendByte(0x44); //Read Scratchpad command
		}
		vTaskDelay(configTICK_RATE_HZ);
	}

}

