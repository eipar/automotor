/*
 * uart.h
 *
 *  Created on: 31 may. 2020
 *      Author: Francisco
 */

#ifndef UART_H_
#define UART_H_

void Set_UART0_LIN(void);
void LIN_Send(uint32_t);

//defines for setting the uart
#define UART_LIN 	LPC_UART0
#define PORT_0	 	0
#define PIN_2_TXD0	2
#define PIN_3_RXD0	3

//defines for cases of LIN_Send
#define LONG_ZERO	0
#define SYNC_BREAK	1
#define SLAVE_ID	2

extern volatile int send_uart_flag;

#endif /* UART_H_ */
