#include "board.h"
#include <stdio.h>

#include "sys_config.h"
#include "systick.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	static int tick_led = 1000;
	static int tick_lin_master = 500;

	tick_led--;
	tick_lin_master--;

	if (!tick_led)
	{
		tick_led = 999;
		led_flag = 1;
	}

	if (!tick_lin_master)
	{
		tick_lin_master = 499;
		lin_master_ask_moment = 1;
	}
}

/**
 * @brief   Setup the SysTick timer for desired interrup time
 * @return  Nothing
 */
void Set_Systick_Int_Time(int ints_per_second)
{
	SysTick_Config(SystemCoreClock / ints_per_second);
}

/**
 * @brief   Setup the SysTick
 * @return  Nothing
 */
void Set_Systick(int ints_per_second)
{
	Set_Systick_Int_Time(ints_per_second);
	//Add other configs if needed here
}






