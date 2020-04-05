/*===============================================================================
 PROBLEMA 3.1
 Inicializar UART 3 por polling. Luego hacer eco. Con valor 27 termina.
 Versión: 0.0
 Curso: R4054
 Fecha: 09/07/12
 Ing. Ruben M. Lozano
 ==============================================================================*/

#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_gpio.h"
#include "stdio.h"
#include "stdlib.h"

//------------------Defines y Macros para GPIO---------------------
#define TXD0_PIN 2 // Pin de TDX0
#define RXD0_PIN 3 // Pin de RXD0
#define TXD0_BIT_MASK  (1 << TXD0_PIN) //TXD0
#define RXD0_BIT_MASK  (1 << RXD0_PIN) //RXD0

#define BOTON_PORT 0
#define BOTON_PIN 21
#define BOTON_BIT_MASK (1 << BOTON_PIN)

#define CS_PORT 1
#define CS_PIN 30
#define CS_BIT_MASK (1 << CS_PIN)

#define AWAKE_PORT 1
#define AWAKE_PIN 31
#define AWAKE_BIT_MASK (1 << AWAKE_PIN)

#define UART0_PORT 0

#define SALIDA 1
#define ENTRADA 0

//------------------Defines y Macros para UART---------------------
#define TEST_UART LPC_UART0

//------------------Defines y Macros para TIMER---------------------
#define TEST_TIM  LPC_TIM0

//-------------------Defines para LIN--------------------
#define MLin_Data  8 	// Cantidad de bytes de datos por frame
//#define BaudRate  9600 	// 9600 bps (Tb = 104.16 us)
#define BaudRate  4800 	// 9600 bps (Tb = 104.16 us)
#define Tb_uS 202 		// Tb = 104 uS (1/9600)
#define Bytes8Data 3 	// Campo para ID

#define LED0 0

#define APAGAR 0
#define PRENDER 255

struct{
	//SYNC BREAK
	uint8_t sync ; //Sync Field
	uint8_t ID ; // Id Field
	uint8_t DataBytes[MLin_Data] ; // Data bytes
	uint8_t CRC ; //Check Sum
}typedef LIN_frame;


//---------------------APIs de Peripherals------------------------
void Set_UART_0 (UART_FITO_LEVEL_Type LV) ;  // Habilitación de UART 0

void Set_TXD0_GPIO (void) ; // Habilitación de TXD0 como GPIO
void Set_RXD0_GPIO (void)  ; // Habilitación de RXD0 como GPIO

void Set_TIM0 (void) ;  // Habilitación de Timer 0
void TIMER0_IRQHandler(void) ; // Handler de Timer 0

//----------------------Apis de Lin-----------------------
void LIN_HardInit (void) ;  //inicialzación de Hardware

void LIN_MakeFrame (LIN_frame *frame, uint8_t ID, uint8_t *buffer); //Inicialización de LIN_frame
uint8_t LIN_Read (uint8_t *buffer, LIN_frame *frame) ; // Lectura de frame

uint8_t Make_CRC (uint8_t *data_frame) ; // Armado de campo CRC
uint8_t Make_ID (uint8_t ID) ; //Armado de ID

//Transmisión
void Breack_TXD0 (void) ; // Break bits de transmición
uint8_t Breack_RXD0 (void) ; // Break bits de recepción

uint8_t LIN_Send(LIN_frame *frame) ; // Envío bloqueante de un frame

//Recepción
uint8_t LIN_Receive(LIN_frame *frame) ; // Recepción bloqueante de un frame

//----------------------Variables globales-----------------------
int	Tick_Count = 0 ; // Cuenta de ticks de Timer
int Breack_end = 0 ; // Flag de Breack End
int Bits_Breack = 0 ; // flag apra arrancar a usar Timer
PINSEL_CFG_Type PinCfg_UART0_RX, PinCfg_UART0_TX;

