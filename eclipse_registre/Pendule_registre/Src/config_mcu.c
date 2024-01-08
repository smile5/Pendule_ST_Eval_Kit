/*
 * config_mcu.c
 *
 *  Created on: 15 févr. 2023
 *      Author: xgaltier
 */

#include "config_mcu.h"

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

unsigned char tab_dma[20]={'=','0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','+'};

unsigned char user_switch(void)
{
	unsigned char value;
	value = (GPIOC->IDR) && 0x00002000;
	if (value == 1)
	{
		value=0;
	}
	else
	{
		value =1;
	}
	return value;
}

void config_pendule()
{
	conf_horloge();
	conf_gpio();
	conf_uart();
	conf_dma();
	conf_codeur();
	conf_L6474();
	conf_Tech(FECH_NB_FILT);
	conf_TStep();
}

void conf_horloge()
{
	// Prescaler Configrations
		RCC->CFGR 	|= (4 << 10);			// APB1 Prescaler = 2 APB2 a 42MHz
		RCC->CFGR 	|= (3 << 13);			// APB2 Prescaler = 1 APB1 a 84MHz

		//RCC->CR 	|= (1 << 16);			// HSE Clock Enable - HSEON
		//while(!(RCC->CR & 0x00020000));		// Wait until HSE Clock Ready - HSERDY

		// PLL Configrations
		RCC->PLLCFGR = 0;					// Clear all PLLCFGR register
		RCC->PLLCFGR |=  (8		<<  0);		// PLLM = 8;
		RCC->PLLCFGR |=  (168 	<<  6);		// PLLN = 168;
		RCC->PLLCFGR |= (1 	<< 16);			// PLLP = 4; // For 2, Write 0
		RCC->PLLCFGR &=  ~(1 	<< 22);		// HSI Oscillator clock select as PLL
		RCC->PLLCFGR |=  (7 	<< 24);		// PLLQ = 7;

		RCC->CR 		|=  (1 		<< 24); // PLL Clock Enable - PLLON
		while(!(RCC->CR & 0x02000000)); 	// Wait until PLL Clock Ready - PLLRDY

		// Flash Configrations
		FLASH->ACR = 0;						// Clear all ACR register (Access Control Register)
		FLASH->ACR 		|= (2 <<  0); 		// Latency - 5 Wait State
		FLASH->ACR 		|= (1 <<  9);		// Instruction Cache Enable
		FLASH->ACR 		|= (1 << 10);		// Data Cache Enable
		FLASH->ACR 		|= (1 << 8);		// Prefetch Enable

		RCC->CFGR 		|= (2 <<  0);		// PLL Selected as System Clock
		while((RCC->CFGR & 0x0F) != 0x0A); 	// Wait PLL On
		//SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
}

void conf_gpio()
{
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	/*Enable clock access to GPIOC*/
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	/*Set PA5 as output pin LED Green*/
	//GPIOA->MODER |= GPIO_MODER_MODE5_0;
	//GPIOA->MODER  &=~GPIO_MODER_MODE5_1;
	/*Set PC3 as output for Test */
	GPIOC->MODER |= GPIO_MODER_MODE3_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE3_1;
	/* Config PC13 for bouton User */
	GPIOC->MODER &= ~GPIO_MODER_MODE13_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE13_1;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13_0;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13_1;

}

void conf_codeur()
{
	/*Enable clock access to GPIOB*/
		RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
		RCC->APB1ENR  |=  ( RCC_APB1ENR_TIM3EN );
	    /* configuration de GPIOB4 et B5 pour utilisation TIM3 ( quad encoder ) mode AF02 */
		GPIOB->MODER    &= ~( ( 0x3 << ( 4 * 2 ) ) |
	                          ( 0x3 << ( 5 * 2 ) ) );
	    GPIOB->MODER    |=  ( ( 0x2 << ( 4 * 2 ) ) |
	                          ( 0x2 << ( 5 * 2 ) ) );
	    GPIOB->OTYPER   &= ~( ( 0x1 << 4 ) |
	                          ( 0x1 << 5 ) );
	    GPIOB->OSPEEDR  &= ~( ( 0x3 << ( 4 * 2 ) ) |
	                          ( 0x3 << ( 5 * 2 ) ) );
	    GPIOB->OSPEEDR  |=  ( ( 0x3 << ( 4 * 2 ) ) |
	                          ( 0x3 << ( 5 * 2 ) ) );
	/* fonction AF2 */
	    GPIOB->AFR[0] &= ~( ( 0xF << ( 4 * 4 ) ) );
	    GPIOB->AFR[0] |=  ( ( 0x2 << ( 4 * 4 ) ) );
	    GPIOB->AFR[0] &= ~( ( 0xF << ( 5 * 4 ) ) );
	    GPIOB->AFR[0] |=  ( ( 0x2 << ( 5 * 4 ) ) );
	    GPIOB->PUPDR &= ~( ( 0x3 << ( 4 * 2 ) ) |
                		   ( 0x3 << ( 5 * 2 ) ) );
	    GPIOB->PUPDR |= ( ( 0x1 << ( 4 * 2 ) ) |
	                       ( 0x1 << ( 5 * 2 ) ) );
	    /* config TIM3 as quad encoder*/
	    TIM3->SMCR |= 0x03;
	    TIM3->CCMR1 |= (0x01 << 8);
	    TIM3->ARR=8*NPT_CODEUR;
	    /* enable TIM3 encoder mode */
	    TIM3->CR1 |= TIM_CR1_CEN;
}

