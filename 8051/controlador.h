#ifndef		CONTROLADOR_H
#define		CONTROLADOR_H

//===============================================================================================
// CONTROLADOR DE MOTOR DE DC EN FUNCION DE LA TEMPERATURA DEL LIQUIDO REFRIGERANTE
//===============================================================================================
//-----------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------
#ifdef		C8051F020
#include	<C8051F020.h>
#endif

#include	<ntcm12.h>		    	// Sensor de temperatura
#include	<fuzzy.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<intrins.h>
//-----------------------------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------------------------
#define 	ACTIVO_ALTO			0x01
#define 	ACTIVO_BAJO			0x00
#define 	ON					0x01
#define		OFF					0x00
#define		HIGH				0x01
#define		LOW					0x00
#define		TEMPERATURA			0
#define		HUMEDAD				1
#define		PRESION				2
#define		SENSOR_TEMPERATURA	0x9E
//-----------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------
// Macros para setear, limpiar y pullear bits en particular de puertos
#define		setbitp( puerto,pin,activo ) \
								(( puerto ) = ((( activo ) == ACTIVO_ALTO) ? \
								(( puerto ) | (0x01 << ( pin ))) : \
								(( puerto ) & ~(0x01 << ( pin )))))
#define		clrbitp( puerto,pin,activo ) \
								(( puerto ) = ((( activo ) == ACTIVO_BAJO) ? \
								(( puerto ) | (0x01 << ( pin ))) : \
								(( puerto ) & ~(0x01 << ( pin )))))
#define		ifpinp( puerto,pin,estado ) \
								((((( puerto ) >> ( pin )) & 0x01) == \
								( estado )) ? ON : OFF)

//Macros varias para los 4 leds	del kit de desarrollo
#define		led1_ON			setbitp( P5, 4, ACTIVO_ALTO )
#define		led1_OFF		clrbitp( P5, 4, ACTIVO_ALTO )
#define		led1_STATUS		ifpinp ( P5, 4, ACTIVO_ALTO )
#define     led2_ON			setbitp( P5, 5, ACTIVO_ALTO )
#define     led2_OFF		clrbitp( P5, 5, ACTIVO_ALTO )
#define		led3_ON			setbitp( P5, 6, ACTIVO_ALTO )
#define		led3_OFF		clrbitp( P5, 6, ACTIVO_ALTO )
#define		led4_ON			setbitp( P5, 7, ACTIVO_ALTO )
#define		led4_OFF		clrbitp( P5, 7, ACTIVO_ALTO )
#define		led4_STATUS		ifpinp ( P5, 7, ACTIVO_ALTO )

//Macros para saber el estado de los botonoes
#define		tact0_STATUS		ifpinp( P5, 0, ACTIVO_BAJO )
#define		tact1_STATUS		ifpinp( P5, 1, ACTIVO_BAJO )
#define		tact2_STATUS		ifpinp( P5, 2, ACTIVO_BAJO )
#define		tact3_STATUS		ifpinp( P5, 3, ACTIVO_BAJO )

//Macros para frenar el motor
#define 	freno_OFF		setbitp( P1, 0, ACTIVO_ALTO )
#define		freno_ON		clrbitp( P1, 0, ACTIVO_ALTO )

//Macros para manejar el PWM
#define		PWM_ENABLE		(PCA0CN    = 0x40)
#define 	PWM_DISABLE		(PCA0CN    = 0x00)
#define		PMW_0_DUTY		(PCA0CPM0 = PCA0CPM0 & 0xBF)
#define		PWM_ON			(PCA0CPM0 = PCA0CPM0 | 0x42)
#define		PWM_UP			(PCA0CPH0--)
#define		PWM_SPEED_UP	(PCA0CPH0--)
#define		PWM_SPEED_DOWN	(PCA0CPH0++)
#define		PWM_DOWN		(PCA0CPH0++)
#define		PWM_VAL_INI		(PCA0CPH0  = 0x10)
#define		PWM_MAX_STATUS	(PCA0CPH0  == 0xE6)
//#define		PWM_PIN_0		(clrbitp(P0, 2, ACTIVO_ALTO))

//Define para el PCA0
#define 	TIME_13BITS		2400 //1300 //2500
#define		PCA_ON_POSEDGE	(PCA0CPM1  = 0x21)
#define		PCA_ON_NEGEDGE 	(PCA0CPM1  = 0x11)
#define		PCA_INT_ON		(EIE1 = EIE1 | 0x08)
#define		PCA_INT_OFF		(EIE1 = EIE1 & 0xF7)

//Macros para la UART
#define ARRANCAR_TX		(SCON1 = SCON1 | 0x02)
#define TX				(SCON1 & 0x02)
#define RX				(SCON1 & 0x01)
#define APAGAR_TX		(SCON1 = SCON1 & 0xFD )
#define APAGAR_RX		(SCON1 = SCON1 & 0xFE )
#define BUFER_RX_TX		SBUF1

#define ESPERA			0
#define	SYNC_BREAK_OK	1
#define	REC_DATA		2
#define	SEND_DATA		3
//Necesito recibir el estado de aceleracion, con un byte me alcanza
//Necesito transmitir la temperatura y algun coso de diagnostico, 2 bytes
//entonces puedo usar los ID 0 y 1
//que con los bits de paridad se convierten en:
#define ID_REC_DATA		0x80
#define ID_SEND_DATA	0xC1
//segun la norma

#define MAX_REC_BYTES	3

#define AVISO_ON	setbitp( P0, 6, ACTIVO_ALTO)
#define AVISO_OFF	clrbitp( P0, 6, ACTIVO_ALTO)
#define	LIN_CS_ON	setbitp( P1, 2, ACTIVO_ALTO)		
#define	LIN_CS_OFF	clrbitp( P1, 2, ACTIVO_ALTO)
#define	LIN_WK_ON	setbitp( P1, 4, ACTIVO_ALTO)		
#define	LIN_WK_OFF	clrbitp( P1, 4, ACTIVO_ALTO)


//-----------------------------------------------------------------------------------------------
// Prototipos de funciones
//-----------------------------------------------------------------------------------------------
void		delay				( unsigned int tiempo );
void		PLACA_Init	    	( void );

void 		reset_system_timer  ( void );
void 		check_status_sensor ( float );
void 		check_board_live 	( void );

void 		fuzzy_engine(void);
unsigned char compute_memval(unsigned char,unsigned char );
void defuzzify(void);
void normalize (void);

void PushTx(unsigned char );
void PushRx(unsigned char );
int PopRx (void);
int PopTx (void);
void Transmitir( char *);
void Mensajes (void );
unsigned char calculate_checksum( unsigned char, unsigned char );
//===============================================================================================
//-----------------------------------------------------------------------------------------------
// Globales
//-----------------------------------------------------------------------------------------------
#ifdef		APLICACION
	data volatile float temperatura_medida = 0;
#endif
//-----------------------------------------------------------------------------------------------
// Prototipos de funciones
//-----------------------------------------------------------------------------------------------

//===============================================================================================

#endif		/* EMA_H */