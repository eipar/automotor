/*
===============================================================================
 Name        : ecu.c
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
#include "ecu.h"

// TODO: insert other definitions and declarations here
volatile int led_flag;
volatile int send_uart_flag;
volatile int lin_master_ask_moment;
volatile int lin_master_write_moment;
volatile int rx_fifo_full;
volatile uint8_t zero_long_sent;

volatile uint8_t lin_acc;
volatile uint8_t flag_env_diag, flag_transmit_ready, flag_env_acc;
volatile uint8_t can_acc;

int main(void) {

	CAN_BUFFER_ID_T   TxBuf;
	CAN_MSG_T SendMsgBuf;
	uint8_t lin_temp_asked, lin_diag_asked;

    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);


    // TODO: insert code here
    int8_t data_received[CANT_DATOS];


    Set_Systick(INTS_PER_SECOND);

    Set_UART0_LIN();
    TIMER0_Init();

    lin_state_machine_ask 	= LIN_START;
    lin_state_machine_write = LIN_START;

    lin_acc = 0;

    CAN_Init();

	//CAN send alive
	SendMsgBuf.ID = CAN_TX_MSG_STD_ID;
	SendMsgBuf.DLC = 4;
	SendMsgBuf.Type = 0;
	SendMsgBuf.Data[0] = 'H';
	SendMsgBuf.Data[1] = 'O';
	SendMsgBuf.Data[2] = 'L';
	SendMsgBuf.Data[3] = 'A';
	TxBuf = CAN_BUFFER_1;
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);

    while(1) {

        if(led_flag) //just a keep alive check
        {
        	led_flag = 0;
        	Board_LED_Toggle(0);
        }

    	if(flag_env_diag & flag_transmit_ready)
    	{
    		//enviar diagnostico via CAN
    		//reenviar lo que envia LIN
    		flag_env_diag = 0;
    		flag_transmit_ready = 0;
    		SendMsgBuf.Data[0] = lin_temp_asked;
    		SendMsgBuf.Data[1] = lin_diag_asked;
    		SendMsgBuf.Data[2] = 0x55;
    		SendMsgBuf.Data[3] = 0xAA;
    		Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);
    	}

    	if(flag_env_acc)
    	{
    		//mandar la aceleracion por LIN
    		lin_acc = can_acc;
    		flag_env_acc = 0;
    	}

        //state machine for asking
        if(lin_master_ask_moment)
        {
        	//state machine for asking status to slave
        	if(lin_master_write_moment && lin_state_machine_write != LIN_START)
        	{
        		lin_state_machine_ask = LIN_START; //wait
        	}
        	else if(lin_state_machine_ask == LIN_START)
			{
				LIN_Send(C_LONG_ZERO);
				lin_state_machine_ask = LIN_SENDING_LONG;
			}
			else if(lin_state_machine_ask == LIN_SENDING_LONG)
			{
				//wait for long zero, think how, this is not uart standard
				if(zero_long_sent) //long zero is sent
				{
					lin_state_machine_ask = LIN_SEND_SYNC;
					zero_long_sent = 0;
					Chip_GPIO_SetPinOutHigh(LPC_GPIO, PORT_0, PIN_2_TXD0);
					for(int i=0;i<2000;i++);
					Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, PIN_2_TXD0, IOCON_FUNC1);
					Chip_UART_TXEnable(UART_LIN);
				}
				else
				{
					lin_state_machine_ask = LIN_SENDING_LONG;
				}
			}
			else if(lin_state_machine_ask == LIN_SEND_SYNC)
			{
				LIN_Send(C_SYNC_BREAK);
				lin_state_machine_ask = LIN_SEND_ID;
			}
			else if(lin_state_machine_ask == LIN_SEND_ID)
			{
				LIN_Send(C_SLAVE_ID_READ);
				lin_state_machine_ask = LIN_WAIT_DATA;
			}
			else if(lin_state_machine_ask == LIN_WAIT_DATA)
			{
				if(rx_fifo_full)
				{
					lin_state_machine_ask = LIN_PROCESS_DATA;
					rx_fifo_full = 0;
				}
				else
				{
					lin_state_machine_ask = LIN_WAIT_DATA;
				}
			}
			else if(lin_state_machine_ask == LIN_PROCESS_DATA)
			{
				for(int i=0;i<CANT_DATOS;i++)
				{
					data_received[i] = PopRx();
				}
				if((data_received[SYNK_BREAK_INX] == SYNC_BREAK) && (data_received[SLAVE_ID_INX] == (int8_t)SLAVE_ID_READ))
				{	//my transmission was right
					//process data here...
					if(check_checksum(data_received[3],data_received[4],data_received[5]))
					{	//if data is ok
						lin_master_ask_moment = 0;
						lin_temp_asked = data_received[3];
						lin_diag_asked = data_received[4];
						if(lin_diag_asked == 0x66)
						{
							Chip_GPIO_SetPinOutHigh(LPC_GPIO, LIN_LED_ALARM_PORT, LIN_LED_1_PIN);
						}
					}
					else
					{	//data is wrong, restart
						lin_master_ask_moment = 1;
						Chip_GPIO_SetPinOutHigh(LPC_GPIO, LIN_LED_ALARM_PORT, LIN_LED_2_PIN);
					}
				}
				else
				{	//my transmission was wrong
					lin_master_ask_moment = 1;
				}
				lin_state_machine_ask = LIN_START; //start again
			}
       	}

        //state machine for writing LIN slaves
        if(lin_master_write_moment)
        {
        	if(lin_master_ask_moment && lin_state_machine_ask != LIN_START)
        	{
        		lin_state_machine_write = LIN_START; //wait
        	}
        	else if(lin_state_machine_write == LIN_START)
        	{
        		LIN_Send(C_LONG_ZERO);
        		lin_state_machine_write = LIN_SENDING_LONG;
        	}
     		else if(lin_state_machine_write == LIN_SENDING_LONG)
        	{
        	//wait for long zero, think how, this is not uart standard
				if(zero_long_sent) //long zero is sent
				{
					lin_state_machine_write = LIN_SEND_SYNC;
					zero_long_sent = 0;
					Chip_GPIO_SetPinOutHigh(LPC_GPIO, PORT_0, PIN_2_TXD0);
					for(int i=0;i<2000;i++);
					Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, PIN_2_TXD0, IOCON_FUNC1);
					Chip_UART_TXEnable(UART_LIN);
				}
        		else
        		{
        			lin_state_machine_write = LIN_SENDING_LONG;
        		}
        	}
        	else if(lin_state_machine_write == LIN_SEND_SYNC)
        	{
        		LIN_Send(C_SYNC_BREAK);
        		lin_state_machine_write = LIN_SEND_ID;
        	}
        	else if(lin_state_machine_write == LIN_SEND_ID)
        	{
        		LIN_Send(C_SLAVE_ID_WRITE);
        		lin_state_machine_write = LIN_SEND_DATA;
        	}
       		//falta enviar mas datos
        	else if(lin_state_machine_write == LIN_SEND_DATA)
        	{
        		PushTx(lin_acc);
        		PushTx(0x43);
        		PushTx(~(lin_acc+0x43));
        		lin_state_machine_write = LIN_WAIT_DATA;
        	}
        	else if(lin_state_machine_write == LIN_WAIT_DATA)
        	{
        		if(rx_fifo_full)
        		{
        			lin_state_machine_write = LIN_PROCESS_DATA;
        			rx_fifo_full = 0;
        		}
        		else
        		{
        			lin_state_machine_write = LIN_WAIT_DATA;
        		}
        	}
        	else if(lin_state_machine_write == LIN_PROCESS_DATA)
        	{
        		for(int i=0;i<CANT_DATOS;i++)
        		{
        			data_received[i] = PopRx();
        		}
        		if((data_received[SYNK_BREAK_INX] == SYNC_BREAK) && (data_received[SLAVE_ID_INX] == (int8_t)SLAVE_ID_WRITE))
        		{	//my transmission was right
        			//process data here...
        			if(check_checksum(data_received[3],data_received[4],data_received[5]))
        			{	//if data is ok
        				lin_master_write_moment = 0;
        			}
        			else
        			{	//data is wrong, restart
        				lin_master_write_moment = 1;
        				Chip_GPIO_SetPinOutHigh(LPC_GPIO, LIN_LED_ALARM_PORT, LIN_LED_2_PIN);
        			}
        		}
        		else
        		{	//my transmission was wrong
        			lin_master_write_moment = 1;
        		}
        		lin_state_machine_write = LIN_START; //start again
        	}
        }
    }
    return 0 ;
}

bool check_checksum(uint8_t data0, uint8_t data1, uint8_t data2)
{
	uint8_t calc_checksum;

	calc_checksum = ~(data0+data1);

	if(calc_checksum == data2) return TRUE;
	else return FALSE;
}