int main(void)
{
	int i; uint8_t LED_Status = APAGAR;
	uint8_t array[MLin_Data] = {APAGAR,1,0,1,0,1,0,1};
	LIN_frame frame_tx;

	PinCfg_UART0_TX.Funcnum = 1;     // De PINSEL0 , función 01b, para TXD0
	PinCfg_UART0_TX.OpenDrain = 0;   //No salida open drain
	PinCfg_UART0_TX.Pinmode = 0;     //Sin pull up
	PinCfg_UART0_TX.Pinnum = 2;      //Pin 0 para TXD0
	PinCfg_UART0_TX.Portnum = 0;     //Port 0

	PinCfg_UART0_RX.Funcnum = 1;     // De PINSEL0 , función 01b, para TXD0
	PinCfg_UART0_RX.OpenDrain = 0;   //No salida open drain
	PinCfg_UART0_RX.Pinmode = 0;     //Sin pull up
	PinCfg_UART0_RX.Pinnum = 3;      //Pin 0 para RXD0
	PinCfg_UART0_RX.Portnum = 0;     //Port 0


	// Armo Frame
	LIN_MakeFrame(&frame_tx , 1, array);

	// Inicializo Hardware para LIN
	LIN_HardInit();

     while(1)
     {
    	// Envío mensaje
    	LIN_Send(&frame_tx);

 		//LED_Status = GPIO_ReadValue(BOTON_PORT) ; // Leo pin
 		//LED_Status = ((LED_Status >> BOTON_PIN) & 1) ;

    	if(LED_Status == APAGAR)
    		LED_Status = PRENDER;
    	else
    		LED_Status = APAGAR;

    	frame_tx.DataBytes[0] = ~LED_Status;

    	for(i=0; i<10000; i++);
     }

/*
	while(1){
		// Configuro pin P0[3] como GPIO
		Set_RXD0_GPIO() ; // Configuro RXD0 como GPIO
		if(LIN_Receive(&frame_rx)){
			for(i=0;i<10000;i++);
		}
		else {
			for(i=0;i<10000;i++);
		}
	}
*/

     return 1;
 }


//-------------------------------APIs de GPIO------------------------------------
void Set_TXD0_GPIO (void) {

	PINSEL_CFG_Type PinCfg; // defino variable tipo estructura PINSEL_CFG_Type

	// Inicializo  pin connect para GPIO
	PinCfg.Funcnum = 0;     // De PINSEL0 , función 00b, para GPIO
	PinCfg.OpenDrain = 0;   //No salida open drain
	PinCfg.Pinmode = 0;     //Sin pull up
	PinCfg.Pinnum = 2;      //Pin 0 para TXD0
	PinCfg.Portnum = 0;     //Port 0
	PINSEL_ConfigPin(&PinCfg); //cargo el registro PINSEL con los valores anteriores

	GPIO_SetDir(UART0_PORT, TXD0_BIT_MASK , SALIDA);  	//Configuro el pin como salida

	return;
}

void Set_RXD0_GPIO (void) {

	PINSEL_CFG_Type PinCfg; // defino variable tipo estructura PINSEL_CFG_Type

	// Inicializo  pin connect para GPIO
	PinCfg.Funcnum = 0;     // De PINSEL0 , función 00b, para GPIO
	PinCfg.OpenDrain = 0;   //No salida open drain
	PinCfg.Pinmode = 0;     //Sin pull up
	PinCfg.Pinnum = 2;      //Pin 0 para TXD0
	PinCfg.Portnum = 0;     //Port 0
	PINSEL_ConfigPin(&PinCfg); //cargo el registro PINSEL con los valores anteriores

	GPIO_SetDir(UART0_PORT, RXD0_BIT_MASK , ENTRADA);  	//Configuro el pin como salida

	return;
}

//-------------------------------APIs de Timer------------------------------------

void Set_TIM0 (void) {

	uint32_t TClock ; // Tiempo de tick
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct ;

	// Obtención de clock del sistema
	SystemCoreClockUpdate();
	TClock = Tb_uS; // necesario el casteo

	// Cargo los campos de la estructura para un prescaler count time de Tb
    TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL; //valor en microsegundos.
    TIM_ConfigStruct.PrescaleValue  = 1 ; // Directo sin preescaler

    // Cargo los registros de configuración del TIMER 0
	TIM_Init(TEST_TIM, TIM_TIMER_MODE, &TIM_ConfigStruct);

	// Use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType = TIM_EXTMATCH_NOTHING ;
	// Set Match value, count value of 104 uS
	TIM_MatchConfigStruct.MatchValue = TClock ;
	// Cargo los registros de MATCH del TIMER 0
	TIM_ConfigMatch(TEST_TIM,&TIM_MatchConfigStruct);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
	/* Enable interrupt for timer 0 */
	NVIC_EnableIRQ(TIMER0_IRQn);

	// Arranco timer 0
	TIM_Cmd(TEST_TIM,ENABLE);

	return;

}

