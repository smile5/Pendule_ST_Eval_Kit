/*
 * l6474.c
 *
 *  Created on: 7 mars 2023
 *      Author: xgaltier
 */
#include "l6474.h"

void Reset_Low (void)
{
	GPIOA->BSRR |= (1<<9)<<16;
}

void Reset_High (void)
{
	GPIOA->BSRR |= (1<<9);
}

void Cs_Low (void)
{
	GPIOB->BSRR |= (1<<6)<<16;
}

void Cs_High (void)
{
	GPIOB->BSRR |= (1<<6);
}

void DIR_High(void)
{
	GPIOA->BSRR |= (1<<8);
}

void DIR_Low(void)
{
	GPIOA->BSRR |= (1<<8)<<16;
}

void L6474_Enable(void)
{
	unsigned char tab[1];
	tab[0]=0xB8; //Enable
	Spi_Transmit(tab,1);
}

void L6474_Disable(void)
{
	unsigned char tab[1];
	tab[0]=0xA8; //Enable
	Spi_Transmit(tab,1);
}

void L6474_OverCurrent(float current)
{
	//fixe la valeur de over current max ds les mosfet du L6474 par pas de 375mA
	unsigned char u_current;
	unsigned char tab[2];
	if (current>6)
	{
		current=6.0;
	}
	else if (current<0)
	{
		current=380e-3;
	}
	u_current=((unsigned char) (current/375e-3))-1;
	tab[0]=0x13; //OCD_TH
	tab[1]=u_current;
	Spi_Transmit(tab,2);
}

void L6474_SetCurrent(float current)
{
	//fixe la valeur max de la regul en courant du L6474 paer pas de 31.25mA
	unsigned char u_current;
	unsigned char tab[2];
	if (current>4)
	{
		current=4.0;
	}
	else if (current<0)
	{
		current=32e-3;
	}
	u_current=((unsigned char) (current/31.25e-3))-1;
	tab[0]=0x09; //TVAl
	tab[1]=u_current;
	Spi_Transmit(tab,2);
}

void L6474_ResetABSPos(void)
{
	unsigned char tab[4];
	tab[0]=0x01; //ABS_POS
	tab[1]=0;
	tab[2]=0;
	tab[3]=0;
	Spi_Transmit(tab,4);
}

void L6474_SetStepSize(unsigned char step)
{

	// step =1 full step : step = 2 1/2 pas step =4 1/4 pas step=8 1/8 de pas step =16 1/16 de pas
	unsigned char tab[4];
	tab[0]=0x16; //Step Mode
	//disable before change step + reset ABS_POS
	switch (step)
	{
		case 1:
			L6474_Disable();
			tab[1]=0x88;
			Spi_Transmit(tab,2);
			L6474_ResetABSPos();
		break;
		case 2:
			L6474_Disable();
			tab[1]=0x89;
			Spi_Transmit(tab,2);
			L6474_ResetABSPos();
		break;
		case 4:
			L6474_Disable();
			tab[1]=0x8A;
			Spi_Transmit(tab,2);
			L6474_ResetABSPos();
		break;
		case 8:
			L6474_Disable();
			tab[1]=0x8B;
			Spi_Transmit(tab,2);
			L6474_ResetABSPos();
		break;
		case 16:
			L6474_Disable();
			tab[1]=0x8C;
			Spi_Transmit(tab,2);
			L6474_ResetABSPos();
		break;
		default:
		break;
	}
}

void L6474_SetALARM(unsigned char alarm_flag)
{
	unsigned char tab[2];
	tab[0]=0x17; // ALARM
	tab[1]=alarm_flag;
	Spi_Transmit(tab,2);
}

unsigned short L6474_GetCONFIG()
{
		unsigned char tab[4];
		unsigned short result;
		tab[0]=0x38;
		Spi_Transmit(tab,1);
		Spi_Receive(tab,2);
		result=(tab[0]<<8)+(tab[1]);
		return result;

}