void conf_dma()
{
	/* Enable clock pour la DMA periph. */
	RCC->AHB1ENR |= (RCC_AHB1ENR_DMA1EN);
	/* Uart2 Tx est sur DMA1 Stream 6 Channel 4 */
	DMA1_Stream6->CR |= DMA_SxCR_CHSEL_2;
	DMA1_Stream6->CR &= ~(DMA_SxCR_CHSEL_0 + DMA_SxCR_CHSEL_1);
	/* Memory increment*/
	DMA1_Stream6->CR |= 1 << 10;
	/* transfer memoie to periph */
	DMA1_Stream6->CR |= 1 << 6;
	/* flow transfer control by DMA controller  */
	//DMA1_Stream6->CR |= 1 << 5;
	/* Set size of transfert */
	DMA1_Stream6->NDTR=19;
	/* set address od periph*/
	DMA1_Stream6->PAR = (uint32_t) &USART2->DR;
	/* set addres of memory */
	DMA1_Stream6->M0AR = tab_dma;

}

void conf_uart()
{
	/* Enable clock USART2 Utilise par Nucleo-F401 To bridge USB */
	RCC->APB1ENR  |=  ( RCC_APB1ENR_USART2EN );
	RCC->AHB1ENR  |= ( RCC_AHB1ENR_GPIOAEN );
    /* configuration de GPIOA2 et A3 pour utilisation UART2 mode AF7 */
	GPIOA->MODER    &= ~( ( 0x3 << ( 2 * 2 ) ) |
                          ( 0x3 << ( 3 * 2 ) ) );
    GPIOA->MODER    |=  ( ( 0x2 << ( 2 * 2 ) ) |
                          ( 0x2 << ( 3 * 2 ) ) );
    GPIOA->OTYPER   &= ~( ( 0x1 << 2 ) |
                          ( 0x1 << 3 ) );
    GPIOA->OSPEEDR  &= ~( ( 0x3 << ( 2 * 2 ) ) |
                          ( 0x3 << ( 3 * 2 ) ) );
    GPIOA->OSPEEDR  |=  ( ( 0x2 << ( 2 * 2 ) ) |
                          ( 0x2 << ( 3 * 2 ) ) );
    GPIOA->AFR[0] &= ~( ( 0xF << ( 2 * 4 ) ) );
    GPIOA->AFR[0] |=  ( ( 0x7 << ( 2 * 4 ) ) );
    GPIOA->AFR[0] &= ~( ( 0xF << ( 3 * 4 ) ) );
    GPIOA->AFR[0] |=  ( ( 0x7 << ( 3 * 4 ) ) );
    /* config baud rate,... */
    // Enable the USART peripheral.
      USART2->CR1 |= ( USART_CR1_RE | USART_CR1_TE | USART_CR1_UE );
      // baud BRR=fclk/(baud_rate*8*(2-OVER8)) => 115200 et 42MHz => 22.786 => mantise=22 et Fraction= 0.786*16=13
      USART2->BRR = ( ( ( 22 ) << USART_BRR_DIV_Mantissa_Pos ) |
                      ( (12 ) << USART_BRR_DIV_Fraction_Pos ) );
      USART2->CR1|=USART_CR1_RXNEIE; // autorise Interruption su receive
      USART2->CR3 |= USART_CR3_DMAT;
      NVIC_EnableIRQ(USART2_IRQn);
}

