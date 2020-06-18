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
#define		led1_STATUS		ifpinp( P5, 4, ACTIVO_ALTO )
#define     led2_ON			setbitp( P5, 5, ACTIVO_ALTO )
#define     led2_OFF		clrbitp( P5, 5, ACTIVO_ALTO )
#define		led3_ON			setbitp( P5, 6, ACTIVO_ALTO )
#define		led3_OFF		clrbitp( P5, 6, ACTIVO_ALTO )
#define		led4_ON			setbitp( P5, 7, ACTIVO_ALTO )
#define		led4_OFF		clrbitp( P5, 7, ACTIVO_ALTO )

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

//-----------------------------------------------------------------------------------------------
// Prototipos de funciones
//-----------------------------------------------------------------------------------------------
void		delay				( unsigned int tiempo );
void		PLACA_Init	    	( void );

void 		reset_system_timer  ( void );
void 		check_status_sensor ( float );
void 		check_board_live 	( void );
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