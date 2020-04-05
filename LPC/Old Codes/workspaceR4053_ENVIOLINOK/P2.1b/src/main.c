/*
===============================================================================
 PROBLEMA 2.1-b
 Hacer parpadear un LED en P0.22, con demora por software, utilizando registros.
 Versión: 0.1
 Curso: R4054
 Fecha: 27/04/13
 Ing. Ruben M. Lozano
 ===============================================================================
*/
#include "LPC17xx.h"
#define LED_BIT_MASK  (1 << 22 )

unsigned int i=0;

int main(void)
{
	LPC_PINCON->PINSEL1 &=~(1<<12);	//Seleciono P0.22 como GPIO
	LPC_PINCON->PINSEL1 &=~(1<<13);	//Seleciono P0.22 como GPIO

	LPC_GPIO0->FIODIR |=(1<<22);	//Configuro el pin como Salida

	while(1)
		{
		LPC_GPIO0->FIOSET |= LED_BIT_MASK;   //coloco un "1" en el pin
		for (i=0;i<50000;i++){}         //demora

		LPC_GPIO0->FIOCLR |= LED_BIT_MASK;   //coloco un "0" en el pin
		for (i=0;i<50000;i++){}         //demora
		}

	return 0 ;
}

