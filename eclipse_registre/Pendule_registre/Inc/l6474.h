/*
 * l6474.h
 *
 *  Created on: 7 mars 2023
 *      Author: xgaltier
 */

#ifndef L6474_H_
#define L6474_H_

#include <stdint.h>
#include <stm32f4xx.h>
#include "delay.h"

void Reset_Low (void);
void Reset_High (void);
void Cs_Low (void);
void Cs_High (void);
void DIR_Low(void);
void DIR_High(void);
void L6474_Disable(void);
void L6474_Enable(void);
void L6474_OverCurrent(float current);
void L6474_SetCurrent(float current);
void L6474_ResetABSPos(void);
void L6474_SetStepSize(unsigned char step);
void L6474_SetALARM(unsigned char alarm_flag);

unsigned short L6474_GetCONFIG();
unsigned char L6474_GetSTEPMODE();
float L6474_GetOCD_TH(void);
float L6474_GetTVAL(void);
long L6474_GetABSPOS(void);
void L6474_GetStatus(unsigned char *data);

void Spi_Receive (unsigned char *data, unsigned short size);
void Spi_Transmit (unsigned char *data, unsigned short size);
void Spi_Transmit1 (unsigned char data);


#endif /* L6474_H_ */
