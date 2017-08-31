/*
 * oneWire.h
 *
 *  Created on: 28 авг. 2017 г.
 *      Author: Dmitry
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_
void vOneWireTask(void *pvParameters);

extern int16_t temp;
extern int dallasTemp;
#endif /* ONEWIRE_H_ */
