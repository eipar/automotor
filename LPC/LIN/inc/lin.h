

#include "systick.h"

#include "uart.h"

//Macros for LIN State Machine
#define LIN_START			0
#define LIN_SENDING_LONG 	1
#define LIN_SEND_SYNC		2
#define LIN_SEND_ID			3
#define LIN_SEND_DATA		4
#define LIN_WAIT_DATA		5

#define LONG_ZERO	0
#define SYNC_BREAK	1
#define SLAVE_ID	2


uint32_t uart0_status;

uint32_t lin_state_machine;
