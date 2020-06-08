/*
 * uart0.c
 *
 *  Created on: 7 jun. 2020
 *      Author: Francisco
 */

#include "board.h"
#include <stdio.h>

#include "sys_config.h"
#include "uart.h"

void Set_UART0_LIN(void)
{
	//Temporary registers for UART0 configuration for LIN
	uint32_t uart_config_data,uart_config_int;
	//Configure pins function
	Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, PIN_2_TXD0, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, PIN_3_RXD0, IOCON_FUNC1);
	//Start UART with default configurations
	Chip_UART_Init(UART_LIN);
	//UART Baudrate set to 9600
	Chip_UART_SetBaud(UART_LIN, 9600);
	//UART set to 8N1
	uart_config_data = (UART_LCR_WLEN8|UART_LCR_SBS_1BIT|UART_LCR_PARITY_DIS);
	Chip_UART_ConfigData(UART_LIN, uart_config_data);

	//UART interrupts when is ready to send
	uart_config_int =  UART_IER_RBRINT | UART_IER_THREINT | UART_IER_RLSINT;
	Chip_UART_IntEnable(UART_LIN,uart_config_int);

	//Activate Tx
	Chip_UART_TXEnable(UART_LIN);

	NVIC_SetPriority(UART0_IRQn, 1);
	NVIC_EnableIRQ(UART0_IRQn);

}

void UART0_IRQHandler(void)
{
	uint32_t current_int;
	uint32_t uart_status;
	uint32_t data_received;
	//get the current interruptions
	current_int = (Chip_UART_ReadIntIDReg(UART_LIN) & UART_IIR_INTID_MASK);

	switch(current_int)
	{
		case UART_IIR_INTID_THRE: //the transmition register is empty
			//flag is clean by reading IIR before
			//we can send something else
			send_uart_flag = 1;
			break;
		case UART_IIR_INTID_RDA: //data was received
			data_received = Chip_UART_ReadByte(UART_LIN); //read and clear int flag
			//send data somewhere...
			break;
		case UART_IIR_INTID_RLS: //error in the line, identification needed
			uart_status = Chip_UART_ReadLineStatus(UART_LIN); //clear int flag
			//check failure? maybe later
			break;
		case UART_IIR_INTID_CTI: //character timeout, no use

			break;
		default:	//just in case
			//can i halt this?
			break;
	}

}

void LIN_Send(uint32_t flag)
{
	uint32_t uart0_status;
	switch(flag)
	{
		case LONG_ZERO:
			//short zero, replace for long zero
	   		uart0_status = (Chip_UART_ReadLineStatus(UART_LIN) & UART_LSR_BITMASK);
	   		uart0_status &= 0x20;
	   		if(uart0_status == UART_LSR_THRE)
	   		{
	   			Chip_UART_SendByte(UART_LIN, 0x00);
	   		}
			break;
		case SYNC_BREAK:
	   		uart0_status = (Chip_UART_ReadLineStatus(UART_LIN) & UART_LSR_BITMASK);
	   		uart0_status &= 0x20;
	   		if(uart0_status == UART_LSR_THRE)
	   		{
	   			Chip_UART_SendByte(UART_LIN, 0x55);
	   		}
			break;
		case SLAVE_ID:
	   		uart0_status = (Chip_UART_ReadLineStatus(UART_LIN) & UART_LSR_BITMASK);
	   		uart0_status &= 0x20;
	   		if(uart0_status == UART_LSR_THRE)
	   		{
	   			Chip_UART_SendByte(UART_LIN, 0xAA);
	   		}
			break;
		default:
	   		uart0_status = (Chip_UART_ReadLineStatus(UART_LIN) & UART_LSR_BITMASK);
	   		uart0_status &= 0x20;
	   		if(uart0_status == UART_LSR_THRE)
	   		{
	   			Chip_UART_SendByte(UART_LIN, 0x45);
	   		}
			break;
	}

}









