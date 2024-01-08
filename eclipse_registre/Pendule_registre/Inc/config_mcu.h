/*
 * config_mcu.h
 *
 *  Created on: 15 févr. 2023
 *      Author: xgaltier
 */

#ifndef CONFIG_MCU_H_
#define CONFIG_MCU_H_

//Encoder: 	 E1 - rouge - +5V
//         	 E2 - noir - GND
//			 E3 - Blanc - A -  PB4
//			 E4 - Vert  - B - PB5

// Carte moteur IHMO1A1
//			CN5 - 1 - PA9 - Reset L6474
//			CN5 - 2 - PC7 - Step
//			CN5 - 3 - PB6 - SPI_CS
//			CN5 - 4 - PA7 - SPI_MOSI
//			CN5 - 5 - PA6 - SPI_MISO
//			CN5 - 6 - PA5 - SPI_SCK  aussi LED Verte sur Nucleo-F401RE
//			CN5 - 7 - GND - GND

//			CN9 - 1 - PA3 - Uart RX
//			CN9 - 2 - PA2 - Uart TX
//			CN9 - 3 - PA10 - FLAG
//			CN9 - 4 - PB3 - Nada
//			CN9 - 5 - PB5 - Nada
//			CN9 - 6 - PB4 - Nada
//			CN9 - 7 - PB10 - Nada
//			CN9 - 8 - PA8 - DIR
//
// 			CN7 - 37 - PC3 - GPIO for Test
//          CN7 - 23 - PC13 - User bouton sur Nucleo-F401RE

#include <stdint.h>
#include <stm32f4xx.h>

#define NPT_CODEUR 600
#define FOSC 84000000
#define FOSC_2 42000000
#define FECH 500 // 10 fois Fech car on divise par 10 dans l'ISR du timer10 (ici donc fech =500Hz => Tech= 2ms )
#define MOY_FILT 10 // nb de moy filtre
#define FECH_NB_FILT MOY_FILT*FECH

unsigned char user_switch(void);

void conf_horloge();
void config_pendule();
void conf_gpio();
void conf_dma();
void conf_uart();
void conf_codeur();
void conf_L6474();
void conf_Tech(unsigned short );
void conf_TStep();
#endif /* CONFIG_MCU_H_ */
