#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdlib.h"
double filter(int d)
{
  static double acc = 0;
  acc = 0.2*(double)d + 0.8 *acc;
  return acc;
}

#define Ntap 8

    float FIRCoef[Ntap] = {
        0.08997650465060308400,
        0.13443834128434992000,
        0.16588587705222707000,
        0.17723405264792491000,
        0.16588587705222707000,
        0.13443834128434992000,
        0.08997650465060308400,
        0.04216450137771470000
    };
float  x[Ntap]; //input samples
float  fir(float  NewSample) {
    float y=0;            //output sample
    int n;

    //shift the old samples
    for(n=Ntap-1; n>0; n--)
       x[n] = x[n-1];

    //Calculate the new output
    x[0] = NewSample;
    for(n=0; n<Ntap; n++)
        y += FIRCoef[n] * x[n];

    return y;
}


int calcPoly(int mV, int mV1, int mV2, int cm1, int cm2)
{
  float d = abs(mV1-mV2) / (float)abs(cm1-cm2);
  int dist = 60;
  if(d > 0){
    dist = ((mV1-mV)/d) + cm1;
  }
  return dist;
}

int recalcMvToCm(int mV)
{
  int dist = 0;
  if(mV < 500){
    dist = calcPoly(mV, 500, 250, 25, 40); //500to250 mv = 25to40;
  }
  else if(mV < 1000){
    dist = calcPoly(mV, 1000, 500, 12, 25); //1000to500 mv = 12to25;
  }
  else{
    dist = calcPoly(mV, 3000, 1000, 3,12);   //3000to1000 mv = 3to12
  }
  return dist;

}
void getDistance()
{


  //filteredDist = recalcMvToCm((int)filter(mV));
}

volatile int sharpVal = 0;

void vAdcTask(void *pvParameters)
{
	Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_INACT, IOCON_FUNC1);
	//Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 25, false);  //VBat


//	ADC_CLOCK_SETUP_T adcSetupStr;
//	adcSetupStr.adcRate =  ADC_MAX_SAMPLE_RATE;
//	adcSetupStr.bitsAccuracy = 12;
//	adcSetupStr.burstMode = false;
	ADC_CLOCK_SETUP_T ADCSetup;
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ADC);
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	//Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, 100);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
	Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);
//	uint32_t cr = 0;
//	cr |= ADC_CR_CLKDIV(100);
//	cr |= ADC_CR_CH_SEL(ADC_CH2);
//	//cr |= ADC_CR_BURST;
//	cr |= ADC_CR_PDN;
//	LPC_ADC->CR = cr;
	//bool LedState = false;
	DEBUGOUT("ADC periph clock %d \r\n", Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_ADC));

	uint16_t val;
	//int iPass =0;
	Status stat;
	while (1) {
		bool overrun = false;
		int waitCount = 0;
		uint32_t averVal = 0;
		for(int i=0; i<1000; i++){
			while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH2, ADC_DR_DONE_STAT) != SET) {
				//DEBUGOUT("ADC: wait\r\n");
				waitCount++;
				vTaskDelay(0);
			}
			if(Chip_ADC_ReadStatus(LPC_ADC, ADC_CH2, ADC_DR_OVERRUN_STAT) == SET){
				//DEBUGOUT("ADC: overrun\r\n");
				overrun = true;
			}
			stat = Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &val);
			averVal+=val;

		}
		averVal/=1000;
		//stat = SUCCESS;
		//val = ADC_DR_RESULT(LPC_ADC->GDR);
		//chn = (LPC_ADC->GDR>>24)&0x3;

		if(stat == SUCCESS){
//			if(averVal > 400){
//				DEBUGOUT("ADC: %d wc:%d %s\r\n", averVal, waitCount, overrun? "overrun":"");
//			}
			//sharpVal = val;
			  int mV = fir(val)*0.8; //in mV

			  //sharpVal = recalcMvToCm(mV);
			  sharpVal = recalcMvToCm(mV);
//			  if (sharpVal < 25){
//				  DEBUGOUT("sharpVal: %d iPass:%d\r\n", sharpVal, iPass++);
//			  }
		}
		else{
			DEBUGOUT("ADC error\r\n");

		}

		//vTaskDelay(configTICK_RATE_HZ / 20);
		vTaskDelay(configTICK_RATE_HZ / 20);
	}
}