void conf_L6474()
{
	// On doit configurer le SPI et les GPIO de communnication
	// On utilisera un timer pour generer la frequence de l'horloge de rotation step clock

	//Conifg SPI
	  RCC->APB2ENR |= (1<<12);  // Enable SPI1 CLock
	  SPI1->CR1 |= (1<<0)|(1<<1)|(1<<2);   // CPOL=1, CPHA=1, Master Mode
	  SPI1->CR1 |= (5<<3);  // BR[2:0] = 101: fPCLK/64, PCLK2 = 80MHz, SPI clk = 5MHz
	  SPI1->CR1 &= ~(1<<7);  // LSBFIRST = 0, MSB first
	  //SPI1->CR1 |= (1<<8) | (1<<9);  // SSM=1, SSi=1 -> Software Slave Management
	  SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
	  SPI1->CR1 &= ~(1<<10);  // RXONLY = 0, full-duplex
	  SPI1->CR1 &= ~(1<<11);  // DFF=0, 8 bit data
	  SPI1->CR1 |=(1<<6);//SPI Enable
	  SPI1->CR2 = 0;
	// GPIO for SPI
	  //PORT A
	  RCC->AHB1ENR |= (1<<0);  // Enable GPIO Clock
      GPIOA->MODER |= (2<<10)|(2<<12)|(2<<14);  // Alternate functions for PA5, PA6, PA7
	  GPIOA->OSPEEDR |= (3<<10)|(3<<12)|(3<<14);  // HIGH Speed for PA5, PA6, PA7
	  GPIOA->AFR[0] |= (5<<20)|(5<<24)|(5<<28);   // AF5(SPI1) for PA5, PA6, PA7
	  //PORT B
	  RCC->AHB1ENR |= (1<<1);  		// Enable GPIO Clock
	  GPIOB->MODER |= (1<<12); 		//Output PB6
	  GPIOB->OSPEEDR |= (3<<12);	//HIGH Speed for PB6
	//Config GPIO
	  GPIOA->MODER |= (1<<16)|(1<<18); // Output PA8, PA9
	  GPIOA->MODER &= ~( 0x3 << ( 2 * 10 ) ); //Input PA10
	  GPIOA->OSPEEDR |= (3<<20); // HIGH speed for PA10 pas necessaire
	  //PORT C
	  RCC->AHB1ENR |= (1<<2);  		// Enable GPIO Clock
	  GPIOC->MODER |= (1<<14);  // Alternate functions for PC7
	  GPIOC->OSPEEDR |= (3<<14); // HIGH speed for PC7
}

void conf_Tech(unsigned short freq_ech)
{
	// On configure TIM10 pour generer ine interruption toute les 2 ms (Tech) qui pourrait etre reglable par la suite
	/*Enable clock for TIM10 */
	RCC->APB2ENR  |=  ( RCC_APB2ENR_TIM10EN );
	// assurer que TIM10 est a l'arret
	TIM10->CR1 &= ~(TIM_CR1_CEN);
	// On resette ce beau monde
    RCC->APB2RSTR |=  (RCC_APB2RSTR_TIM10RST);
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM10RST);
    // Prescaler
    TIM10->PSC   = 0;
    //Autoreload
    TIM10->ARR =(FOSC/freq_ech);
    // Send an update event to reset the timer and apply settings.
    TIM10->EGR  |= TIM_EGR_UG;
    // Enable the hardware interrupt.
    TIM10->DIER |= TIM_DIER_UIE;
	// Enable the NVIC interrupt for TIM2.
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0x03);
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    // Enable the timer.
    TIM10->CR1  |= TIM_CR1_CEN;
}

void conf_TStep()
{
	// On configure TIM5 pour generer une interruption au bout de TIM5_ARR pour faire un pulse sur la patte STEP
	/*Enable clock for TIM5 - horloge de TIM5 = 42MHz*/
	RCC->APB1ENR  |=  ( RCC_APB1ENR_TIM5EN );
	// assurer que TIM5 est a l'arret
	TIM5->CR1 &= ~(TIM_CR1_CEN);

	// On resette ce beau monde
    RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM5RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM5RST);
    // Prescaler
    TIM5->PSC   = 0;
    //Autoreload
    TIM5->ARR =500000;
    // Send an update event to reset the timer and apply settings.
    TIM5->EGR  |= TIM_EGR_UG;
    // Enable the hardware interrupt.
    TIM5->DIER |= TIM_DIER_UIE;
	// Enable the NVIC interrupt for TIM5.
	NVIC_SetPriority(TIM5_IRQn, 0x01);
	NVIC_EnableIRQ(TIM5_IRQn);
    // Enable the timer.
	TIM5->CR1 |= TIM_CR1_ARPE;
    TIM5->CR1  |= TIM_CR1_CEN;
}


// override _write de syscalls pour utiliser printf...
int _write( int handle, char* data, int size ) {
  int count = size;
  while( count-- ) {
      while( !( USART2->SR & USART_SR_TXE ) ) {};
      USART2->DR = *data++;

  }
  return size;
}
