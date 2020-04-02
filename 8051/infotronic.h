#ifndef		INFOTRONIC_H
#define		INFOTRONIC_H

//===============================================================================================
// Micro
//===============================================================================================
#ifdef		C8051F020
#include	<C8051F020.h>
//-----------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------
// Prototipos de funciones
//-----------------------------------------------------------------------------------------------
void		delay				( unsigned int tiempo );
void		PLACA_Init	    	( void );
//===============================================================================================

#endif