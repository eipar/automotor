/*
===============================================================================
 Name        : can.c
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

#include "can.h"

volatile uint8_t flag_env_diag, flag_transmit_ready, flag_env_acc;
volatile uint8_t can_acc;

/** Main Program **/
int main(void) {
	CAN_BUFFER_ID_T   TxBuf;
	CAN_MSG_T SendMsgBuf;
	uint8_t aceleraciones;

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

    	if(flag_env_diag & flag_transmit_ready)
    	{
    		//enviar diagnostico via CAN
    		//reenviar lo que envia LIN
    		flag_env_diag = 0;
    		flag_transmit_ready = 0;
    		SendMsgBuf.Data[0] = 0x55;
    		SendMsgBuf.Data[1] = 0x55;
    		SendMsgBuf.Data[2] = 0x55;
    		SendMsgBuf.Data[3] = 0x55;
    		Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);
    	}

    	if(flag_env_acc)
    	{
    		//mandar la aceleracion por LIN
    		aceleraciones++;
    	}


    }
    return 0 ;
}

/** Handlers **/
void CAN_IRQHandler(void)
{
	uint32_t IntStatus;
	CAN_MSG_T RcvMsgBuf;
	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN);

	/* New Message came */
	if (IntStatus & CAN_ICR_RI)
	{
		Chip_CAN_Receive(LPC_CAN, &RcvMsgBuf);
		if(RcvMsgBuf.ID == CAN_ID_DIAG)
		{
			//llego un pedido de mandar el diagnostico
			flag_env_diag = 1;
		}
		else if(RcvMsgBuf.ID == CAN_ID_ACC)
		{
			//llego la aceleracion por CAN
			flag_env_acc = 1;
			can_acc = RcvMsgBuf.Data[0];
		}
		else
		{
			//mensaje es falopa, algo malio sal
			Board_LED_Toggle(0);
		}
	}
	/* New Message sent */
	if (IntStatus & CAN_ICR_TI1)
	{
		flag_transmit_ready = 1;
	}

}

/**
 * @}
 */
