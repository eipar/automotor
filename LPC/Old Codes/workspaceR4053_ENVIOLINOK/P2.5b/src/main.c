/*===============================================================================
 PROBLEMA 2.5b
 Hacer parpadear un LED en P0.22, con demora por TIMER 0, utilizando drivers.
 Versión: 0.1
 Fecha: 27/04/13
 Ing. Ruben M. Lozano
 ==============================================================================*/
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"
#define LED_BIT_MASK  (1 << 22 )
#define LED_PORT 0
#define SALIDA 1

 TIM_TIMERCFG_Type TIM_ConfigStruct;
 TIM_MATCHCFG_Type TIM_MatchConfigStruct ;
 uint8_t volatile  X=0;

 void TIMER0_IRQHandler(void); // Prototipo de la ISR TIMER 0
 void Set_TIMER_0 (void);      // Prototipo de Set_TIMER_0

  void TIMER0_IRQHandler(void)
 {
	     //Verificamos para el Timer 0, cual fué la fuente de interrupción.
	     //En este ejemplo, la única habilitada es Match 0.
         if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)== SET)
         {
        	if( X++)
        		{GPIO_ClearValue(LED_PORT, LED_BIT_MASK );        //coloco un "0" en el pin
        	    X=0;
        		}
        	else
        		GPIO_SetValue(LED_PORT, LED_BIT_MASK ); //coloco un "1" en el pin
         }
         TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
 }

 int main (void)
	  {
	     GPIO_SetDir(LED_PORT, LED_BIT_MASK , SALIDA);  	//Configuro el pin como salida

	     Set_TIMER_0 ();
         while (1)
         {

         }
        return 1;
 }

void Set_TIMER_0 (void)
{
	         // Cargo los campos de la estructura para un prescaler count time de 100uS
	         TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL; //valor en microsegundos.
	         TIM_ConfigStruct.PrescaleValue  = 100;
	         // Cargo los registros de configuración del TIMER 0
	         TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);

	         // use channel 0, MR0
	         TIM_MatchConfigStruct.MatchChannel = 0;
	         // Enable interrupt when MR0 matches the value in TC register
	         TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	         //Enable reset on MR0: TIMER will reset if MR0 matches it
	         TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	         //Stop on MR0 if MR0 matches it
	         TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	         //Toggle MR0.0 pin if MR0 matches it
	         TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
	         // Set Match value, count value of 5000 (5000 * 100uS = 500mS de semi-período =  1 Hz)
	         TIM_MatchConfigStruct.MatchValue   = 5000;
	         // Cargo los registros de MATCH del TIMER 0
	         TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);

	        /* preemption = 1, sub-priority = 1 */
	        NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
	        /* Enable interrupt for timer 0 */
	         NVIC_EnableIRQ(TIMER0_IRQn);
	         // To start timer 0
	         TIM_Cmd(LPC_TIM0,ENABLE);
}



