/*
 * Electrónica Aplicada al Automotor
 * TP2
 * CAN / M-LIN protocol
 *
 * @author Badenas, Francisco
 * 		   Ipar, Eugenia

 */

#include "board.h"

#define TICKRATE 0.5

uint8_t ticks = 1;
uint8_t flag_LED = 0;

#if defined(CHIP_LPC175X_6X)
#define CAN_CTRL_NO         1
#define LPC_CAN             (LPC_CAN2)
#endif

#define AF_LUT_USED         1
#if AF_LUT_USED
#define FULL_CAN_AF_USED    1
#endif

#define CAN_TX_MSG_STD_ID (0x200)
//#define CAN_TX_MSG_REMOTE_STD_ID (0x300)
//#define CAN_TX_MSG_EXT_ID (0x10000200)
#define CAN_RX_MSG_ID (0x100)

/*** Look-Up Table ***/
#if AF_LUT_USED
#if FULL_CAN_AF_USED
CAN_STD_ID_ENTRY_T FullCANSection[] = {
	{CAN_CTRL_NO, 0, 0x03},
	{CAN_CTRL_NO, 0, 0x05},
	{CAN_CTRL_NO, 0, 0x07},
	{CAN_CTRL_NO, 0, 0x09},
};
#endif
CAN_STD_ID_ENTRY_T SFFSection[] = {
	{CAN_CTRL_NO, 0, 0x30},
	{CAN_CTRL_NO, 0, 0x50},
	{CAN_CTRL_NO, 0, 0x70},
	{CAN_CTRL_NO, 0, 0x90},
	{CAN_CTRL_NO, 0, 0xB0},
};
CAN_STD_ID_RANGE_ENTRY_T SffGrpSection[] = {
	{{CAN_CTRL_NO, 0, 0x300}, {CAN_CTRL_NO, 0, 0x400}},
	{{CAN_CTRL_NO, 0, 0x500}, {CAN_CTRL_NO, 0, 0x600}},
	{{CAN_CTRL_NO, 0, 0x700}, {CAN_CTRL_NO, 0, 0x780}},
};
CAN_EXT_ID_ENTRY_T EFFSection[] = {
	{CAN_CTRL_NO, ((1 << 11) | 0x03)},
	{CAN_CTRL_NO, ((1 << 11) | 0x05)},
	{CAN_CTRL_NO, ((1 << 11) | 0x07)},
	{CAN_CTRL_NO, ((1 << 11) | 0x09)},
};
CAN_EXT_ID_RANGE_ENTRY_T EffGrpSection[] = {
	{{CAN_CTRL_NO, ((1 << 11) | 0x300)}, {CAN_CTRL_NO, ((1 << 11) | 0x400)}},
	{{CAN_CTRL_NO, ((1 << 11) | 0x500)}, {CAN_CTRL_NO, ((1 << 11) | 0x600)}},
	{{CAN_CTRL_NO, ((1 << 11) | 0x700)}, {CAN_CTRL_NO, ((1 << 11) | 0x780)}},
};

CANAF_LUT_T AFSections = {
#if FULL_CAN_AF_USED
	FullCANSection, sizeof(FullCANSection) / sizeof(CAN_STD_ID_ENTRY_T),
#else
	NULL, 0,
#endif
	SFFSection, sizeof(SFFSection) / sizeof(CAN_STD_ID_ENTRY_T),
	SffGrpSection, sizeof(SffGrpSection) / sizeof(CAN_STD_ID_RANGE_ENTRY_T),
	EFFSection, sizeof(EFFSection) / sizeof(CAN_EXT_ID_ENTRY_T),
	EffGrpSection, sizeof(EffGrpSection) / sizeof(CAN_EXT_ID_RANGE_ENTRY_T),
};
#endif /*AF_LUT_USED*/

