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

#define MAX_TX	20
#define MAX_RX	20
volatile unsigned char BUF_TX[MAX_TX];
volatile unsigned char BUF_RX[MAX_RX];
volatile unsigned char Tx_In,Tx_Out;
volatile unsigned char Rx_In,Rx_Out;

void Set_UART0_LIN(void)
{
	//Init LEDs Alarm for LIN
	Chip_IOCON_PinMuxSet(LPC_IOCON, LIN_LED_ALARM_PORT, LIN_LED_2_PIN, IOCON_FUNC0);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LIN_LED_ALARM_PORT, LIN_LED_2_PIN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, LIN_LED_ALARM_PORT, LIN_LED_1_PIN, IOCON_FUNC0);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LIN_LED_ALARM_PORT, LIN_LED_1_PIN);

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
	//Chip_UART_TXEnable(UART_LIN);

	NVIC_SetPriority(UART0_IRQn, 1);
	NVIC_EnableIRQ(UART0_IRQn);

	//Settings for CS and WAKE of the transceiver
	Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, PIN_LIN_CS,   IOCON_FUNC0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, PIN_LIN_WAKE, IOCON_FUNC0);
	//Always on...
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, PORT_0, PIN_LIN_CS);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, PORT_0, PIN_LIN_WAKE);
}

void UART0_IRQHandler(void)
{
	uint32_t current_int;
	uint32_t uart0_status;
	uint32_t data_received;
	int8_t	 data_to_send;
	static int8_t bytes_received;
	//get the current interruptions
	current_int = (Chip_UART_ReadIntIDReg(UART_LIN) & UART_IIR_INTID_MASK);

	switch(current_int)
	{
		case UART_IIR_INTID_THRE: //the transmition register is empty
			//flag is clean by reading IIR before
			//we can send something else, checking LSR just in case
	   		uart0_status = (Chip_UART_ReadLineStatus(UART_LIN) & UART_LSR_BITMASK);
	   		uart0_status &= 0x20;
	   		if(uart0_status == UART_LSR_THRE)
	   		{
	   			data_to_send = PopTx();
	   			if(data_to_send != -1)
	   			{
	   				Chip_UART_SendByte(UART_LIN, data_to_send);
	   			}
	   		}
			break;
		case UART_IIR_INTID_RDA: //data was received
			data_received = Chip_UART_ReadByte(UART_LIN); //read and clear int flag
			PushRx(data_received);
			bytes_received++;
			if(bytes_received == CANT_DATOS)
			{
				rx_fifo_full = 1;
				bytes_received = 0;
			}
			else
			{
				rx_fifo_full = 0;
			}
			break;
		case UART_IIR_INTID_RLS: //error in the line, identification needed
			uart0_status = Chip_UART_ReadLineStatus(UART_LIN); //clear int flag
			//check failure? maybe later
			Chip_GPIO_SetPinOutHigh(LPC_GPIO, LIN_LED_ALARM_PORT, LIN_LED_2_PIN);
			break;
		case UART_IIR_INTID_CTI: //character timeout, no use
			Chip_GPIO_SetPinOutHigh(LPC_GPIO, LIN_LED_ALARM_PORT, LIN_LED_2_PIN);
			break;
		default:	//just in case
			//can i halt this?
			Chip_GPIO_SetPinOutHigh(LPC_GPIO, LIN_LED_ALARM_PORT, LIN_LED_2_PIN);
			break;
	}

}

void LIN_Send(uint32_t flag)
{
	uint32_t uart0_status;
	switch(flag)
	{
		case C_LONG_ZERO:
			//short zero, replace for long zero
//	   		uart0_status = (Chip_UART_ReadLineStatus(UART_LIN) & UART_LSR_BITMASK);
//	   		uart0_status &= 0x20;
//	   		if(uart0_status == UART_LSR_THRE)
//	   		{
//	   			Chip_UART_SendByte(UART_LIN, 0x00);
//	   		}
			Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, PIN_2_TXD0, IOCON_FUNC0);
			Chip_GPIO_SetPinDIROutput(LPC_GPIO, PORT_0, PIN_2_TXD0);
			Chip_GPIO_SetPinOutLow(LPC_GPIO, PORT_0, PIN_2_TXD0);
			Chip_TIMER_Enable(LPC_TIMER0);
			break;
		case C_SYNC_BREAK:
			//PushTx(SYNC_BREAK);
			uart0_status = (Chip_UART_ReadLineStatus(UART_LIN) & UART_LSR_BITMASK);
			uart0_status &= 0x20;
			if(uart0_status == UART_LSR_THRE)
			{
				Chip_UART_SendByte(UART_LIN, 0x55);
			}
			break;
		case C_SLAVE_ID_READ:
			PushTx(SLAVE_ID_READ);
			break;
		case C_SLAVE_ID_WRITE:
			PushTx(SLAVE_ID_WRITE);
			break;
		default:
			PushTx(0x45);
			break;
	}

}

void PushTx(uint8_t dato)
{
	BUF_TX[Tx_In] = dato;
	Tx_In ++;
	Tx_In %= MAX_TX;
}

int PopTx (void)
{
	int8_t dato = -1;

	if( Tx_In != Tx_Out )
	{
		dato = 	(int8_t ) BUF_TX[Tx_Out];
		Tx_Out ++;
		Tx_Out %= MAX_TX;
	}

	return dato;
}

void PushRx(unsigned char dato)
{
	BUF_RX[Rx_In] = dato;
	Rx_In ++;
	Rx_In %= MAX_RX;
}

int PopRx (void)
{
	int dato = -1;

	if( Rx_In != Rx_Out )
	{
		dato = 	(int ) BUF_RX[Rx_Out];
		Rx_Out ++;
		Rx_Out %= MAX_RX;
	}

	return dato;
}


