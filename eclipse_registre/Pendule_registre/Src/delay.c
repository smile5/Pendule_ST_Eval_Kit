/*
 * delay.c
 *
 *  Created on: 15 févr. 2023
 *      Author: xgaltier
 */


#include "delay.h"
#include "stm32f4xx.h"                  // Device header


volatile uint32_t ms,rms;

void systick_init_ms(uint32_t freq)
{
	__disable_irq();
	SysTick->LOAD=(freq/1000)-1;
	SysTick->VAL=0;
	SysTick->CTRL=7; //0b00000111;
	__enable_irq();
}

uint32_t millis(void)
{
	__disable_irq();
	rms=ms; //store current ms in rms
	__enable_irq();
	return rms;
}

void SysTick_Handler(void)
{
	ms++;
}

void delay_ms(uint32_t delay)
{
	  // Calculate the tick value when the system should stop delaying.
	  uint32_t next = ms + delay;

	  // Wait until the system reaches that tick value.
	  // Use the 'wait for interrupt' instruction to save power.
	  while ( ms < next ) { }//__asm__( "WFI" ); }
}