void TIMER0_IRQHandler(void) {

			if(Tick_Count)  {
				Tick_Count--;
				if(!Tick_Count) Breack_end = 1;
				else Breack_end = 0;
			}
			else Breack_end = 0;

	/*if(Tick_Count) GPIO_SetValue(UART0_PORT, TXD0_BIT_MASK );
	else GPIO_ClearValue(UART0_PORT, TXD0_BIT_MASK );

	Tick_Count = ~Tick_Count;*/


	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);

}

//-------------------------------APIs de UART------------------------------------

void Set_UART_0 (UART_FITO_LEVEL_Type LV)
{
	UART_CFG_Type UARTConfigStruct; // defino variable tipo estructura UART_CFG_Type

	UART_FIFO_CFG_Type UARTFIFOConfigStruct; // defino variable tipo estructura UART_FIFO_CFG_Type

	PINSEL_CFG_Type PinCfg; // defino variable tipo estructura PINSEL_CFG_Type

	// Inicializo  pin connect para UART0
/*
	PinCfg.Funcnum = 1;     // De PINSEL0 , función 01b, para TXD0
	PinCfg.OpenDrain = 0;   //No salida open drain
	PinCfg.Pinmode = 0;     //Sin pull up
	PinCfg.Pinnum = 2;      //Pin 0 para TXD0
	PinCfg.Portnum = 0;     //Port 0
	PINSEL_ConfigPin(&PinCfg); //cargo el registro PINSEL con los valores anteriores
*/

	PinCfg.Pinnum = 3;      //Pin 1 para RXD0, más el resto de los campos anteriores
	PINSEL_ConfigPin(&PinCfg);

	// Cargo los parametros en la estructura de configuración.
	// Por default: Baudrate = 9600bps, 8 data bit, 1 Stop bit, None parity
	//UART_ConfigStructInit(&UARTConfigStruct);
	UARTConfigStruct.Baud_rate = BaudRate ;
    UARTConfigStruct.Databits = UART_DATABIT_8 ;
	UARTConfigStruct.Parity = UART_PARITY_NONE ;
	UARTConfigStruct.Stopbits = UART_STOPBIT_1 ;

	// Inicializo la UART según los parametros anteriores.
	UART_Init((LPC_UART_TypeDef*)TEST_UART, &UARTConfigStruct);

	// Cargo los parametros en la estructura para la FIFO.
	// Por default: FIFO_DMAMode = DISABLE, FIFO_Level = UART_FIFO_TRGLEV2
	// FIFO_ResetRxBuf = ENABLE, FIFO_ResetTxBuf = ENABLE, FIFO_State = ENABLE
	UARTFIFOConfigStruct.FIFO_DMAMode = DISABLE ;
	UARTFIFOConfigStruct.FIFO_Level = LV ; // FIFO con 8 caracteres
	UARTFIFOConfigStruct.FIFO_ResetRxBuf = ENABLE ;
	UARTFIFOConfigStruct.FIFO_ResetTxBuf = ENABLE ;
 	// Inicializo la FIFO según los parametros anteriores.
 	UART_FIFOConfig((LPC_UART_TypeDef*)TEST_UART, &UARTFIFOConfigStruct);

 	// Habilito transmision en pin TXD
 	UART_TxCmd((LPC_UART_TypeDef*)TEST_UART, ENABLE);

 	return;
}

//------------------------------APIs de LIN-----------------------------

