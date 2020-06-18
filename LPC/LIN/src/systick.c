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
	static int tick_lin_master_read  = 2500;			//0001;
	static int tick_lin_master_write = 0001;			//2500;
	static int tick_acc = 2000;

	tick_led--;
	tick_lin_master_read--;
	tick_lin_master_write--;
	tick_acc--;

	if (!tick_led)
	{
		tick_led = 999;
		led_flag = 1;
	}

	if (!tick_lin_master_read)
	{
		tick_lin_master_read = 4999;
		lin_master_ask_moment = 1;
	}

	if (!tick_lin_master_write)
	{
		tick_lin_master_write = 4999;
		lin_master_write_moment = 1;
	}

	if(!tick_acc)
	{
		tick_acc = 2000;
		switch(lin_acc)
		{
			case 0:
				lin_acc = 8;
				break;
			case 8:
				lin_acc = 35;
				break;
			case 35:
				lin_acc = 65;
				break;
			case 65:
				lin_acc = 95;
				break;
			case 95:
				lin_acc = 0;
				break;
			default:
				lin_acc = 8;
				break;
		}
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






