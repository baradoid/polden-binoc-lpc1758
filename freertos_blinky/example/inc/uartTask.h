/*
 * uartTask.h
 *
 *  Created on: 19 февр. 2018 г.
 *      Author: Dmitry
 */

#pragma once

void vUARTTask(void *pvParameters);

extern TaskHandle_t xUartTaskHandle;

#define SSP_ENC1_BIT_NOTIFY 0x01
#define SSP_ENC2_BIT_NOTIFY 0x02
#define ADC_BIT_NOTIFY 		0x04
#define SENSOR_BIT_NOTIFY 	0x08
