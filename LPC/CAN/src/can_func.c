/*
 * can_func.c
 *
 *  Created on: 18 jun. 2020
 *      Author: Francisco
 */

#include "can.h"

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
    Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);
    Chip_CAN_SetBitRate(LPC_CAN, 500000);
    Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);

	SetupAFLUT();

	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_NORMAL_MODE);

	NVIC_EnableIRQ(CAN_IRQn);
}
