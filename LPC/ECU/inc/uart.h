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
#define PIN_LIN_CS		21
#define PIN_LIN_WAKE	22

//defines for cases of LIN_Send
#define C_LONG_ZERO		 0
#define C_SYNC_BREAK	 1
#define C_SLAVE_ID_READ  2
#define C_SLAVE_ID_WRITE 3

//defines to send
#define SYNC_BREAK		0x55
#define SLAVE_ID_READ	0xC1
#define SLAVE_ID_WRITE	0x80

extern volatile int send_uart_flag;

void PushTx(unsigned char);
int PopTx (void);
void PushRx(unsigned char);
int PopRx (void);

#define CANT_DATOS 		6
extern volatile int rx_fifo_full;

#endif /* UART_H_ */
