/*===============================================================================
 PROBLEMA 2.5a
 Hacer parpadear un LED en P0.22, con demora por SysTick, utilizando drivers.
 Versión: 0.1
 Curso: R4054
 Fecha: 27/04/13
 Ing. Ruben M. Lozano
 ==============================================================================*/

#include <lpc17xx_gpio.h>
#include <lpc17xx_systick.h>
#define LED_BIT_MASK  (1 << 22 )
#define LED_PORT 0
#define SALIDA 1

volatile unsigned long SysTickCnt; //Contador de interrupciones

void SysTick_Handler (void);       //Prototipo de la ISR

void Delay (unsigned long tick);   //Prototipo de la función Delay

void SysTick_Handler (void)        //ISR (Interrupt Service Routine)
 {
   SysTickCnt++;
 }

void Delay (unsigned long tick) //Función Delay
 {
   unsigned long systickcnt;

   systickcnt = SysTickCnt;
   while ((SysTickCnt - systickcnt) < tick);
 }


int main(void)
{
	GPIO_SetDir(LED_PORT, LED_BIT_MASK , SALIDA);  	//Configuro el pin como salida

	SYSTICK_InternalInit(100);    	//Inicializo el System Tick para intervalos de 100ms

	SYSTICK_IntCmd(ENABLE);			//Habilito la interrupción  del System Tick

	SYSTICK_Cmd(ENABLE);			//Habilito el System Tick Counter

	while(1)
		{
		GPIO_SetValue(LED_PORT, LED_BIT_MASK ); //coloco un "1" en el pin
		Delay (10);                    // demora de 1 segundo (10 x 100ms)
		GPIO_ClearValue(LED_PORT, LED_BIT_MASK );        //coloco un "0" en el pin
		Delay (10);                    // demora de 1 segundo (10 x 100ms)
		}

	return 0 ;
}

