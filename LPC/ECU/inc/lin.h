

#include "systick.h"

#include "uart.h"
#include "timer.h"

//Macros for LIN State Machine
#define LIN_START			0
#define LIN_SENDING_LONG 	1
#define LIN_SEND_SYNC		2
#define LIN_SEND_ID			3
#define LIN_SEND_DATA		4
#define LIN_WAIT_DATA		5
#define LIN_PROCESS_DATA	6

#define SYNK_BREAK_INX 	1
#define SLAVE_ID_INX	2

uint32_t uart0_status;

uint32_t lin_state_machine_ask;
uint32_t lin_state_machine_write;

bool check_checksum(uint8_t, uint8_t, uint8_t);