void LIN_HardInit (void) {
	int i;
	// Configuro pin P0[2] como GPIO
	Set_TXD0_GPIO() ; // Configuro TXD0 como GPIO

	// Inicializo Timer y lo arranco con los 13 bits
	Set_TIM0() ;

	// Configuro UART para mandar
	Set_UART_0(UART_FIFO_TRGLEV2);

	// Inicio Pin en estado alto
	GPIO_SetValue(UART0_PORT, TXD0_BIT_MASK );

	//Pin boton como entrada
	GPIO_SetDir(BOTON_PORT, BOTON_BIT_MASK , ENTRADA);

	//CS y ~Wake como Salida
	GPIO_SetDir(CS_PORT, CS_BIT_MASK , SALIDA);
	GPIO_SetDir(AWAKE_PORT, AWAKE_BIT_MASK , SALIDA);

	//Activo ~Wake
	GPIO_ClearValue(AWAKE_PORT, AWAKE_BIT_MASK);	//Fran-Euge Ponerlo en 1 por el TBJ
	//Delay
	for(i=0; i<100; i++);
	//Activo CS
	GPIO_SetValue(CS_PORT, CS_BIT_MASK);

	return;
}


void LIN_MakeFrame (LIN_frame *frame, uint8_t ID, uint8_t *buffer){

	int i;

	frame->sync = 85 ; // 0x55h

	frame->ID = Make_ID(ID) ;

	for(i = 0 ; i < MLin_Data; i++) frame->DataBytes[i] = buffer[i] ;

	frame->CRC = Make_CRC(buffer) ;

  return ;
}


uint8_t LIN_Read (uint8_t *buffer, LIN_frame *frame){

	int i;
	uint8_t aux_crc, aux_id;
	uint8_t aux_par0[4], aux_par1[4];
	uint8_t aux_p1, aux_p0, aux_p;

	aux_crc = Make_CRC(frame->DataBytes) ;

	aux_id = (frame->ID & (uint8_t)3) ; //rescato lso 2 bits menos significativos

	//P0
	aux_par0[0] = (((frame->ID) >> 3) & 1);
	aux_par0[1] = (((frame->ID) >> 5) & 1);
	aux_par0[2] = (((frame->ID) >> 6) & 1);
	aux_par0[3] = (((frame->ID) >> 7) & 1);
	aux_p0 = aux_par0[0]^aux_par0[1]^aux_par0[2]^aux_par0[3] ;
	//P1
	aux_par1[0] = (((frame->ID) >> 2) & 1);
	aux_par1[1] = (((frame->ID) >> 3) & 1);
	aux_par1[2] = (((frame->ID) >> 4) & 1);
	aux_par1[3] = (((frame->ID) >> 6) & 1);
	aux_p1 = aux_par1[0]^aux_par1[1]^aux_par1[2]^aux_par1[3] ;
	aux_p1 = ~ aux_p1 ;
	//P0 P1
	aux_p = aux_p1 + aux_p0*2 ;

	if((aux_p == aux_id) && (aux_crc == frame->CRC)) {
		for(i = 0 ; i < MLin_Data; i++) buffer[i] = frame->DataBytes[i] ;
		return(1); // Frame correcto
	}
	else return(0); //Frame incorrecto

}

uint8_t Make_CRC (uint8_t *array){

	int i;
	uint8_t aux_crc;

	aux_crc = 0 ;

	for(i = 0; i < MLin_Data ; i++) {
		aux_crc += array[i];
		if(aux_crc >= 256) aux_crc -= 255 ;
	}

	return(aux_crc);

}

uint8_t Make_ID (uint8_t ID){

	uint8_t aux_id;
	uint8_t aux_par0[4], aux_par1[4];
	uint8_t aux_p1, aux_p0, aux_p;

	//P0
	aux_par0[0] = ((ID >> 3) & 1);
	aux_par0[1] = ((ID >> 5) & 1);
	aux_par0[2] = ((ID >> 6) & 1);
	aux_par0[3] = ((ID >> 7) & 1);
	aux_p0 = aux_par0[0]^aux_par0[1]^aux_par0[2]^aux_par0[3] ;
	//P1
	aux_par1[0] = ((ID >> 2) & 1);
	aux_par1[1] = ((ID >> 3) & 1);
	aux_par1[2] = ((ID >> 4) & 1);
	aux_par1[3] = ((ID >> 6) & 1);
	aux_p1 = aux_par1[0]^aux_par1[1]^aux_par1[2]^aux_par1[3] ;
	aux_p1 = ~ aux_p1 ;
	//P0 P1
	aux_p = aux_p1 + aux_p0*2 ;

	aux_id = 0;

	aux_id = aux_p + (Bytes8Data<<2) + (ID<<4) ;

	return(aux_id);
}

