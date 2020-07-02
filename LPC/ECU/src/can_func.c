/*
 * can_func.c
 *
 *  Created on: 18 jun. 2020
 *      Author: Francisco
 */

#include "can.h"

extern volatile uint8_t flag_env_diag, flag_transmit_ready, flag_env_acc;
extern volatile uint8_t can_acc;

//IDs for the Acceptance Filter AF_LUT
CAN_STD_ID_ENTRY_T SFFSection[] = {
	{CAN_CTRL_NO, 0, 0x30},
	{CAN_CTRL_NO, 0, 0x50},
	{CAN_CTRL_NO, 0, 0x70},
	{CAN_CTRL_NO, 0, 0x90},
	{CAN_CTRL_NO, 0, 0xB0},
};

CANAF_LUT_T AFSections = {
	NULL, 0,
	SFFSection, sizeof(SFFSection) / sizeof(CAN_STD_ID_ENTRY_T),
	NULL, 0,
	NULL, 0,
	NULL, 0,
};


/* Setup AF LUT */
void SetupAFLUT(void)
{
	Chip_CAN_SetAFLUT(LPC_CANAF, LPC_CANAF_RAM, &AFSections);
}

void CAN_Init(void)
{
	//LED de Alarma en CAN
	Chip_IOCON_PinMuxSet(LPC_IOCON, CAN_LED_ALARM_PORT, CAN_LED_ALARM_PIN, IOCON_FUNC0);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, CAN_LED_ALARM_PORT, CAN_LED_ALARM_PIN);

	//Inicialización cAN
    Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);
    Chip_CAN_SetBitRate(LPC_CAN, 500000);
    Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);

	SetupAFLUT();

	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_NORMAL_MODE);

	NVIC_EnableIRQ(CAN_IRQn);

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
			Chip_GPIO_SetPinOutHigh(LPC_GPIO, CAN_LED_ALARM_PORT, CAN_LED_ALARM_PIN);
		}
	}
	else if (IntStatus & CAN_ICR_TI1)
	{
		flag_transmit_ready = 1;
	}
	else
	{
		Chip_GPIO_SetPinOutHigh(LPC_GPIO, CAN_LED_ALARM_PORT, CAN_LED_ALARM_PIN);
	}

}
