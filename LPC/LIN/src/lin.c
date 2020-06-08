/*
===============================================================================
 Name        : lin.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "lin.h"

// TODO: insert other definitions and declarations here
volatile int led_flag;
volatile int send_uart_flag;
volatile int lin_master_ask_moment;

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    // TODO: insert code here

    Set_Systick(INTS_PER_SECOND);

    Set_UART0_LIN();

    lin_state_machine = LIN_START;

    while(1) {

        if(led_flag) //just a keep alive check
        {
        	led_flag = 0;
        	Board_LED_Toggle(0);
        }

        if(lin_master_ask_moment)
        {
        	//state machine for asking status to slave
			if(lin_state_machine == LIN_START)
			{
				LIN_Send(LONG_ZERO);
				lin_state_machine = LIN_SENDING_LONG;
			}
			else if(lin_state_machine == LIN_SENDING_LONG)
			{
				//wait for long zero, think how, this is not uart standard
				if(1) //long zero is sent
				{
					lin_state_machine = LIN_SEND_SYNC;
				}
				else
				{
					lin_state_machine = LIN_SENDING_LONG;
				}
			}
			else if(lin_state_machine == LIN_SEND_SYNC)
			{
				LIN_Send(SYNC_BREAK);
				lin_state_machine = LIN_SEND_ID;
			}
			else if(lin_state_machine == LIN_SEND_ID)
			{
				LIN_Send(SLAVE_ID);
				lin_state_machine = LIN_WAIT_DATA;
			}
			else if(lin_state_machine == LIN_WAIT_DATA)
			{
				if(1)//wait for all data
				{
					lin_state_machine = LIN_START;
					lin_master_ask_moment = 0;
				}
			}
       	}

    }
    return 0 ;
}


