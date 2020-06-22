/*
 * can.h
 *
 *  Created on: 18 jun. 2020
 *      Author: Francisco
 */

#ifndef CAN_H_
#define CAN_H_

#include "board.h"

#define CAN_CTRL_NO         1
#define LPC_CAN				LPC_CAN2
#define AF_LUT_USED         1
#define CAN_TX_MSG_STD_ID 	(0x200)
#define CAN_ID_DIAG			(0x30)
#define CAN_ID_ACC			(0x50)


/** Function Declaration **/
void PrintCANErrorInfo(uint32_t);
void PrintCANMsg(CAN_MSG_T *);
void PrintAFLUT(void);
void SetupAFLUT(void);
void CAN_Init(void);



#endif /* CAN_H_ */