unsigned char L6474_GetALARM()
{
	unsigned char tab[4];
	unsigned char result;
	tab[0]=0x37;
	Spi_Transmit(tab,1);
	Spi_Receive(tab,1);
	return tab[0];
}

unsigned char L6474_GetSTEPMODE(void)
{
	unsigned char tab[4],tmp;
	unsigned char result;
	tab[0]=0x36;
	Spi_Transmit(tab,1);
	Spi_Receive(tab,1);
	tmp=tab[0] & 0x07;
	switch (tmp)
	{
			case 0:
				result=1;
			break;
			case 1:
				result=2;
			break;
			case 2:
				result=4;
			break;
			case 3:
				result=8;
			break;
			default:
				result=16;
			break;
		}
	return result;
}

float L6474_GetOCD_TH(void)
{
	unsigned char tab[4];
	float result;
	tab[0]=0x33;
	Spi_Transmit(tab,1);
	Spi_Receive(tab,1);
	result= tab[0]*375e-3+375e-3;
	return result;
}

float L6474_GetTVAL(void)
{
	unsigned char tab[4];
	float result;
	tab[0]=0x29;
	Spi_Transmit(tab,1);
	Spi_Receive(tab,1);
	result= tab[0]*31.25e-3+31.25e-3;
	return result;
}

long L6474_GetABSPOS(void)
{
	unsigned char tab[4];
	long result;
	tab[0]=0x21;
	Spi_Transmit(tab,1);
	Spi_Receive(tab,3);
	if (tab[2] & 0x20)
	{
		result=(0xFF << 24) +((tab[0]|0xC0)<<16)+(tab[1]<<8)+tab[2];
	}
	else
	{
		result=(tab[0]<<16)+(tab[1]<<8)+(tab[2]);
	}
	return result;
}


void L6474_GetStatus(unsigned char *data)
{
	unsigned char *tab;
	tab=data;
	*tab=0xD0;
	Spi_Transmit(tab,1);
	Spi_Receive(tab,2);
}

void Spi_Receive (unsigned char *data, unsigned short size)
{
	/************** STEPS TO FOLLOW *****************
	1. Wait for the BSY bit to reset in Status Register
	2. Send some Dummy data before reading the DATA
	3. Wait for the RXNE bit to Set in the status Register
	4. Read data from Data Register
	 ************************************************/
	while (size)
	{
		Cs_Low();

		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		SPI1->DR = 0x00;  // send dummy data
		//Cs_Low();
		while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
		*data++ = (SPI1->DR);
		Cs_High();
		size--;
		//Cs_High();

	}
}

void Spi_Transmit (unsigned char *data, unsigned short size)
{
	/************** STEPS TO FOLLOW *****************
	1. Wait for the TXE bit to set in the Status Register
	2. Write the data to the Data Register
	3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
	4. Clear the Overrun flag by reading DR and SR
	 ************************************************/
	unsigned char temp;
	unsigned short i=0;
	while (i<size)
	{
		Cs_Low();

		while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
		SPI1->DR = data[i];  // load the data into the Data Register
		i++;
		while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		//  Clear the Overrun flag by reading DR and SR
		temp = SPI1->DR;
		temp = SPI1->SR;
		Cs_High();

	}

}

void Spi_Transmit1 (unsigned char data)
{
	/************** STEPS TO FOLLOW *****************
	1. Wait for the TXE bit to set in the Status Register
	2. Write the data to the Data Register
	3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
	4. Clear the Overrun flag by reading DR and SR
	 ************************************************/
	unsigned char temp;
	Cs_Low();

		while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
		SPI1->DR = data;  // load the data into the Data Register

		while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		//  Clear the Overrun flag by reading DR and SR
		 temp = SPI1->DR;
		temp = SPI1->SR;
		Cs_High();
		delay_ms(1);
}