/*** Funciones ***/
/* Print Error */
static void PrintCANErrorInfo(uint32_t Status)
{
	if (Status & CAN_ICR_EI) {
		DEBUGOUT("Error Warning!\r\n");
	}
	if (Status & CAN_ICR_DOI) {
		DEBUGOUT("Data Overrun!\r\n");
	}
	if (Status & CAN_ICR_EPI) {
		DEBUGOUT("Error Passive!\r\n");
	}
	if (Status & CAN_ICR_ALI) {
		DEBUGOUT("Arbitration lost in the bit: %d(th)\r\n", CAN_ICR_ALCBIT_VAL(Status));
	}
	if (Status & CAN_ICR_BEI) {

		DEBUGOUT("Bus error !!!\r\n");

		if (Status & CAN_ICR_ERRDIR_RECEIVE) {
			DEBUGOUT("\t Error Direction: Transmiting\r\n");
		}
		else {
			DEBUGOUT("\t Error Direction: Receiving\r\n");
		}

		DEBUGOUT("\t Error Location: 0x%2x\r\n", CAN_ICR_ERRBIT_VAL(Status));
		DEBUGOUT("\t Error Type: 0x%1x\r\n", CAN_ICR_ERRC_VAL(Status));
	}
}

/* Reply message received */
static void ReplyNormalMessage(CAN_MSG_T *pRcvMsg)
{
	CAN_MSG_T SendMsgBuf = *pRcvMsg;
	CAN_BUFFER_ID_T   TxBuf;

	SendMsgBuf.ID = CAN_TX_MSG_STD_ID;
	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);

	DEBUGOUT("Message Replied\r\n");
}

/* Setup AF LUT */
static void SetupAFLUT(void)
{
	Chip_CAN_SetAFLUT(LPC_CANAF, LPC_CANAF_RAM, &AFSections);
}

/*** Interrupciones ***/
//CAN
void CAN_IRQHandler(void)
{
	uint32_t IntStatus;
	CAN_MSG_T RcvMsgBuf;

	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN);

	PrintCANErrorInfo(IntStatus);

	/* Nuevo Msg */
	if (IntStatus & CAN_ICR_RI) {
		Chip_CAN_Receive(LPC_CAN, &RcvMsgBuf);
		DEBUGOUT("Message Received\r\n");

		// Enviar por LIN para saber qué LED prender RcvMsgBuf.Data
		flag_LED = RcvMsgBuf.Data[0];

		ReplyNormalMessage(&RcvMsgBuf);
	}

}

//Systick
void SysTick_Handler(void){

	if(ticks == 1){
		ticks = 0;
	}else{
		ticks = 1;
	}
}

/*** Main ***/
int main(void){
	Status aux;
	uint32_t auxstat;
	CAN_BUFFER_ID_T   TxBuf;
	CAN_MSG_T SendMsgBuf;

	/* Init */
	SystemCoreClockUpdate();
	Board_Init();

	//Systick - Para cada 1seg mandar el estado del pote
	SysTick_Config(SystemCoreClock / TICKRATE);

	//Pines CAN I/O
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 4);	//RD2
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 5);	//TD2

	//Pines CAN: RD2 P0.4 TD2 P0.5
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 4, IOCON_FUNC2);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 5, IOCON_FUNC2);

	//CANCAN
	Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);
	Chip_CAN_SetBitRate(LPC_CAN, 500000); 				//500kbits/s
	Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);

#if AF_LUT_USED
	SetupAFLUT();

#if FULL_CAN_AF_USED
	Chip_CAN_ConfigFullCANInt(LPC_CANAF, ENABLE);
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_FULL_MODE);
#else
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_NORMAL_MODE);
#endif /*FULL_CAN_AF_USED*/
#else
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_BYBASS_MODE);
#endif /*AF_LUT_USED*/

	NVIC_EnableIRQ(CAN_IRQn);

	while (1){

		//Tengo que enviar lo del pote
		if(ticks == 1){
			SendMsgBuf.ID = CAN_TX_MSG_STD_ID;
			SendMsgBuf.DLC = 4;
			SendMsgBuf.Type = 0;
			SendMsgBuf.Data[0] = 'P';	//Estado del pote - recibido por LIN
			SendMsgBuf.Data[1] = 'U';
			SendMsgBuf.Data[2] = 'T';
			SendMsgBuf.Data[3] = 'O';

			TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);

			aux = Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);

			auxstat = Chip_CAN_GetStatus(LPC_CAN);
			while ((auxstat & CAN_SR_TCS(TxBuf)) == 0) {
				auxstat = Chip_CAN_GetStatus(LPC_CAN);
			}
			DEBUGOUT("Sent\r\n");

			ticks = 0;
		}

		if(flag_LED != 0){
			/* Código a enviar por LIN:
			 * 0x01 -> LED1 Prender
			 * 0x02 -> LED2 Prender
			 * 0x11 -> LED1 Apagar
			 * 0x12 -> LED2 Apagar
			 * */
		}
	}

}
