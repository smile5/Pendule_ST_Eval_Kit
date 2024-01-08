/*
 * delay.h
 *
 *  Created on: 15 févr. 2023
 *      Author: xgaltier
 */

#ifndef DELAY_H_
#define DELAY_H_
#include <stdint.h>

uint32_t millis(void);
void systick_init_ms(uint32_t freq);
void delay_ms(uint32_t delay);

#endif /* DELAY_H_ */