void Breack_TXD0 (void){

	// Configuro pin P0[2] como GPIO
	Set_TXD0_GPIO() ; // Configuro TXD0 como GPIO

	// Pongo pin a estado bajo
	GPIO_ClearValue(UART0_PORT, TXD0_BIT_MASK );

	//Arranco Timer
	Tick_Count = 13 ; // 13 bits

	// Espero fin de los 13 Bits
	while(Breack_end == 0) ;
	//Breack_end = 0 ; // limpio Flag

	// Pongo pin a estado alto
	//GPIO_SetValue(UART0_PORT, TXD0_BIT_MASK );

/*	//Arranco Timer
	Tick_Count = 1 ;

	//Espero fin de 1 bit
	while(Breack_end == 0) ;
	Breack_end = 0 ; // limpio Flag

	// Pongo pin a estado bajo
	GPIO_ClearValue(UART0_PORT, TXD0_BIT_MASK );*/

	return;
}

uint8_t Breack_RXD0 (void){

	uint32_t aux;

	aux = GPIO_ReadValue (RXD0_BIT_MASK) ; // Leo pin
	aux = ((aux >> RXD0_PIN) & 1) ;

	if(!aux)  Tick_Count = 13;

	// Sync Break
	while(Breack_end == 0){
		aux = GPIO_ReadValue (RXD0_BIT_MASK) ; // Leo pin
		aux = ((aux >> RXD0_PIN) & 1) ;
		if (aux == 1) return(0) ; // Si hubo un error en sync breack retorno con error
	}
	Breack_end = 0 ; // Limpio flag

	return(1);
}


uint8_t LIN_Send(LIN_frame *frame){
	//PINSEL_CFG_Type PinCfg; // defino variable tipo estructura PINSEL_CFG_Type

	// Sync Breack
	//Breack_TXD0();

	Set_TXD0_GPIO() ; // Configuro TXD0 como GPIO

	// Pongo pin a estado bajo
	GPIO_ClearValue(UART0_PORT, TXD0_BIT_MASK );

	//Arranco Timer
	Tick_Count = 13 ; // 13 bits

	// Espero fin de los 13 Bits
	while(Breack_end == 0) ;

	PINSEL_ConfigPin(&PinCfg_UART0_TX); //cargo el registro PINSEL con los valores anteriores

	// Sync Field
	UART_Send((LPC_UART_TypeDef*)TEST_UART, &(frame->sync), sizeof(frame->sync), BLOCKING);

	// ID Field
	UART_Send((LPC_UART_TypeDef*)TEST_UART, &(frame->ID), sizeof(frame->ID), BLOCKING);

	// Configuro UART para mandar 8 bytes
	//Set_UART_0(UART_FIFO_TRGLEV2);

	// Sync Field
	UART_Send((LPC_UART_TypeDef*)TEST_UART, frame->DataBytes, sizeof(frame->DataBytes), BLOCKING);

	// Configuro UART para mandar 1 byte
	//Set_UART_0(UART_FIFO_TRGLEV0);

	// Sync Field
	UART_Send((LPC_UART_TypeDef*)TEST_UART, &(frame->CRC), sizeof(frame->CRC), BLOCKING);


	return(1);
}

uint8_t LIN_Receive(LIN_frame *frame){

	// Sync Breack
	if(Breack_RXD0()){

	// Configuro UART para recibir
	PINSEL_ConfigPin(&PinCfg_UART0_RX); //cargo el registro PINSEL con los valores anteriores

	// Sync Field
	UART_Receive((LPC_UART_TypeDef*)TEST_UART, &(frame->sync), sizeof(frame->sync), BLOCKING);

	// Sync Field
	UART_Receive((LPC_UART_TypeDef*)TEST_UART, &(frame->ID), sizeof(frame->ID), BLOCKING);

	// Data Bytes
	UART_Receive((LPC_UART_TypeDef*)TEST_UART, frame->DataBytes, sizeof(frame->DataBytes), BLOCKING);

	// CRC Field
	UART_Receive((LPC_UART_TypeDef*)TEST_UART, &(frame->CRC), sizeof(frame->CRC), BLOCKING);

	static int x = 0;
	if(frame->sync != 85) {
		x++;
		return(0);
	}

	return(1);

	}

	else return(0);
}
