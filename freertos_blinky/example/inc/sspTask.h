/*
 * sspTask.h
 *
 *  Created on: 19 февр. 2018 г.
 *      Author: Dmitry
 */
#pragma once

void vSSPTask(void *pvParameters);
extern int xPos1, xPos2;
void initSSP();
bool checkEncData();
