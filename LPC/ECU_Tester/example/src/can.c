/*
 * @brief CAN example
 * This example show how to use CAN peripheral
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */
#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define CAN_CTRL_NO         1
#define LPC_CAN             (LPC_CAN2)

#define CAN_TX_MSG_STD_ID (0x200)
#define CAN_TX_MSG_REMOTE_STD_ID (0x300)

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

CAN_STD_ID_ENTRY_T SFFSection[] = {
	{CAN_CTRL_NO, 0, 0x30},
	{CAN_CTRL_NO, 0, 0x50},
	{CAN_CTRL_NO, 0, 0x70},
	{CAN_CTRL_NO, 0, 0x90},
	{CAN_CTRL_NO, 0, 0xB0},
};
CAN_STD_ID_RANGE_ENTRY_T SffGrpSection[] = {
	{{CAN_CTRL_NO, 0, 0x100}, {CAN_CTRL_NO, 0, 0x200}},
	{{CAN_CTRL_NO, 0, 0x300}, {CAN_CTRL_NO, 0, 0x400}},
	{{CAN_CTRL_NO, 0, 0x500}, {CAN_CTRL_NO, 0, 0x580}},
};

CANAF_LUT_T AFSections = {
	NULL, 0,
	SFFSection, sizeof(SFFSection) / sizeof(CAN_STD_ID_ENTRY_T),
	SffGrpSection, sizeof(SffGrpSection) / sizeof(CAN_STD_ID_RANGE_ENTRY_T),
	NULL, 0,
	NULL, 0,
};

uint8_t can_acc;

/* Setup AF LUT */
static void SetupAFLUT(void)
{
	Chip_CAN_SetAFLUT(LPC_CANAF, LPC_CANAF_RAM, &AFSections);
}

void CAN_IRQHandler(void)
{
	uint32_t IntStatus;
	CAN_MSG_T RcvMsgBuf;
	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN);


	/* New Message came */
	if (IntStatus & CAN_ICR_RI){
		Chip_CAN_Receive(LPC_CAN, &RcvMsgBuf);

		if(RcvMsgBuf.ID == 0x200){
			Board_LED_Toggle(0);
		}

	}

}

int main(void)
{
	CAN_BUFFER_ID_T   TxBuf;
	CAN_MSG_T SendMsgBuf;

	SystemCoreClockUpdate();
	Board_Init();
	Board_LED_Set(0, true);

	Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);
	Chip_CAN_SetBitRate(LPC_CAN, 500000);
	Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);
	SetupAFLUT();
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_NORMAL_MODE);
	NVIC_EnableIRQ(CAN_IRQn);

	TxBuf = CAN_BUFFER_1;

	while (1){

		for(int i = 0; i<50000000; i++);

		//Envio pedido de diagnostico
		SendMsgBuf.ID = 0x30;
		SendMsgBuf.DLC = 4;
		SendMsgBuf.Type = 0;
		SendMsgBuf.Data[0] = 0x01;
		SendMsgBuf.Data[1] = 0x02;
		SendMsgBuf.Data[2] = 0x03;
		SendMsgBuf.Data[3] = 0x04;

		Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);

		for(int i = 0; i<50000000; i++);

		switch(can_acc){
			case 3:
				can_acc = 15;
				break;
			case 15:
				can_acc = 35;
				break;
			case 35:
				can_acc = 65;
				break;
			case 65:
				can_acc = 90;
				break;
			case 90:
				can_acc = 3;
				break;
			default:
				can_acc = 3;
				break;
		}

		//Envio aceleracion
		SendMsgBuf.ID = 0x50;
		SendMsgBuf.DLC = 4;
		SendMsgBuf.Type = 0;
		SendMsgBuf.Data[0] = can_acc;
		SendMsgBuf.Data[1] = 0x55;
		SendMsgBuf.Data[2] = 0x55;
		SendMsgBuf.Data[3] = 0x55;

		Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);

	}
}

/**
 * @}
 */

